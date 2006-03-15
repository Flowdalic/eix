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

#ifndef __BASICCACHE_H__
#define __BASICCACHE_H__

#include <string>

class Category;
class Package;

// Add package to vector
Package *addPackage(Category &v, const std::string &cat, const std::string &pkg);

// Find Package and return pointer to it.
Package *findPackage(Category &v, const char *pkg);

// Remove and delete Package. */
bool deletePackage(Category &v, const std::string &pkg);

// Parent class of every cache that eix can use. */
class BasicCache {

	public:
		// Virtual deconstructor. */
		virtual ~BasicCache()
		{ }

		// Set scheme for this cache. */
		void setScheme(std::string scheme) 
		{ m_scheme = scheme; }

		// Set overlay-key. */
		void setKey(short key)
		{ m_overlay_key = key; }

		// Set arch for system. */
		void setArch(const std::string &arch) 
		{ m_arch = arch; }

		// Set arch for system. */
		void setErrorCallback(void (*error_callback)(const char *fmt, ...))
		{ m_error_callback = error_callback; }

		// Get scheme for this cache. */
		std::string getPath() const 
		{ return m_scheme; }

		// Return name of Cache.*/
		virtual const char *getType() const = 0;

		// Read Cache for a category with a little from portageif. */
		virtual int readCategory(Category &vec, const std::string &cat) = 0;

	protected:
		std::string m_scheme;
		short  m_overlay_key;
		std::string m_arch;
		void (*m_error_callback)(const char *fmt, ...);

	public:
};

#endif /* __BASICCACHE_H__ */
