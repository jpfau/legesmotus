/*
 * gui/Bindings.cpp
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

#include "Bindings.hpp"

using namespace LM;
using namespace std;


Bindings::ControlEvent Bindings::process_event(const KeyEvent& event) {
	// TODO unhardcode
	ControlEvent e;
	e.type = CONTROL_NONE;
	switch (event.type) {
	case KEY_LETTER:
		switch (event.character) {
		case L'Y':
		case L'y':
		case L'/':
		case L'?':
			e.type = CONTROL_BEGIN_TYPING;
			break;

		case L' ':
			e.type = CONTROL_JUMP;
			break;
		}
		break;
	default:
		break;
	}
	return e;
}

Bindings::ControlEvent Bindings::process_event(const MouseMotionEvent& event) {
	ControlEvent e;
	e.type = CONTROL_NONE;
	return e;
}

Bindings::ControlEvent Bindings::process_event(const MouseButtonEvent& event) {
	ControlEvent e;
	e.type = CONTROL_NONE;

	switch (event.button) {
	case BUTTON_LEFT:
		e.type = event.down?CONTROL_FIRE:CONTROL_STOP_FIRE;
		break;
	}
	return e;
}
