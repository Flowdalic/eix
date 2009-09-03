// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <vaeth@mathematik.uni-wuerzburg.de>

#ifndef EIX__EIXRC_H__
#define EIX__EIXRC_H__ 1

#include <portage/keywords.h>
#include <search/redundancy.h>

#include <cstdio>
#include <iostream>

#define EIX_VARS_PREFIX "EIX_"
#define DIFF_VARS_PREFIX "DIFF_"

class EixRcOption {
	public:
		typedef enum { STRING, PREFIXSTRING, INTEGER, BOOLEAN, LOCAL } OptionType;
		OptionType type;
		std::string key, value, local_value, description;

		EixRcOption(OptionType t, std::string name, std::string val, std::string desc);
};

class EixRc : public std::map<std::string,std::string> {
	public:
		std::string varprefix;
		std::string m_eprefixconf;

		typedef std::vector<EixRcOption>::size_type default_index;
		typedef std::pair<RedAtom, RedAtom> RedPair;

		void read();

		void clear();

		void addDefault(EixRcOption option);

		bool getBool(const char *key)
		{ return istrue((*this)[key].c_str()); }

		short getBoolText(const char *key, const char *text)
		{
			const char *s = (*this)[key].c_str();
			if(!strcasecmp(s, text))
				return -1;
			if(istrue(s))
				return 1;
			return 0;
		}

		short getBeforeAfter(const char *key);

		LocalMode getLocalMode(const char *key);

		void getRedundantFlags(const char *key,
			Keywords::Redundant type,
			RedPair &p);

		unsigned int getInteger(const char *key);

		void dumpDefaults(FILE *s, bool use_defaults);

		const char *cstr(const char *var) const;

		const char *prefix_cstr(const char *var) const;

		void print_var(const char *var);
	private:
		static bool istrue(const char *s);
		enum DelayedType { DelayedNotFound, DelayedVariable, DelayedIfTrue, DelayedIfFalse, DelayedIfNonempty, DelayedIfEmpty, DelayedElse, DelayedFi };
		std::vector<EixRcOption> defaults;
		std::set<std::string> prefix_keys;
		static bool getRedundantFlagAtom(const char *s, Keywords::Redundant type, RedAtom &r);
		void modify_value(std::string &value, const std::string &key);

		std::string *resolve_delayed_recurse(std::string key, std::set<std::string> &visited, std::set<std::string> &has_reference, const char **errtext, std::string *errvar);

		  /** Create defaults and the main map with all variables
		     (including all values required by delayed references).
		   @arg has_reference is initialized to corresponding keys */
		void read_undelayed(std::set<std::string> &has_reference);
		void join_delayed(const std::string &val, std::set<std::string> &default_keys, const std::map<std::string,std::string> &tempmap);
		static DelayedType find_next_delayed(const std::string &str, std::string::size_type *pos, std::string::size_type *length = NULL);
		static std::string as_comment(const char *s);
};

#endif /* EIX__EIXRC_H__ */
