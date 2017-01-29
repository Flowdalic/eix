// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <martin@mvath.de>

#ifndef SRC_CACHE_BASE_H_
#define SRC_CACHE_BASE_H_ 1

#include <ctime>

#include <string>

#include "eixTk/null.h"
#include "eixTk/stringtypes.h"
#include "eixTk/sysutils.h"
#include "eixTk/unused.h"
#include "portage/extendedversion.h"

class Category;
class Package;
class PackageTree;
class PortageSettings;
class Version;

/**
Parent class of every cache that eix can use
**/
class BasicCache {
		friend class AssignReader;
		friend class FlatReader;
		friend class EbuildExec;

	public:
		typedef void (*ErrorCallback)(const std::string& str);

		BasicCache() {
			portagesettings = NULLPTR;
		}

		/**
		Virtual deconstructor
		**/
		virtual ~BasicCache() {
		}

		ATTRIBUTE_CONST_VIRTUAL virtual bool use_prefixport() const {
			return false;
		}

		/**
		Set scheme for this cache
		**/
		virtual void setScheme(const char *prefix, const char *prefixport, const std::string& scheme);

		/**
		Set overlay-key
		**/
		virtual void setKey(ExtendedVersion::Overlay key) {
			m_overlay_key = key;
		}

		/**
		Set overlay-name
		**/
		virtual void setOverlayName(const std::string& name) {
			m_overlay_name = name;
		}

		/**
		Set verbose mode
		**/
		virtual void setVerbose() {
		}

		/**
		Get overlay-key
		**/
		ExtendedVersion::Overlay getKey() const {
			return m_overlay_key;
		}

		/**
		Get overlay-name
		**/
		const std::string& getOverlayName() const {
			return m_overlay_name;
		}

		/**
		Get scheme for this cache
		**/
		std::string getPath() const {
			return m_scheme;
		}

		/**
		Get scheme with path for this cache
		**/
		std::string getPrefixedPath() const;

		/**
		Get scheme with path(s) for this cache
		**/
		std::string getPathHumanReadable() const;

		/**
		Set callback function to be used in case of errors
		**/
		virtual void setErrorCallback(ErrorCallback error_callback) {
			m_error_callback = error_callback;
		}

		/**
		@return name of Cache (formatted for good printing)
		**/
		virtual const char *getType() const = 0;

		/**
		@return true if the method even read multiple categories at once
		**/
		ATTRIBUTE_CONST_VIRTUAL virtual bool can_read_multiple_categories() const {
			return false;
		}

		/**
		If available, the function to read multiple categories.
		@param packagetree should point to packagetree. The other parameters are only used if packagetree is NULLPTR:
		@param cat_name If packagetree is NULLPTR, only packages with this category name are read.
		@param category If packagetree is NULLPTR, the packages matching cat_name are added to this category.
		@return false if some error caused incomplete read
		**/
		virtual bool readCategories(ATTRIBUTE_UNUSED PackageTree *packagetree, ATTRIBUTE_UNUSED const char *cat_name, ATTRIBUTE_UNUSED Category *category) {
			UNUSED(packagetree);
			UNUSED(cat_name);
			UNUSED(category);
			return 1;
		}
		bool readCategories(ATTRIBUTE_UNUSED PackageTree *packagetree) {
			return readCategories(packagetree, NULLPTR, NULLPTR);
		}

		/**
		Prepare reading Cache for an individual category.
		If not overloaded, then readCategories() must be overloaded.
		@return false if a trivial check shows that the category is empty.
		This is used in eix-update to avoid unnecessary updates of the percentage bar.
		In case of a false return value, readCategory() must not be called,
		but readCategoryFinalize() must be called anyway.
		**/
		ATTRIBUTE_NONNULL_ virtual bool readCategoryPrepare(const char *cat_name) {
			m_catname = cat_name;
			return true;
		}

		/**
		Read Cache for an individual category, defined before with readCategoryPrepare().
		If not overloaded, then readCategories() must be overloaded.
		After calling this, readCategoryFinalize() must be called.
		@return false if some error caused incomplete read.
		**/
		ATTRIBUTE_NONNULL_ virtual bool readCategory(Category *cat) {
			return readCategories(NULLPTR, m_catname.c_str(), cat);
		}

		/**
		This must be called to release the data stored with readCategoryPrepare().
		After calling this, readCategory() must not be called without a new readCategoryPrepare().
		**/
		virtual void readCategoryFinalize() {
			m_catname.clear();
		}

		ATTRIBUTE_NONNULL_ virtual bool get_time(ATTRIBUTE_UNUSED time_t *t,ATTRIBUTE_UNUSED const std::string &pkg_name, ATTRIBUTE_UNUSED const std::string &ver_name) const {
			UNUSED(t);
			UNUSED(pkg_name);
			UNUSED(ver_name);
			return 0;
		}

		ATTRIBUTE_NONNULL_ virtual const char *get_md5sum(ATTRIBUTE_UNUSED const std::string &pkg_name, ATTRIBUTE_UNUSED const std::string &ver_name) const {
			UNUSED(pkg_name);
			UNUSED(ver_name);
			return NULLPTR;
		}

		ATTRIBUTE_NONNULL_ virtual void get_version_info(ATTRIBUTE_UNUSED const std::string &pkg_name, ATTRIBUTE_UNUSED const std::string &ver_name, ATTRIBUTE_UNUSED Version *version) const {
			UNUSED(pkg_name);
			UNUSED(ver_name);
			UNUSED(version);
		}

		ATTRIBUTE_NONNULL_ virtual void get_common_info(ATTRIBUTE_UNUSED const std::string &pkg_name, ATTRIBUTE_UNUSED const std::string &ver_name, ATTRIBUTE_UNUSED Package *pkg) const {
			UNUSED(pkg_name);
			UNUSED(ver_name);
			UNUSED(pkg);
		}

	protected:
		virtual void setSchemeFinish() {
		}

		std::string m_scheme, m_prefix, m_catname;
		std::string m_overlay_name;
		bool have_prefix;
		ExtendedVersion::Overlay m_overlay_key;
		ErrorCallback m_error_callback;
		ATTRIBUTE_NONNULL_ void env_add_package(WordMap *env, const Package& package, const Version& version, const std::string& ebuild_dir, const char *ebuild_full) const;

	public:
		PortageSettings *portagesettings;
};

#endif  // SRC_CACHE_BASE_H_
