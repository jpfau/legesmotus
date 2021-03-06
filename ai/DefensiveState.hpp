/*
 * ai/DefensiveState.hpp
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

#ifndef LM_AI_DEFENSIVESTATE_HPP
#define LM_AI_DEFENSIVESTATE_HPP


#include "FuzzyLogicState.hpp"
#include "common/misc.hpp"
#include "FuzzyLogic.hpp"

namespace LM {
	class FuzzyLogicFSM;

	class DefensiveState : public FuzzyLogicState {
	private:
		std::string m_name;
		std::string m_next_state;
		const FuzzyLogic* m_fuzzy;
		
		Player* m_target;
		int m_wanted_weapon;
		float m_desired_aim;
		AI::AimReason m_aim_reason;
		
		// Fuzzy Logic Rules
		FuzzyLogic::Rule* m_rule_dangerous;
		FuzzyLogic::Rule* m_rule_can_target;
		FuzzyLogic::Rule* m_rule_firing_importance;
		FuzzyLogic::Rule* m_rule_run_away;
		FuzzyLogic::Rule* m_rule_jump_own_gate;
		FuzzyLogic::Rule* m_rule_dont_jump;
		FuzzyLogic::Rule* m_rule_weapon_fitness;
		FuzzyLogic::Rule* m_rule_holding_gate;
		
		void load_rules();
		bool check_switch_weapons(FuzzyLogicAI* ai, const GameLogic& logic, FuzzyEnvironment* env);
		void update_wanted_aim(FuzzyLogicAI* ai, const GameLogic& logic, FuzzyEnvironment* env);
		void check_transitions(FuzzyLogicAI* ai, const GameLogic& logic, FuzzyEnvironment* env);

	public:
		DefensiveState(const FuzzyLogic* fuzzy_logic);
		virtual ~DefensiveState();
	
		virtual const std::string& get_name() const;
		virtual const FuzzyLogicState* next_state(const FuzzyLogicFSM* fsm);
		
		virtual float find_desired_aim() const;
		virtual AI::AimReason get_aim_reason() const;
		virtual int get_curr_weapon() const;

		virtual void decide(FuzzyLogicAI* ai, FuzzyEnvironment* env, const GameLogic& logic);
	};
}

#endif
