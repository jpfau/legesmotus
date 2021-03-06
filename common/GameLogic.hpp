/*
 * common/GameLogic.hpp
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

#ifndef LM_COMMON_GAMELOGIC_HPP
#define LM_COMMON_GAMELOGIC_HPP

#include "common/Player.hpp"
#include "common/physics.hpp"
#include <map>
#include <vector>
#include <utility>
#include "common/MapObject.hpp"
#include "common/Iterator.hpp"

class b2World;

namespace LM {
	class Map;
	class Weapon;

	class GameLogic : public b2ContactListener {
	
	const static float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const static int VEL_ITERATIONS = 10;
	const static int POS_ITERATIONS = 10;
	const static float JUMP_ROTATION = 15.0f;
	
	private:
		std::map<uint32_t, Player*> m_players;
		Map* m_map;
		b2World* m_physics;
		std::vector<Weapon*> m_weapons;
		std::map<std::string, std::string> m_params;
		
		int m_energy_recharge;
		uint64_t m_energy_recharge_rate;
		uint64_t m_energy_recharge_delay;
		bool m_recharge_continuously;
		float m_jump_velocity;
		
		bool m_round_in_progress;
		uint64_t m_round_start_time;
		
		std::vector< std::pair<b2Body*, b2JointDef*> > m_joints_to_create;
		
		float get_dist(b2Vec2 point1, b2Vec2 point2);

	public:
		GameLogic(Map* map);
		~GameLogic();

		void add_player(Player* player);
		// XXX why does this have a return value?
		Player* remove_player(uint32_t id);
		Player* get_player(const uint32_t id);
		const Player* get_player(const uint32_t id) const;
		Iterator<std::pair<uint32_t, Player*> > list_players();
		ConstIterator<std::pair<uint32_t, Player*> > list_players() const;
		int num_players() const;
		
		void add_weapon(size_t index, Weapon* weapon);
		void clear_weapons();
		Weapon* get_weapon(const uint32_t id);
		const Weapon* get_weapon(const uint32_t id) const;
		Iterator<Weapon*> list_weapons();
		ConstIterator<Weapon*> list_weapons() const;
		int num_weapons() const;
		
		const std::string& get_param(const std::string& name) const;
		const std::map<std::string, std::string>& get_params() const { return m_params; }
		
		void update_map();
		Map* get_map();
		const Map* get_map() const;
		Map* unregister_map();
		
		void round_started();
		void round_ended();
		
		bool round_in_progress() const;
		uint64_t get_round_start_time() const; // Value is only valid if round_in_progress() is true.
		
		// Run the next step of the game logic.
		void step();

		// Run the number of steps for a period of time, and return the number of unused ticks
		uint64_t steps(uint64_t ticks);

		b2World* get_world();
		const b2World* get_world() const;
		
		// Attempt to jump off an obstacle
		virtual bool attempt_jump(uint32_t player_id, float angle);
		
		// Attempt to fire a weapon
		virtual bool attempt_fire(uint32_t player_id, uint32_t weapon_id, float angle, Packet::WeaponDischarged* packet);
		
		// Update the states of the gates
		virtual void update_gate_progress(char team, float progress);
		float get_gate_progress(char team) const;
		
		// Check if a player is engaging a gate
		virtual bool is_engaging_gate(uint32_t player_id, char team) const;
		
		// Set game parameters
		virtual void set_param(const std::string& param_name, const std::string& param_value);
		
		// Physics helper methods
		virtual void create_contact_joint(b2Body* body1, b2JointDef* joint_def);
		virtual MapObject::CollisionResult collide(PhysicsObject* userdata1, PhysicsObject* userdata2, b2Contact* contact, bool isnew, bool disengage);
		virtual void create_grab(Player* player, b2Body* body2, b2WorldManifold* manifold, bool weld);
		
		// Physics contact listener methods.
		virtual void BeginContact(b2Contact* contact);
		virtual void EndContact(b2Contact* contact);
		virtual void PreSolve(b2Contact* contact, const b2Manifold* old_manifold);
		virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	};
}

#endif
