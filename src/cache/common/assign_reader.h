// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <martin@mvath.de>

#ifndef SRC_CACHE_COMMON_ASSIGN_READER_H_
#define SRC_CACHE_COMMON_ASSIGN_READER_H_ 1

#include <string>

#include "cache/base.h"

class Package;
class Depend;

const char *assign_get_md5sum(const std::string& filename);
void assign_get_keywords_slot_iuse_restrict(const std::string& filename, std::string *eapi, std::string *keywords, std::string *slotname, std::string *iuse, std::string *required_use, std::string *restr, std::string *props, Depend *dep, BasicCache::ErrorCallback error_callback) ATTRIBUTE_NONNULL_;
void assign_read_file(const char *filename, Package *pkg, BasicCache::ErrorCallback error_callback) ATTRIBUTE_NONNULL_;

#endif  // SRC_CACHE_COMMON_ASSIGN_READER_H_
