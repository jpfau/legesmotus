/*
 * client/TransitionManager.hpp
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

#ifndef LM_CLIENT_TRANSITIONMANAGER_HPP
#define LM_CLIENT_TRANSITIONMANAGER_HPP

#include "Transition.hpp"
#include <list>
#include <map>
#include <string>

class TransitionManager {
public:
	enum RemovePolicy {
		KEEP,
		REMOVE,
		DELETE
	};
private:
	struct State {
		Transition*	transition;
		bool		loop;
		RemovePolicy	removal;
	};
	std::list<State>	m_transitions;
	std::map<std::string, State*> m_statemap;
public:
	TransitionManager();
	~TransitionManager();

	void add_transition(Transition* transition, bool loop = false, RemovePolicy autodelete = KEEP);
	void add_transition(Transition* transition, const std::string& name, bool loop = false, RemovePolicy autodelete = KEEP);
	void remove_transition(Transition* transition);

	Transition* get_transition(const std::string& name);

	void update(uint64_t time);
};

#endif
