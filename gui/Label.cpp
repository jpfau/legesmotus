/*
 * gui/Label.cpp
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

#include "Label.hpp"
#include <cmath>

using namespace LM;
using namespace std;

Label::Label(Font* font) {
	m_font = font;
	set_height(font->get_height());
	m_align = ALIGN_LEFT;
	m_tracking = 0;
}

Label::Label(const wstring& str, Font* font) : m_text(str) {
	m_font = font;
	set_height(font->get_height());
	m_align = ALIGN_LEFT;
	m_tracking = 0;
	recalculate_width();
}

Label::Label(const string& str, Font* font) : m_text(str.length(), L' ') {
	m_font = font;
	set_height(font->get_height());
	copy(str.begin(), str.end(), m_text.begin());
	m_align = ALIGN_LEFT;
	m_tracking = 0;
	recalculate_width();
}

void Label::recalculate_width() {
	float total_advance = 0;
	wchar_t prev_char = -1;

	for (wstring::const_iterator iter = m_text.begin(); iter != m_text.end(); ++iter) {
		const Font::Glyph* glyph = m_font->get_glyph(*iter);
		float kern = m_font->kern(prev_char, *iter);
		total_advance += glyph->advance + m_tracking + kern;
		prev_char = *iter;
	}

	set_width(total_advance);
}

void Label::set_string(const wstring& str) {
	m_text = str;
	recalculate_width();
}

void Label::set_string(const string& str) {
	m_text.clear();
	m_text.reserve(str.size());
	copy(str.begin(), str.end(), m_text.begin());
	recalculate_width();
}

void Label::set_color(Color color) {
	m_color = color;
}

void Label::set_align(Align align) {
	m_align = align;
}

Label::Align Label::get_align() const {
	return m_align;
}

void Label::set_tracking(float tracking) {
	m_tracking = tracking;
	recalculate_width();
}

void Label::redraw(DrawContext* ctx) const {
	float total_advance = 0;
	int align = 0;
	wchar_t prev_char = -1;

	if (get_align() == ALIGN_CENTER) {
		align = get_width() * 0.5f;
	} else if (get_align() == ALIGN_RIGHT) {
		align = get_width();
	}

	ctx->set_draw_color(m_color);
	ctx->translate(get_x() - align, get_y());

	for (wstring::const_iterator iter = m_text.begin(); iter != m_text.end(); ++iter) {
		const Font::Glyph* glyph = m_font->get_glyph(*iter);
		float kern = m_font->kern(prev_char, *iter);
		float advance = glyph->advance + m_tracking + kern;
		int height = - glyph->height + glyph->baseline;
		ctx->translate(glyph->bearing, height);
		if (glyph->bitmap_width > 0 && glyph->bitmap_height > 0) {
			ctx->draw_image(glyph->bitmap_width, glyph->bitmap_height, glyph->image);
		}
		ctx->translate(advance - int(glyph->bearing), -height);
		total_advance += advance;
		prev_char = *iter;
	}

	ctx->translate(align - (total_advance + get_x()), -get_y());
}
