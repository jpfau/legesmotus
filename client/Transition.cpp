/*
 * client/Transition.cpp
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

#include "Transition.hpp"

Transition::Transition(Graphic* transitioned, Property property, Curve* curve, uint64_t start, uint64_t duration) {
	m_transed = transitioned;
	m_prop = property;
	m_curve = curve;
	m_start = start;
	m_duration = duration;
}

bool Transition::update(uint64_t current) {
	bool passed = false;
	uint64_t progress = current - m_start;
	if(progress >= m_duration) {
		progress = m_duration;
		passed = true;
	}
	(m_transed->*m_prop)((*m_curve)(double(progress)/double(m_duration)));
	return passed;
}