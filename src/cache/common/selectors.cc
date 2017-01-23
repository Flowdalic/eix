// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <martin@mvath.de>

#include <config.h>

#include <dirent.h>

#include <cstring>

#include <string>

#include "cache/common/selectors.h"
#include "eixTk/dialect.h"
#include "eixTk/likely.h"
#include "eixTk/null.h"
#include "eixTk/regexp.h"
#include "eixTk/utils.h"
#include "eixrc/eixrc.h"
#include "eixrc/global.h"

using std::string;

int package_selector(SCANDIR_ARG3 dent) {
	return (dent->d_name[0] != '.'
			&& strcmp(dent->d_name, "CVS") != 0);
}

int ebuild_selector(SCANDIR_ARG3 dent) {
	return package_selector(dent);
}

string::size_type ebuild_pos(const std::string& str) {
	string::size_type pos(str.length());
	static CONSTEXPR const string::size_type append_size = 7;
	if(pos <= append_size) {
		return string::npos;
	}
	pos -= append_size;
	if(unlikely(str.compare(pos, append_size, ".ebuild") == 0))
		return pos;
	static Regex *r = NULLPTR;
	static bool is_empty = false;
	if(unlikely(is_empty))
		return false;
	if(unlikely(r == NULLPTR)) {
		string m("\\.ebuild-(");
		EixRc eixrc(get_eixrc());
		const string& s(eixrc["EAPI_REGEX"]);
		if(s.empty()) {
			is_empty = true;
			return false;
		}
		m.append(s);
		m.append(")$");
		r = new Regex(m.c_str());
	}
	string::size_type b;
	if(r->match(str.c_str(), &b, NULLPTR)) {
		return b;
	}
	return string::npos;
}
