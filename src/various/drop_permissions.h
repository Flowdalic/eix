// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Martin Väth <martin@mvath.de>

#ifndef SRC_VARIOUS_DROP_PERMISSIONS_H_
#define SRC_VARIOUS_DROP_PERMISSIONS_H_ 1

#include <string>

class EixRc;

bool drop_permissions(EixRc *eix, std::string *errtext) ATTRIBUTE_NONNULL_;

#endif  // SRC_VARIOUS_DROP_PERMISSIONS_H_
