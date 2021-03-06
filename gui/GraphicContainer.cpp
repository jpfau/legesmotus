/*
 * gui/GraphicContainer.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "GraphicContainer.hpp"
#include "ResourceCache.hpp"
#include "Sprite.hpp"
#include <sstream>
#include <GL/gl.h>

using namespace LM;
using namespace std;

GraphicContainer::GraphicContainer(bool self_contained, Widget* parent) : Widget(parent) {
	m_autodelete = self_contained;
	m_tex = NULL;
	m_priv_ctx = NULL;
}

GraphicContainer::~GraphicContainer() {
	m_name_map.clear();
	delete_texture();

	if (m_autodelete) {
		for (multimap<int, Graphic*>::iterator iter = m_priority_map.begin(); iter != m_priority_map.end(); ++iter) {
			delete iter->second;
		}
	}
}

void GraphicContainer::add_graphic(Graphic* graphic, int priority) {
	m_priority_map.insert(make_pair(priority, m_autodelete?graphic->clone():graphic));
}

void GraphicContainer::add_graphic(const std::string& name, Graphic* graphic, int priority) {
	std::multimap<int, Graphic*>::iterator ins = m_priority_map.insert(make_pair(priority, graphic->clone()));
	m_name_map[name] = ins;
}

Graphic* GraphicContainer::get_graphic(const std::string& name) {
	return m_name_map[name]->second;
}

void GraphicContainer::change_priority(const std::string& name, int new_priority) {
	std::multimap<int, Graphic*>::iterator ins = m_name_map[name];
	Graphic* g = ins->second;
	m_priority_map.erase(ins);
	m_priority_map.insert(make_pair(new_priority, g));
}

void GraphicContainer::build_texture(ResourceCache* cache) {
	ASSERT(get_width() > 0);
	ASSERT(get_height() > 0);
	if (m_priv_ctx != NULL) {
		delete_texture();
	}
	m_priv_ctx = cache->get_context()->make_new_context(get_width(), get_height());
	stringstream name;
	name << "container: " << hex << this;
	Image tex = m_priv_ctx->get_image(name.str(), cache);
	m_tex = new Sprite(&tex);
}

void GraphicContainer::delete_texture() {
	delete m_tex;
	m_tex = NULL;
	delete m_priv_ctx;
	m_priv_ctx = NULL;
}

Graphic* GraphicContainer::get_texture() {
	return m_tex;
}

void GraphicContainer::draw(DrawContext* ctx) const {
	DrawContext* dctx = ctx;
	if (m_priv_ctx != NULL) {
		dctx = m_priv_ctx;
		dctx->push_context();
		dctx->clear();
		// TODO fix inverted axis bug
		dctx->scale(1.0f, -1.0f);
		dctx->translate(0, -get_height());

	} else {
		ctx->translate(get_x(), get_y());
	}
	for (multimap<int, Graphic*>::const_iterator iter = m_priority_map.begin(); iter != m_priority_map.end(); ++iter) {
		if (!iter->second->is_invisible()) {
			iter->second->draw(dctx);
		}
	}
	if (m_priv_ctx != NULL) {
		dctx->pop_context();

		ctx->translate(get_x(), get_y());
		m_tex->draw(ctx);
	}
	ctx->translate(-get_x(), -get_y());
}
