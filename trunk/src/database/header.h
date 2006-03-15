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

#ifndef __DBHEADER_H__
#define __DBHEADER_H__

#include <vector>
#include <string>

using namespace std;

/** Representation of a database-header.
 * Contains your arch, the version of the db, the number of packages/categories
 * and a table of key->directory mappings. */
class DBHeader {

	private:
		/** The mapping from key->directory. */
		vector<string> overlays;

	public:
		/** Current version of database-format. */
		static const int current = 12;

		int version; /**< Version of the db. */
		unsigned int numcategories; /**< Number of categories. */
		unsigned int numpackages;   /**< Number of packages. */

		/** Get string for key from directory-table. */
		string getOverlay(short key);

		short countOverlays() {
			return overlays.size();
		}

		/** Add overlay to directory-table and return key. */
		short addOverlay(string overlay);

		/** Write the database-header to FILE-stream. */
		bool write(FILE *stream);

		/** Read database-header from FILE-stream. */
		bool read(FILE *stream);

		bool isCurrent() {
			return version == DBHeader::current;
		}
};
#endif /* __DBHEADER_H__ */
