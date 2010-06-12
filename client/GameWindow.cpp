/*
 * GameWindow.cpp
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

#include "GameWindow.hpp"
#include "compat_gl.h"
#include <cmath>
#include <iostream>

using namespace LM;
using namespace std;

GameWindow* GameWindow::m_instance = NULL;
SDL_Surface* GameWindow::m_icon = NULL;
const int GameWindow::MAX_MSAA = 5;
const int GameWindow::FULLSCREEN	= 0x1;
const int GameWindow::VSYNC			= 0x2;
static const pair<int, int> DEFAULT_RESOLUTIONS[] = {
	/* 4:3 */
	pair<int, int>(640, 480),
	pair<int, int>(800, 600),
	pair<int, int>(1024, 768),
	pair<int, int>(1280, 960),
	pair<int, int>(1400, 1050),
	pair<int, int>(1600, 1200),
	/* 16:10 */
	pair<int, int>(1280, 800),
	pair<int, int>(1400, 900),
	pair<int, int>(1680, 1050),
	pair<int, int>(1920, 1200),

	/* Sentinel */
	pair<int, int>(0, 0)
};

GameWindow::GameWindow(int width, int height, int depth, int flags, int msaa) {
	m_width = width;
	m_height = height;
	m_fullscreen = flags & FULLSCREEN;

	for (int i = 0; i <= LAYER_SUPER; ++i) {
		m_layers[i].offset_x = 0;
		m_layers[i].offset_y = 0;
		m_layers[i].visible = true;
	}

	int rflags = SDL_HWSURFACE|SDL_OPENGL;
	if (m_fullscreen) {
		rflags |= SDL_FULLSCREEN;
	}
	depth = SDL_VideoModeOK(width, height, depth, rflags);
	if (depth == 0) {
		throw VmodeNotSupportedException("Video mode not supported.\n"
			"If this problem persists, try deleting the settings file for Leges Motus.",
			width, height, depth, m_fullscreen);
	}

	m_depth = depth;

	
	switch (depth) {
	case 16:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
		break;
	case 24:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		break;
	case 32:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		break;
	default:
		throw Exception("Bad depth");
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (msaa > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);
	}
#if SDL_VERSION_ATLEAST(1, 2, 10)
	// Enable VSYNC
	if (flags & VSYNC) {
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // Deprecated in SDL 1.3
	}
#endif
	SDL_WM_SetCaption("Leges Motus","Leges Motus");
	SDL_ShowCursor(SDL_DISABLE);
	m_context = SDL_SetVideoMode(width, height, depth, rflags);
	if (m_context == NULL) {
		throw Exception(SDL_GetError());
	}
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	if (msaa > 0) {
		glEnable(GL_MULTISAMPLE);
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Set up texture environment for GL_INTERPOLATE, if needed
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_ALPHA, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
	glViewport(0, 0, width, height);
	glOrtho(0, width, height, 0, 1, -1);
}

GameWindow::~GameWindow() {
	m_instance = NULL;
}

bool GameWindow::init_video() {
	if (SDL_WasInit(SDL_INIT_VIDEO)) {
		return true;
	}
	return SDL_InitSubSystem(SDL_INIT_VIDEO) == 0;
}

void GameWindow::deinit_video() {
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	delete m_icon;
	m_icon = NULL;
}

void GameWindow::set_icon(SDL_Surface* icon) {
	if (icon == NULL) {
		return;
	}
	if (!init_video()) {
		throw Exception(SDL_GetError());
	}
	m_icon = icon;
	SDL_WM_SetIcon(icon, NULL);
}

GameWindow* GameWindow::get_instance(int width, int height, int depth, int flags, int msaa) {
	if (m_instance == NULL) {
		if (!init_video()) {
			throw Exception(SDL_GetError());
		}
		try {
			m_instance = new GameWindow(width, height, depth, flags, msaa);
		} catch (const VmodeNotSupportedException& e) {
			if (e.get_fullscreen()) {
				return get_optimal_instance(msaa);
			} else {
				throw e;
			}
		}
	} else {
		m_instance->set_dimensions(width, height);
		m_instance->set_fullscreen(flags & FULLSCREEN);
	}
	return m_instance;
}

GameWindow* GameWindow::get_instance() {
	return m_instance;
}

GameWindow* GameWindow::get_optimal_instance(int flags, int msaa) {
	int depth;
	size_t num_modes;
	if (!init_video()) {
		throw Exception(SDL_GetError());
	}
	supported_resolutions(NULL, NULL, &depth, &num_modes);
	int *w = new int[num_modes];
	int *h = new int[num_modes];
	supported_resolutions(w, h, &depth, &num_modes);
	int max_w = 0;
	int max_h = 0;
	for(size_t i = 0; i < num_modes; ++i) {
		if(w[i]*h[i] > max_w*max_h) {
			max_w = w[i];
			max_h = h[i];
		}
	}
	delete[] w;
	delete[] h;
	return get_instance(max_w, max_h, depth, flags | FULLSCREEN, msaa);
}

void GameWindow::destroy_instance() {
	delete m_instance;
	m_instance = NULL;
}

void GameWindow::supported_resolutions(int* widths, int* heights, int* depth, size_t* num_modes) {
	if (!init_video()) {
		throw Exception(SDL_GetError());
	}
	const SDL_VideoInfo *vidInfo = SDL_GetVideoInfo();
	SDL_Rect **res = NULL;
	if (vidInfo == NULL) {
		*depth = 24; // Hopefully they have a modern display
		res = SDL_ListModes(NULL, SDL_OPENGL|SDL_HWSURFACE|SDL_FULLSCREEN);
	} else {
		*depth = vidInfo->vfmt->BitsPerPixel;
		res = SDL_ListModes(vidInfo->vfmt, SDL_OPENGL|SDL_HWSURFACE|SDL_FULLSCREEN);
	}

	if (res == NULL || ((long)res) == -1L) {
		cerr << "Could not detect resolutions: " << SDL_GetError() << endl;
		size_t real_num = 0;
		for(int i = 0; DEFAULT_RESOLUTIONS[i].first > 0; ++i) {
			int d = SDL_VideoModeOK(DEFAULT_RESOLUTIONS[i].first, DEFAULT_RESOLUTIONS[i].second,
				*depth, SDL_HWSURFACE|SDL_FULLSCREEN);
			if (d != 0) {
				if (widths != NULL && heights != NULL) {
					widths[real_num] = DEFAULT_RESOLUTIONS[i].first;
					heights[real_num] = DEFAULT_RESOLUTIONS[i].second;
				}
				++real_num;
			}
		}
		*num_modes = real_num;
	} else {
		size_t real_num;
		if(widths == NULL || heights == NULL) {
			for(real_num = 0; res[real_num]; ++real_num);
		} else {
			for(real_num = 0; res[real_num] && real_num < *num_modes; ++real_num) {
				widths[real_num] = res[real_num]->w;
				heights[real_num] = res[real_num]->h;
			}
		}
		*num_modes = real_num;
	}
}

int GameWindow::get_width() const {
	return m_width;
}

int GameWindow::get_height() const {
	return m_height;
}

int GameWindow::get_depth() const {
	return m_depth;
}

double GameWindow::get_offset_x(Layer layer) const {
	if (layer < 0 || layer > LAYER_SUPER) {
		throw Exception("Bad window layer");
	}
	return m_layers[layer].offset_x;
}

double GameWindow::get_offset_y(Layer layer) const {
	if (layer < 0 || layer > LAYER_SUPER) {
		throw Exception("Bad window layer");
	}
	return m_layers[layer].offset_y;
}

void GameWindow::set_dimensions(int width, int height) {
	int flags = SDL_HWSURFACE|SDL_OPENGL;
	if (m_fullscreen) {
		flags |= SDL_FULLSCREEN;
	}
	SDL_Surface* context = SDL_SetVideoMode(width, height, m_depth, flags);
	if (context != NULL) {
		m_context = context;
		m_width = width;
		m_height = height;
	}
}

void GameWindow::set_fullscreen(bool fullscreen) {
	int flags = SDL_HWSURFACE|SDL_OPENGL;
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}
	SDL_Surface* context = SDL_SetVideoMode(m_width, m_height, m_depth, flags);
	if(m_context != NULL) {
		m_context = context;
		m_fullscreen = fullscreen;
	}
}

