/***************************************************************************
 *   eix is a small utility for searching ebuilds in the                   *
 *   Gentoo Linux portage system. It uses indexing to allow quick searches *
 *   in package descriptions with regular expressions.                     *
 *                                                                         *
 *   https://sourceforge.net/projects/eix                                  *
 *                                                                         *
 *   Copyright (c)                                                         *
 *     Wolfgang Frisch <xororand@users.sourceforge.net>                    *
 *     Emil Beinroth <emilbeinroth@gmx.net>                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __ARGSREADER_H__
#define __ARGSREADER_H__

#include <vector>

using namespace std;

/** Maps longopt->shortopt. */
typedef struct Option {
	const char *longopt; /**< longopt of this pair. */
	const int  shortopt; /**< shortopt of this pair. */

	static const char NONE = 0, /**< No type set (will not be removed on fold) */
				 BOOLEAN_T = 1, /**< Boolean. Will be set to true if found. */
				 BOOLEAN_F = 2, /**< Boolean. Will be set to false if found. */
				 BOOLEAN = 3,   /**< Boolean. Will be flipped if found. */
				 INTEGER = 4,   /**< Int. Increase value if found. */
				 STRING = 5;    /**< String. Set if found (warn if already set) */

	const char type; /**< Type of variable. */
	void       *ptr; /**< Pointer to variable of argument. */
};

/** Represents a parameter. */
class Parameter {
	public:
		static const char ARGUMENT = 1, /**< Class represents an argument. */
					 OPTION = 2;        /**< Class represents a option (shortopt or longopt). */

		char type; /**< Type of arguemnt. ARGUMENT or OPTION. */
		int  opt;  /**< If type is OPTION this holds the option-key. */
		char *arg; /**< If type is ARGUMENT this is the string we got. */

		/** Initalize parameters with given values. */
		Parameter(int option, char *argument) {
			arg = NULL;
			opt = -1;
			if(argument) {
				type = Parameter::ARGUMENT;
				arg = argument;
			}
			else {
				type = Parameter::OPTION;
				opt = option;
			}
		}

		int& operator * () {
			return opt;
		}
};

/** Main class for argument parsing. */
class ArgumentReader : public vector<Parameter> {

	public:
		char *name; /**< Name of called program. */

		/** Reads arguments into vector of TParameters. */
		ArgumentReader(int argc, char **argv, struct Option opt_table[]);

	private:
		struct Option *a_table; /**< Table for options. */

		/** Return shortopt for longopt stored in opt.
		 * @return shortopt */
		int lookupLongOpt(const char *long_opt, struct Option *opt_table);

		/** Check if short_opt is a known option.
		 * @return shortopt */
		int lookupShortOpt(const char short_opt, struct Option *opt_table);

		/** Return Option from internal table. */
		Option *lookupTOption(const int opt, struct Option *opt_table);

		/** Fold parameter-vector so that a option with an arguments has its argument set
		 * internal rather than lying around after it in the vector.
		 * Options which are booleans and integers will be removed and their
		 * values increased, flipped, set true or whatever. */
		void foldAndRemove(struct Option *opt_table);
};

#endif /* __ARGSREADER_H__ */
