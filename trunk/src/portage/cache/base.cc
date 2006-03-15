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

#include "base.h"

#include <portage/package.h>
#include <database/database.h>

using namespace std;

Package *
addPackage(Category &v, const string &cat, const string &pkg)
{
	Package *p = new Package(cat, pkg);
	OOM_ASSERT(p);
	v.push_back(p);
	return p;
}

Package *
findPackage(Category &v, const char *pkg)
{
	Package *ret = NULL;
	for(Category::iterator i = v.begin();
		i != v.end();
		++i)
	{
		if((*i)->name == pkg) {
			ret = *i;
			break;
		}
	}
	return ret;
}

bool
deletePackage(Category &v, const string &pkg)
{
	bool ret = false;
	for(Category::iterator i = v.begin();
		i != v.end();
		++i)
	{
		if((*i)->name == pkg) {
			delete *i;
			v.erase(i);
			ret = true;
			break;
		}
	}
	return false;
}
