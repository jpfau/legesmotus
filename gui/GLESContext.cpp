/*
 * gui/GLESContext.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "GLESContext.hpp"
#include "Widget.hpp"
#include "common/math.hpp"
#include "common/Exception.hpp"

#include "SDL.h"

using namespace LM;

const GLint GLESContext::m_rect_tex_vertices[] = {
	0, 0,
	1, 0,
	1, 1,
	0, 1
};

GLESContext::GLESContext(int width, int height) {
	m_width = width;
	m_height = height;

	m_stencil_depth = 0;
	m_stencil_type = GL_GEQUAL;

	m_bound_img = 0;
	m_img_bound = false;

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glViewport(0, 0, width, height);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	set_blend_mode(BLEND_NORMAL);
}

GLESContext::~GLESContext() {
}

void GLESContext::make_active() {
	glBindTexture(GL_TEXTURE_2D, m_bound_img);
	glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);
	set_blend_mode(m_mode);
}

void GLESContext::update_stencil() {
	glStencilFunc(m_stencil_type, m_stencil_depth + m_stencil_func, 0xFF);
}

void GLESContext::prepare_arc(float len, float xr, float yr, int fine) {
	m_arc_vertices[0] = 0.0;
	m_arc_vertices[1] = 0.0;

	if (fine > MAX_ARC_FINE) {
		fine = MAX_ARC_FINE;
	}
	for (int i = 0; i <= fine; ++i) {
		m_arc_vertices[(i + 1)*2 + 0] = xr*cos(len*i*2.0*M_PI/fine);
		m_arc_vertices[(i + 1)*2 + 1] = yr*sin(len*i*2.0*M_PI/fine);
	}
	glVertexPointer(2, GL_FLOAT, 0, m_arc_vertices);
}

void GLESContext::prepare_rect(float w, float h) {
	m_rect_vertices[0] = -w/2.0f;
	m_rect_vertices[1] = -h/2.0f;
	m_rect_vertices[2] = w/2.0f;
	m_rect_vertices[3] = -h/2.0f;
	m_rect_vertices[4] = w/2.0f;
	m_rect_vertices[5] = h/2.0f;
	m_rect_vertices[6] = -w/2.0f;
	m_rect_vertices[7] = h/2.0f;
	glVertexPointer(2, GL_FLOAT, 0, m_rect_vertices);
}

void GLESContext::draw_subimage(int width, int height, float tex_x, float tex_y, float tex_width, float tex_height) {
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glTranslatef(-tex_x/tex_width, -tex_y/tex_height, 0);
	glScalef(width/(tex_width), height/(tex_height), 1.0);

	glTexCoordPointer(2, GL_INT, 0, m_rect_tex_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

unsigned char* GLESContext::setup_texture(PixelFormat fmt, const unsigned char* data,
										  int* w, int* h,
										  GLint* bpc, GLint* ifmt,
										  GLenum* glfmt, GLenum* type) {
	int nwidth = to_pow_2(*w);
	int nheight = to_pow_2(*h);
	// 2px textures seem to break things
	if (nwidth < 4) {
		nwidth = 4;
	}
	if (nheight < 4) {
		nheight = 4;
	}

	// XXX don't break const correctness
	unsigned char* ndata = const_cast<unsigned char*>(data);

	switch (fmt) {
	case RGBA:
		*bpc = 4;
		*ifmt = 4;
		*glfmt = GL_RGBA;
		*type = GL_UNSIGNED_BYTE;
		break;
	case ALPHA:
		*bpc = 1;
		*ifmt = GL_ALPHA8;
		*glfmt = GL_ALPHA;
		*type = GL_UNSIGNED_BYTE;
		break;
	default:
		throw new Exception("Invalid image format");
	}
	

	if (nwidth != *w || nheight != *h) {
		ndata = new unsigned char[nwidth*nheight* *bpc];
		for (int y = 0; y < *h; ++y) {
			memcpy(&ndata[nwidth * y * *bpc], &data[*w * y * *bpc], *w * *bpc);
			memset(&ndata[(nwidth * y + *w)* *bpc], 0, (nwidth - *w) * *bpc);
		}
		for (int y = *h; y < nheight; ++y) {
			memset(&ndata[nwidth * y * *bpc], 0, nwidth * *bpc);
		}
	}

	*w = nwidth;
	*h = nheight;
	return ndata;
}

int GLESContext::get_width() const {
	return m_width;
}

int GLESContext::get_height() const {
	return m_height;
}

void GLESContext::load_identity() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_width, m_height, 0, -0x7FFF, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLESContext::push_transform() {
	glPushMatrix();
}

void GLESContext::pop_transform() {
	glPopMatrix();
}

void GLESContext::start_clip() {
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	++m_stencil_depth;
	clip_add();
}

void GLESContext::clip_add() {
	m_stencil_func = -1;
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
	update_stencil();

	//set_draw_color(Color(1, 0, 0, 1));
}

void GLESContext::clip_sub() {
	m_stencil_func = 0;
	glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
	update_stencil();

	//set_draw_color(Color(0, 0, 1, 1));
}

void GLESContext::finish_clip() {
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	--m_stencil_depth;
	update_stencil();
}

void GLESContext::invert_clip() {
	if (m_stencil_type == GL_GEQUAL) {
		m_stencil_type = GL_LEQUAL;
	} else if (m_stencil_type == GL_LEQUAL) {
		m_stencil_type = GL_GEQUAL;
	}

	update_stencil();
}

void GLESContext::push_clip() {
	++m_stencil_depth;
	update_stencil();
}

void GLESContext::pop_clip() {
	--m_stencil_depth;
	update_stencil();
}

int GLESContext::clip_depth() {
	return m_stencil_depth;
}

void GLESContext::translate(float x, float y) {
	glTranslatef(x, y, 0);
}

void GLESContext::scale(float x, float y) {
	glScalef(x, y, 0);
}

void GLESContext::rotate(float degrees) {
	glRotatef(degrees, 0, 0, 1);
}

void GLESContext::skew_x(float amount) {
	float mat[16] = {  1,      0, 0, 0,
					  -amount, 1, 0, 0,
					   0,      0, 1, 0,
					   0,      0, 0, 1 };
	glMultMatrixf(mat);
}

void GLESContext::skew_y(float amount) {
	float mat[16] = { 1, -amount, 0, 0,
					  0,       1, 0, 0,
					  0,       0, 1, 0,
					  0,       0, 0, 1 };
	glMultMatrixf(mat);
}

void GLESContext::set_draw_color(Color c) {
	glColor4f(c.r, c.g, c.b, c.a);
	m_color = c;
}

void GLESContext::set_blend_mode(BlendMode m) {
	switch (m) {
		case BLEND_NORMAL:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;

		case BLEND_MULTIPLY:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;

		case BLEND_ADD:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}
	m_mode = m;
}

void GLESContext::draw_arc(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_TRIANGLE_FAN, 0, fine + 2);
	glDrawArrays(GL_LINE_STRIP, 1, fine + 1);
}

void GLESContext::draw_arc_fill(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_TRIANGLE_FAN, 0, fine + 2);
}

void GLESContext::draw_arc_line(float len, float xr, float yr, int fine) {
	prepare_arc(len, xr, yr, fine);
	glDrawArrays(GL_LINE_STRIP, 1, fine + 1);
}

void GLESContext::draw_rect(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void GLESContext::draw_rect_fill(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLESContext::draw_rect_line(float w, float h) {
	prepare_rect(w, h);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void GLESContext::draw_line(float x1, float y1, float x2, float y2) {
	static GLfloat vertices[4];
	vertices[0] = x1;
	vertices[1] = y1;
	vertices[2] = x2;
	vertices[3] = y2;
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
}

void GLESContext::draw_lines(float vertices[], int n, bool loop) {
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(loop?GL_LINE_LOOP:GL_LINE_STRIP, 0, n);
}

DrawContext::Image GLESContext::gen_image(int* width, int* height, PixelFormat format, const unsigned char* data) {
	int w = *width;
	int h = *height;
	GLint	bpc;
	GLint	ifmt;
	GLenum	glfmt;
	GLenum	type;

	unsigned char* ndata = setup_texture(format, data, &w, &h, &bpc, &ifmt, &glfmt, &type);

	GLuint img;
	glGenTextures(1, &img);
	bind_image(img);glTexParameterf(GL_TEXTURE_2D,
		GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, ifmt, w, h, 0, glfmt, type, ndata);
	if (ndata != data) {
		delete[] ndata;
	}
	unbind_image();
	*width = w;
	*height = h;
	return img;
}

void GLESContext::add_mipmap(Image handle, int level, int* width, int* height, PixelFormat format, const unsigned char* data) {
	int nwidth = *width;
	int nheight = *height;
	GLint bpc;
	GLint ifmt;
	GLenum glfmt;
	GLenum type;
	unsigned char* ndata;
	ndata = setup_texture(format, data, &nwidth, &nheight, &bpc, &ifmt, &glfmt, &type);

	bind_image(handle);
	glTexImage2D(GL_TEXTURE_2D, level, ifmt, nwidth, nheight, 0, glfmt, type, ndata);
	if (ndata != data) {
		delete[] ndata;
	}
	unbind_image();
	*width = nwidth;
	*height = nheight;
}

void GLESContext::del_image(Image img) {
	glDeleteTextures(1, &img);
}

void GLESContext::draw_image(int width, int height, Image img) {
	bind_image(img);
	draw_bound_image(width, height);
	unbind_image();
}

void GLESContext::bind_image(Image img) {
	// XXX uncomment when graphics revamp is done
	//if (m_bound_img != img || !m_img_bound) {
		glBindTexture(GL_TEXTURE_2D, img);
		m_bound_img = img;
	//}
	if (!m_img_bound) {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		m_img_bound = true;
	}
}

void GLESContext::unbind_image() {
	if (m_img_bound) {
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		m_img_bound = false;
	}
}

void GLESContext::draw_bound_image(int width, int height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	GLint vertices[8] = {
		0, 0,
		width, 0,
		width, height,
		0, height
	};

	glVertexPointer(2, GL_INT, 0, vertices);
	glTexCoordPointer(2, GL_INT, 0, m_rect_tex_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLESContext::draw_bound_image_region(int width, int height,
										  float tex_x, float tex_y,
										  float tex_width, float tex_height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	GLint vertices[8] = {
		0, 0,
		width, 0,
		width, height,
		0, height
	};

	glVertexPointer(2, GL_INT, 0, vertices);

	draw_subimage(width, height, tex_x, tex_y, tex_width, tex_height);
}

void GLESContext::draw_bound_image_tiled(int width, int height,
										 float tex_x, float tex_y,
										 float tex_width, float tex_height) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLint vertices[8] = {
		0, 0,
		width, 0,
		width, height,
		0, height
	};

	glVertexPointer(2, GL_INT, 0, vertices);

	draw_subimage(width, height, tex_x, tex_y, tex_width, tex_height);
}

void GLESContext::redraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	load_identity();

	get_root_widget()->draw(this);

	SDL_GL_SwapBuffers();
}