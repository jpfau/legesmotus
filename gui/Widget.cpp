/*
 * gui/Widget.cpp
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

#include "Widget.hpp"
#include "pubsub.hpp"

using namespace LM;
using namespace std;

uint64_t Widget::CURR_ID = 0;

Widget::Widget(Widget* parent) {
	m_parent = parent;
	if (parent != NULL) {
		parent->add_child(this);
	}

	m_x = 0;
	m_y = 0;
	m_w = 0;
	m_h = 0;
	m_id = CURR_ID++;
	
	m_receive_input = true;

	m_drawable = true;

	set_color(Color::WHITE, COLOR_PRIMARY);
}

Widget::~Widget() {
	set_parent(NULL);
	clear_children();
}

void Widget::set_parent(Widget* new_parent) {
	if (new_parent == m_parent) {
		return;
	}

	if (m_parent != NULL) {
		m_parent->remove_child(this);
	}

	if (new_parent != NULL) {
		new_parent->add_child(this);
	}

	m_parent = new_parent;
}

Widget* Widget::get_parent() {
	return m_parent;
}

void Widget::on_add_child(Widget* child, int priority) {
}

void Widget::on_remove_child(Widget* child) {
}

void Widget::add_child(Widget* child, int priority) {
	if (child != NULL) {
		if (child->m_parent != NULL) {
			child->m_parent->remove_child(child);
		}
		child->m_parent = this;
		m_children.insert(make_pair(priority, child));
		
		on_add_child(child, priority);
	}
}

void Widget::remove_child(Widget* child) {
	if (child != NULL) {
		child->m_parent = NULL;
		for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end();) {
			if (iter->second == child) {
				m_children.erase(iter++);
				
				on_remove_child(child);
			} else {
				++iter;
			}
		}
	}
}

void Widget::clear_children() {
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		iter->second->m_parent = NULL;
		delete iter->second;
	}
}

Iterator<std::pair<int, Widget*> > Widget::list_children() {
	return Iterator<pair<int, Widget*> >(new StdMultiMapIterator<int, Widget*>(&m_children));
}

Point Widget::get_relative_point(float x, float y) {
	return Point(x - get_x(), y - get_y());
}

Widget* Widget::top_child_at(float x, float y) {
	Point relative_point = get_relative_point(x, y);
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->contains_point(relative_point.x, relative_point.y)) {
			return iter->second;
		}
	}
	return NULL;
}

multimap<int, Widget*> Widget::children_at(float x, float y) {
	Point relative_point = get_relative_point(x, y);
	multimap<int, Widget*> children;
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->contains_point(relative_point.x, relative_point.y)) {
			children.insert(*iter);
		}
	}
	return children;
}

bool Widget::contains_point(float x, float y) const {
	if (x < get_x() - get_width()/2 || x > get_x() + get_width()/2) {
		return false;
	}
	if (y < get_y() - get_height()/2 || y > get_y() + get_height()/2) {
		return false;
	}

	return true;
}

void Widget::change_priority(int old_priority, int new_priority) {
	multimap<int, Widget*> buffer;
	pair<multimap<int, Widget*>::iterator, multimap<int, Widget*>::iterator> range(m_children.equal_range(old_priority));
	for (multimap<int, Widget*>::iterator iter = range.first; iter != range.second; ++iter) {
		buffer.insert(make_pair(new_priority, iter->second));
	}
	m_children.erase(range.first, range.second);
	m_children.insert(buffer.begin(), buffer.end());
}

void Widget::change_priority(Widget* widget, int new_priority) {
	remove_child(widget);
	add_child(widget, new_priority);
}

uint64_t Widget::get_id() const {
	return m_id;
}

void Widget::set_id(uint64_t id) {
	m_id = id;
}

void Widget::set_receives_input(bool receive_input) {
	m_receive_input = receive_input;
}

bool Widget::receives_input() const {
	return m_receive_input;
}

void Widget::set_x(float x) {
	m_x = x;
}

void Widget::set_y(float y) {
	m_y = y;
}

void Widget::set_width(float w) {
	m_w = w;
}

void Widget::set_height(float h) {
	m_h = h;
}

float Widget::get_x() const {
	return m_x;
}

float Widget::get_y() const {
	return m_y;
}

float Widget::get_absolute_x() const {
	if (m_parent != NULL) {
		return m_parent->get_absolute_x() + m_x;
	} else {
		return m_x;
	}
}

float Widget::get_absolute_y() const {
	if (m_parent != NULL) {
		return m_parent->get_absolute_y() + m_y;
	} else {
		return m_x;
	}
}

float Widget::get_width() const {
	return m_w;
}

float Widget::get_height() const {
	return m_h;
}

void Widget::set_color(const Color& c, ColorType type) {
	ASSERT((unsigned long) type < COLOR_MAX);
	m_colors[type] = c;
}

const Color& Widget::get_color(ColorType type) const {
	ASSERT((unsigned long) type < COLOR_MAX);
	return m_colors[type];
}

void Widget::set_drawable(bool drawable) {
	m_drawable = drawable;
}

bool Widget::is_drawable() const {
	return m_drawable;
}

void Widget::focus() {
	//s_focus();
}

void Widget::blur() {
	//s_blur();
}

// TODO better info about whether child handled the event!

void Widget::private_mouse_clicked(bool child_handled, float x, float y, bool down, int button) {
}
void Widget::private_mouse_moved(bool child_handled, float x, float y, float delta_x, float delta_y) {
}
void Widget::private_keypress(const KeyEvent& event) {
}

void Widget::mouse_clicked(float x, float y, bool down, int button) {
	// All children receive up events, but only the one clicked receives the down event
	Point relative_point = get_relative_point(x, y);
	if (down) {
		bool child_handled = false;
		Point relative_point = get_relative_point(x, y);
		for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
			if (iter->second->receives_input() && iter->second->contains_point(relative_point.x, relative_point.y)) {
				iter->second->mouse_clicked(relative_point.x, relative_point.y, down, button);
				child_handled = true;
			}
		}
		
		private_mouse_clicked(child_handled, x, y, down, button);
	} else {
		bool child_handled = false;
		for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
			if (iter->second->receives_input()) {
				iter->second->mouse_clicked(relative_point.x, relative_point.y, down, button);
				child_handled = true;
			}
		}
		
		private_mouse_clicked(child_handled, x, y, down, button);
	}
}

void Widget::mouse_moved(float x, float y, float delta_x, float delta_y) {
	multimap<int, Widget*> children = children_at(x, y);
	
	Point relative_point = get_relative_point(x, y);
	bool child_handled = false;
	
	// For now, have all children receive the event
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->receives_input()) {
			iter->second->mouse_moved(relative_point.x, relative_point.y, delta_x, delta_y);
			child_handled = true;
		}
	}

	private_mouse_moved(child_handled, x, y, delta_x, delta_y);
}

void Widget::keypress(const KeyEvent& event) {
	// TODO: replicate down to children?

	private_keypress(event);
}

void Widget::update(uint64_t timediff) {
	for (multimap<int, Widget*>::iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		iter->second->update(timediff);
	}
}

void Widget::draw(DrawContext* ctx) const {
	draw_internals(ctx);
}

void Widget::draw_internals(DrawContext* ctx) const {
	ctx->translate(get_x(), get_y());
	for (multimap<int, Widget*>::const_iterator iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (iter->second->is_drawable()) {
			iter->second->draw(ctx);
		}
	}
	ctx->translate(-get_x(), -get_y());
}
