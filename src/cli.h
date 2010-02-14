// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <vaeth@mathematik.uni-wuerzburg.de>

#ifndef EIX__CLI_H__
#define EIX__CLI_H__ 1

#include <eixTk/argsreader.h>

class DBHeader;
class EixRc;
class MarkedList;
class MatchTree;
class PortageSettings;
class SetStability;
class VarDbPkg;
void parse_cli(MatchTree *matchtree, EixRc &eixrc, VarDbPkg &varpkg_db, PortageSettings &portagesettings, const SetStability &stability, const DBHeader &header, MarkedList **marked_list, ArgumentReader::iterator arg, ArgumentReader::iterator end);

/*	If you want to add a new parameter to eix just insert a line into
 *	long_options. If you only want a longopt, add a new define.
 *
 *	-- ebeinroth
 */

enum cli_options {
	O_FMT = 256,
	O_FMT_VERBOSE,
	O_FMT_COMPACT,
	O_XML,
	O_PRINT_VAR,
	O_PRINT_OPATH,
	O_PRINT_OLABEL,
	O_DUMP,
	O_DUMP_DEFAULTS,
	O_CARE,
	O_IGNORE_ETC_PORTAGE,
	O_ONLY_NAMES,
	O_BRIEF2,
	O_CURRENT,
	O_HASH_IUSE,
	O_HASH_KEYWORDS,
	O_HASH_SLOT,
	O_HASH_PROVIDE,
	O_HASH_LICENSE,
	O_WORLD_SETS,
	O_STABLE_DEFAULT,
	O_TESTING_DEFAULT,
	O_NONMASKED_DEFAULT,
	O_BINARY,
	O_WORLD_FILE,
	O_WORLD_SET,
	O_WORLD_ALL,
	O_SELECTED_FILE,
	O_SELECTED_SET,
	O_SELECTED_ALL,
	O_SYSTEM_DEFAULT,
	O_STABLE_LOCAL,
	O_TESTING_LOCAL,
	O_NONMASKED_LOCAL,
	O_SYSTEM_LOCAL,
	O_STABLE_NONLOCAL,
	O_TESTING_NONLOCAL,
	O_NONMASKED_NONLOCAL,
	O_SYSTEM_NONLOCAL,
	O_UPGRADE_LOCAL,
	O_UPGRADE_NONLOCAL,
	O_INSTALLED_UNSTABLE,
	O_INSTALLED_TESTING,
	O_INSTALLED_MASKED,
	O_OVERLAY,
	O_ONLY_OVERLAY,
	O_INSTALLED_OVERLAY,
	O_INSTALLED_SOME,
	O_INSTALLED_WITH_USE,
	O_INSTALLED_WITHOUT_USE,
	O_INSTALLED_SLOT,
	O_FROM_OVERLAY,
	O_EIX_CACHEFILE,
	O_DEBUG,
	O_SEARCH_SLOT,
	O_SEARCH_SET,
	O_END_ALGO,
	O_RESTRICT_FETCH,
	O_RESTRICT_MIRROR,
	O_RESTRICT_PRIMARYURI,
	O_RESTRICT_BINCHECKS,
	O_RESTRICT_STRIP,
	O_RESTRICT_TEST,
	O_RESTRICT_USERPRIV,
	O_RESTRICT_INSTALLSOURCES,
	O_RESTRICT_BINDIST,
	O_PROPERTIES_INTERACTIVE,
	O_PROPERTIES_LIVE,
	O_PROPERTIES_VIRTUAL,
	O_PROPERTIES_SET
};


#endif /* EIX__CLI_H__ */