bool GameWindow::is_fullscreen() const {
	return m_fullscreen;
}

void GameWindow::set_offset_x(double offset, Layer layer) {
	if (layer < 0 || layer > LAYER_SUPER) {
		throw Exception("Bad window layer");
	}
	m_layers[layer].offset_x = offset;
}

void GameWindow::set_offset_y(double offset, Layer layer) {
	if (layer < 0 || layer > LAYER_SUPER) {
		throw Exception("Bad window layer");
	}
	m_layers[layer].offset_y = offset;
}

void GameWindow::register_graphic(Graphic* graphic, Layer layer) {
	if (layer < 0 || layer > LAYER_SUPER) {
		throw Exception("Bad window layer");
	}

	for (list<Graphic*>::iterator iter = m_layers[layer].graphics.begin(); iter != m_layers[layer].graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_layers[layer].graphics.insert(iter,graphic);
			return;
		}
	}
	m_layers[layer].graphics.push_back(graphic);
}

void GameWindow::unregister_graphic(Graphic* graphic, Layer layer) {
	if (layer < 0) {
		for (int i = 0; i <= LAYER_SUPER; ++i) {
			m_layers[i].graphics.remove(graphic);
		}
	} else {
		if (layer > LAYER_SUPER) {
			throw Exception("Bad window layer");
		}
		m_layers[layer].graphics.remove(graphic);
	}
}

void GameWindow::reprioritize_graphic(Graphic* graphic, int priority, Layer layer) {
	unregister_graphic(graphic, layer);
	graphic->set_priority(priority);
	register_graphic(graphic, layer);
}

void GameWindow::set_layer_visible(bool visible, Layer layer) {
	if (layer < 0 || layer > LAYER_SUPER) {
		throw Exception("Bad window layer");
	}

	m_layers[layer].visible = visible;
}

void GameWindow::redraw() const {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	for (int i = 0; i <= LAYER_SUPER; ++i) {
		if (!m_layers[i].visible) {
			continue;
		}
		glTranslated(-round(m_layers[i].offset_x), -round(m_layers[i].offset_y),0.0);
		for (list<Graphic*>::const_iterator iter = m_layers[i].graphics.begin(); iter != m_layers[i].graphics.end(); ++iter) {
			if (!(*iter)->is_invisible()) {
				(*iter)->draw();
			}
		}
		glPopMatrix();
	}
	SDL_GL_SwapBuffers();
}

VmodeNotSupportedException::VmodeNotSupportedException(const char* message, int width, int height, int depth, bool fullscreen) : Exception(message) {
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_fullscreen = fullscreen;
}

VmodeNotSupportedException::VmodeNotSupportedException(const string& message, int width, int height, int depth, bool fullscreen) : Exception(message) {
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_fullscreen = fullscreen;
}
