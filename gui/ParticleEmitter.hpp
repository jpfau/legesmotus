/*
 * gui/ParticleEmitter.hpp
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

#include "Image.hpp"
#include "DrawContext.hpp"
#include "common/Point.hpp"
#include <list>

#ifndef LM_GUI_PARTICLEEMITTER_HPP
#define LM_GUI_PARTICLEEMITTER_HPP

namespace LM {
	class ParticleManager;
	class Particle;

	class ParticleEmitter {
	private:
		Image* m_image;
		DrawContext::BlendMode m_blend_mode;
		int m_alive_count;
		Point m_center;
		ParticleManager* m_manager;
		
		int m_max_expected_particles;
		float* m_vertices;
		float* m_colors;
	protected:
		std::list<Particle*> m_particles;
	public:
		const static float MAX_PARTICLES_AT_A_TIME = 100000;
	
		ParticleEmitter(ParticleManager* manager, Point center, Image* image, DrawContext::BlendMode mode = DrawContext::BLEND_ADD);
		virtual ~ParticleEmitter();
		
		void draw(DrawContext* context);
		
		std::list<Particle*>& get_particles();
		
		void clear();
		
		Particle* request_particle();
		void free_particle(Particle* particle, std::list<Particle*>::iterator it);
		
		virtual bool update(uint64_t timediff);
		
		void init_arrays(int max_expected_particles);
		
		Point get_center();
		
		void set_center(Point center);
		void set_center(float center_x, float center_y);
	};
}

#endif
