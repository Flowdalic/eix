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

/* we use strndup */
#if !defined _GNU_SOURCE
#define _GNU_SOURCE
#endif /* !defined _GNU_SOURCE */

#include "stringutils.h"

// OOM_ASSERT 
#include <eixTk/exceptions.h>

#include <eixTk/regexp.h> 

#if !defined HAVE_STRNDUP
/* If we don't have strndup, we use our own ..
 * darwin (macos) doesn't have strndup, it's a GNU extension
 * See http://bugs.gentoo.org/show_bug.cgi?id=111912 */

#include <stdlib.h>

char *
strndup(const char *s, size_t n)
{
	char       *r = NULL;
	const char *p = s;
	while(*p++ && n--);
	n = p - s - 1;
	r = (char *) malloc(n + 1);
	if(r != NULL)
	{
		memcpy(r, s, n);
		r[n] = 0;
	}
	return r;
}
#endif /* !defined HAVE_STRNDUP */

const char *
ExplodeAtom::get_start_of_version(const char* str)
{
	const char *x = NULL;
	while(*str)
	{
		if(*str++ == '-'
		   && (*str >= 48
			   && *str <= 57)) 
		{
			x = str;
		}
	}

	return x;
}

char *
ExplodeAtom::split_version(const char* str)
{
	const char *x = get_start_of_version(str);

	if(x)
	{
		return strdup(x);
	}

	return NULL;
}

char *
ExplodeAtom::split_name(const char* str)
{
	const char *x = get_start_of_version(str);

	if(x)
	{
		return strndup(str, ((x - 1) - str));
	}

	return NULL;
}

char **
ExplodeAtom::split(const char* str)
{
	static char* out[2] = { NULL, NULL };
	const char *x = get_start_of_version(str);

	if(x)
	{
		out[0] = strndup(str, ((x - 1) - str));
		out[1] = strdup(x);
	}
	return out;
}

vector<string>
split_string(const string &str, const char *at, bool ignore_empty)
{
	vector<string> vec;
	string::size_type last_pos = 0,
		pos = 0;
	while((pos = str.find_first_of(at, pos)) != string::npos) {
		if((pos - last_pos) > 0 || !ignore_empty)
			vec.push_back(str.substr(last_pos, pos - last_pos));
		last_pos = ++pos;
	}
	if((str.size() - last_pos) > 0 || !ignore_empty)
		vec.push_back(str.substr(last_pos));
	return vec;
}

string
join_vector(vector<string> &vec, string glue)
{
	vector<string>::iterator it = vec.begin();
	if(it == vec.end()) {
		return "";
	}
	string ret;
	for(;;) {
		ret.append(*it);
		if(++it == vec.end()) {
			break;
		}
		ret.append(glue);
	}
	return ret;
}

inline 
bool 
vec_clean(vector<string>::iterator begin, vector<string>::iterator end, string s)
{
	bool found = false;
	vector<string>::iterator it = find(begin, end, s);
	while(it != end) {
		*it = "";
		it = find(begin, end, s);
		found = true;
	}
	return found;
}


string
resolve_plus_minus(string &v, bool warn_plus, bool order)
{
	vector<string> splitted = split_string(v); 
	return join_vector(resolve_plus_minus(splitted, warn_plus, order));
}

vector<string>& 
resolve_plus_minus(vector<string> &v, bool warn_plus, bool order)
{
	for(vector<string>::iterator it = v.begin(); it != v.end(); ++it) {
		if(it->size() == 0) {
			continue;
		}
		if((*it)[0] == '+') {
			warn_plus && cerr << "keyword begins with '+': " << *it << endl;
			it->erase(0, 1);
		}
		if((*it)[0] == '-') {
			if(*it == "-*") {
				v.erase(v.begin(), ++it);
				return resolve_plus_minus(v, warn_plus, order);
			}
			else if(vec_clean(v.begin(), order ? it : v.end(), it->substr(1))) {
				*it = "";
			}
		}
	}
	vector<string>::iterator it = v.begin();
	while(it != v.end()) {
		if(it->size() == 0) {
			v.erase(it);
		}
		else {
			++it;
		}
	}
	return v;
}
