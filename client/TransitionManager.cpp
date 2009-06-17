/*
 * client/TransitionManager.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "TransitionManager.hpp"

using namespace std;

TransitionManager::TransitionManager() {
	// Nothing to do
}

TransitionManager::~TransitionManager() {
	for (list<State>::iterator iter = m_transitions.begin(); iter != m_transitions.end(); ++iter) {
		if (iter->autodelete) {
			delete iter->transition;
		}
	}
}

void TransitionManager::add_transition(Transition* transition, bool loop, bool autodelete) {
	State state;
	state.transition = transition;
	state.loop = loop;
	state.autodelete = autodelete;
	m_transitions.push_back(state);
}

void TransitionManager::remove_transition(Transition* transition) {
	for (list<State>::iterator iter = m_transitions.begin(); iter != m_transitions.end();) {
		if (iter->transition == transition) {
			if (iter->autodelete) {
				delete iter->transition;
			} 
			iter = m_transitions.erase(iter);
		} else {
			++iter;
		}
	}
}

void TransitionManager::update(uint64_t time) {
	for (list<State>::iterator iter = m_transitions.begin(); iter != m_transitions.end();) {
		if (iter->transition->update(time)) {
			if (iter->loop && iter->transition->get_duration() > 0) {
				do {
					iter->transition->set_start(iter->transition->get_start() + iter->transition->get_duration());
				} while (iter->transition->update(time));
			} else {
				if (iter->autodelete) {
					delete iter->transition;
				}
				iter = m_transitions.erase(iter);
				continue;
			}
		}
		++iter;
	}
}