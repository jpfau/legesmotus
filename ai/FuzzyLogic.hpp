/*
 * ai/FuzzyLogic.hpp
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

#ifndef LM_AI_FUZZYLOGIC_HPP
#define LM_AI_FUZZYLOGIC_HPP

#include "FuzzyCategory.hpp"
#include "FuzzyEnvironment.hpp"
#include "common/Configuration.hpp"
#include <map>
#include <vector>

namespace LM {
	class FuzzyLogic {
	public:
		class Rule {
		public:
			virtual ~Rule() {}
			virtual float apply(const FuzzyEnvironment& values, long id) const;
			virtual void apply(const FuzzyEnvironment& values, std::map<long, float>* output) const;
		};

		class Terminal : public Rule {
		private:
			int m_bin;
			int m_cat;

		public:
			Terminal(int cat, int bin);
			virtual float apply(const FuzzyEnvironment& values, long id) const;
		};

		class And : public Rule {
		private:
			const Rule* m_lhs;
			const Rule* m_rhs;

		public:
			And(const Rule* lhs, const Rule* rhs);
			virtual ~And();
			virtual float apply(const FuzzyEnvironment& values, long id) const;
		};

		class Or : public Rule {
		private:
			const Rule* m_lhs;
			const Rule* m_rhs;

		public:
			Or(const Rule* lhs, const Rule* rhs);
			virtual ~Or();
			virtual float apply(const FuzzyEnvironment& values, long id) const;
		};

		class Not : public Rule {
		private:
			const Rule* m_op;

		public:
			Not(const Rule* op);
			virtual ~Not();
			virtual float apply(const FuzzyEnvironment& values, long id) const;
		};

	private:
		std::vector<FuzzyCategory> m_cats;
		std::map<std::string, int> m_cat_ids;

		std::vector<Rule*> m_rules;
		std::map<std::string, int> m_rule_ids;

		std::string m_section;

	public:
		FuzzyLogic(const std::string& section);
		~FuzzyLogic();

		int add_category(const std::string& name);
		FuzzyCategory* get_category(int cat);
		const FuzzyCategory* get_category(int cat) const;
		int get_category_id(const std::string& name) const;

		bool load_category(const Configuration* config, const std::string& category);

		int add_rule(const std::string& name, Rule* rule);
		int get_rule_id(const std::string& name) const;

		void apply(FuzzyEnvironment* env) const;
		float decide(int rule, const FuzzyEnvironment& env) const;
		float decide(int rule, long id, const FuzzyEnvironment& env) const;
		float decide(int rule, void* id, const FuzzyEnvironment& env) const;

		Terminal* make_terminal(const std::string& cat, const std::string& bin) const;
	};
}

#endif
