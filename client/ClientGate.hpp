/*
 * client/ClientGate.hpp
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

#ifndef LM_CLIENT_CLIENTGATE_HPP
#define LM_CLIENT_CLIENTGATE_HPP

#include "BaseMapObject.hpp"
#include "MapObjectParams.hpp"
#include "common/Shape.hpp"
#include <memory>
#include <string>

namespace LM {
	class ClientMap;

	class ClientGate : public BaseMapObject {
	private:
		std::string		m_graphic_name;
		MapObjectParams		m_params;
		Graphic*		m_graphic;
		std::auto_ptr<Shape>	m_bounding_shape;

		char			m_team;
		double			m_width;
		double			m_length;
		double			m_extent;

		double			m_progress;

		bool			m_is_engaged;

	public:
		explicit ClientGate(Point position);

		virtual Graphic*	get_graphic () const { return m_graphic; }
		virtual const Shape*	get_bounding_shape () const { return m_bounding_shape.get(); }

		virtual bool	is_jumpable () const { return false; }
		virtual bool	is_shootable () const { return false; }
		virtual bool	is_collidable () const { return false; }
		virtual bool	is_interactive () const { return true; }
		virtual bool	is_engaged () const { return m_is_engaged; }
		virtual bool	shot (GameController& gc, Player& shooter, Point point_hit, double direction) { return false; }
		virtual void	collide (GameController& gc, Player& player, Point old_position, double angle_of_incidence) { }
		virtual void	interact (GameController& gc, Player& player);
		virtual void	disengage (GameController& gc, Player& player);
		virtual void	init (MapReader& reader, ClientMap& map);

		// 0 == fully closed ... 1 == fully open
		void		set_progress(ClientMap& map, double progress);

	};
}

#endif
