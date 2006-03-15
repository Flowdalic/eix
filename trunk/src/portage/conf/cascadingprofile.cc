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

#include "cascadingprofile.h"

#include <eixTk/utils.h>
#include <varsreader.h>

#include <fstream>

#include <unistd.h>

/* Path to symlink to profile */
#define PROFILE_LINK "/etc/make.profile"
#define PROFILE_LINK_DIRECTORY "/etc/"

/* Buffer size for the readlink-call */
#define READLINK_BUFFER 128

/** Exclude this files from listing of files in profile. */
static const char *profile_exclude[] = { "parent", "..", "." , NULL };

/** Look for parent profile of the profile pointed to by path_buffer. Write
 * the path for the new profile into path_buffer and return true; Return false
 * if there is no parent profile. */
bool CascadingProfile::getParentProfile(string &path_buffer)
{
	string buf;

	/* Open stream and check if it's open */
	ifstream ifstr((path_buffer + "parent").c_str());
	if(! ifstr.is_open())
		return false;

	/* while there are lines in the file */
	while(getline(ifstr, buf))
	{
		trim(&buf);
		/* If it's a comment or a empty line continue with the next line */
		if(buf.size() == 0 || buf[0] == '#')
			continue;

		path_buffer.append(buf);
		path_buffer.append("/");
		return true;
	}
	return false;
}

void
CascadingProfile::readFiles()
{
	for(vector<string>::iterator file = m_profile_files.begin();
		file != m_profile_files.end();
		++file)
	{
		void (CascadingProfile::*handler)(const string &line) = NULL;

		if(strcmp(strrchr(file->c_str(), '/'), "/packages") == 0)
		{
			handler = &CascadingProfile::readPackages;
		}
		else if(strcmp(strrchr(file->c_str(), '/'), "/package.mask") == 0)
		{
			handler = &CascadingProfile::readPackageMasks;
		}

		if(handler != NULL)
		{
			vector<string> lines;
			pushback_lines(file->c_str(), &lines, false);

			for(unsigned int i = 0; i<lines.size(); i++)
			{
				if(lines[i].size() == 0)
					continue;
				try {
					(this->*handler) (lines[i]);
				}
				catch(ExBasic e)
				{
					portage_parse_error(*file, i + 1, lines[i], e);
				}
			}
		}
	}
}

void CascadingProfile::readPackages(const string &line)
{
	/* Cycle through and get rid of comments ..
	 * lines beginning with '*' are m_system-packages
	 * all others are masked by profile .. if they don't match :) */
	const char *p = line.c_str();
	bool remove = (*p == '-');

	if (remove)
	{
		++p;
	}

	Mask     *m = NULL;
	MaskList *ml = NULL;
	switch(*p)
	{
		case '*': 
			++p;
			m = new Mask(p, Mask::maskInSystem) ;
			ml = &m_system;
			break;
		default:
			m = new Mask(p, Mask::maskAllowedByProfile);
			ml = &m_system_allowed;
			break;
	}

	if (remove)
	{
		ml->remove(m);
		delete m;
	}
	else 
	{
		ml->add(m);
	}
}

/** Key that should accumelate their content rathern then replace. */
static const char *default_accumulating_keys[] = {
	"USE",
	"CONFIG_*",
	"FEATURES",
	"ACCEPT_KEYWORDS",
	NULL
};

/** Read all "make.defaults" files found in profile. */
void CascadingProfile::readMakeDefaults()
{
	for(unsigned int i = 0; i<m_profile_files.size(); i++) {
		if( strcmp(strrchr(m_profile_files[i].c_str(), '/'), "/make.defaults") == 0) {
			VarsReader parser(VarsReader::SUBST_VARS|VarsReader::INTO_MAP|VarsReader::APPEND_VALUES);
			parser.accumulatingKeys(default_accumulating_keys); // use defaults
			parser.useMap(m_make_defaults);
			parser.read(m_profile_files[i].c_str());
		}
	}
}

void CascadingProfile::readPackageMasks(const string &line)
{
	if(line[0] == '-')
	{
		Mask *m = new Mask(line.substr(1).c_str(), Mask::maskMask);
		m_package_masks.remove(m);
		delete m;
	}
	else
	{
		m_package_masks.add(new Mask(line.c_str(), Mask::maskMask));
	}
}

/** Cycle through profile and put path to files into this->m_profile_files. */
void CascadingProfile::listProfile(void) throw(ExBasic)
{
	char readlink_buffer[READLINK_BUFFER];
	int len = readlink(PROFILE_LINK, readlink_buffer, READLINK_BUFFER - 1);
	if(len == -1) {
		throw( ExBasic("readlink("PROFILE_LINK") failed: %s", strerror(errno)) );
	}
	readlink_buffer[len] = '\0';

	string path_buffer(readlink_buffer);
	/* If it's a relative path prepend the dirname of PROFILE_LINK_DIRECTORY */
	if( path_buffer[0] != '/' )
		path_buffer.insert(0, PROFILE_LINK_DIRECTORY);
	path_buffer.append("/"); /* append "/" */

	do {
		/* Don't care about errors when reading profile. */
		(void) pushback_files(path_buffer, m_profile_files, profile_exclude);
	} while( getParentProfile(path_buffer) );

}
