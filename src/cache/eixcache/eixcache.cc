// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Martin V�th <vaeth@mathematik.uni-wuerzburg.de>

#include "eixcache.h"

#include <portage/package.h>
#include <portage/version.h>
#include <database/package_reader.h>
#include <database/header.h>
#include <portage/packagetree.h>
#include <eixTk/filenames.h>
#include <eixTk/stringutils.h>
#include <portage/conf/portagesettings.h>

#include <config.h>
#include <string>

using namespace std;



bool EixCache::initialize(const string &name)
{
	vector<string> args = split_string(name, ":", false);
	if(strcasecmp(args[0].c_str(), "eix") == 0)
	{
		m_name = "eix";
		never_add_categories = true;
	}
	else if((strcasecmp(args[0].c_str(), "eix*") == 0) ||
		(strcasecmp(args[0].c_str(), "*eix") == 0))
	{
		m_name = "eix*";
		never_add_categories = false;
	}
	else
		return false;

	m_file = "";
	if(args.size() >= 2) {
		if(! args[1].empty()) {
			m_name += " ";
			m_name += args[1];
			m_file = args[1];
		}
	}

	m_only_overlay = true;
	m_overlay = "";
	m_get_overlay = 0;
	if(args.size() >= 3) {
		if(! args[2].empty()) {
			m_name += " [";
			m_name += args[2];
			m_name += "]";
			if(args[2] == "*") {
				m_only_overlay = false;
			}
			else
				m_overlay = args[2];
		}
	}
	return (args.size() <= 3);
}

bool EixCache::readCategories(PackageTree *packagetree, vector<string> *categories, Category *category) throw(ExBasic)
{
	if(category) {
		packagetree = NULL;
		categories = NULL;
	}
	bool add_categories = (categories != NULL);
	if(never_add_categories)
		add_categories = false;

	string file;
	if(! m_file.empty())
		file = m_prefix + m_file;
	else
		file = m_prefix + EIX_CACHEFILE;
	FILE *fp = fopen(file.c_str(), "rb");
	if(!fp) {
		m_error_callback("Can't read cache file %s: %s",
			file.c_str(), strerror(errno));
		return false;
	}

	DBHeader header;

	io::read_header(fp, header);
	if(!header.isCurrent()) {
		fclose(fp);
		const char *msg = "Cache file %s uses obsolete format %u (current is %u)";
		if(header.version > DBHeader::current)
			msg = "Cache file %s uses newer format %u (current is %u)";
		m_error_callback(msg, file.c_str(),
			uint(header.version), uint(DBHeader::current));
		return false;
	}
	if(m_only_overlay)
	{
		if(!m_overlay.empty())
		{
			const char *portdir = NULL;
			if(portagesettings)
				portdir = (*portagesettings)["PORTDIR"].c_str();
			if(!header.find_overlay(&m_get_overlay, m_overlay.c_str(), portdir, 0, DBHeader::OVTEST_ALLPATH))
			{
				fclose(fp);
				m_error_callback("Cache file %s does not contain overlay %s",
					file.c_str(), m_overlay.c_str());
				return false;
			}
			m_overlay = "";
		}
	}

	if(packagetree)
		packagetree->need_fast_access(categories);

	for(PackageReader reader(fp, header.size); reader.next(); reader.skip())
	{
		reader.read(PackageReader::NAME);
		Package *p = reader.get();
		Category *dest_cat;
		if(add_categories) {
			dest_cat = &((*packagetree)[p->category]);
		}
		else if(category)
		{
			if(category->name() != p->category)
				continue;
			dest_cat = category;
		}
		else
		{
			dest_cat = packagetree->find(p->category);
			if(!dest_cat)
				continue;
		}

		reader.read(PackageReader::VERSIONS);
		p = reader.get();
		bool have_onetime_info = false;
		Package *pkg = dest_cat->findPackage(p->name);
		if(pkg == NULL)
			pkg = dest_cat->addPackage(p->name);
		else
			have_onetime_info = true;
		for(Package::iterator it = p->begin();
			it != p->end(); ++it)
		{
			if(m_only_overlay)
			{
				if(it->overlay_key != m_get_overlay)
					continue;
			}
			Version *version = new Version(it->getFull());
			version->overlay_key = m_overlay_key;
			version->set_full_keywords(it->get_full_keywords());
			version->slotname = it->slotname;
			version->restrictFlags = it->restrictFlags;
			pkg->addVersion(version);
			if(*(pkg->latest()) == *version)
			{
				pkg->homepage = p->homepage;
				pkg->licenses = p->licenses;
				pkg->desc     = p->desc;
				pkg->provide  = p->provide;
				have_onetime_info = true;
			}
		}
		if(have_onetime_info) { // if the package exists:
			// add coll_iuse from the saved data
			pkg->add_coll_iuse(p->coll_iuse);
		}
		else
			dest_cat->deletePackage(p->name);
	}
	fclose(fp);
	if(packagetree)
		packagetree->finish_fast_access();
	if(add_categories)
		packagetree->add_missing_categories(*categories);
	return true;
}