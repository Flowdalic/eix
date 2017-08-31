// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Martin Väth <martin@mvath.de>

#ifndef SRC_DATABASE_IO_H_
#define SRC_DATABASE_IO_H_ 1

#include <config.h>

#include <cstdio>

#include <string>

#include "database/header.h"
#include "eixTk/attribute.h"
#include "eixTk/diagnostics.h"
#include "eixTk/dialect.h"
#include "eixTk/eixint.h"
#include "eixTk/likely.h"
#include "eixTk/null.h"
#include "eixTk/stringtypes.h"
#include "eixTk/stringutils.h"

// check_includes: include "portage/basicversion.h"

class BasicPart;
class IUseSet;
class Package;
class PackageReader;
class PackageTree;
class PortageSettings;
class Depend;
class Version;

#define MAGICNUMCHAR 0xFFU

class File {
	private:
		FILE *fp;
		bool seek(eix::OffsetType offset, int whence, std::string *errtext);

		File(const File& s) ASSIGN_DELETE;
		File& operator=(const File& s) ASSIGN_DELETE;

	public:
		File() : fp(NULLPTR) {
		}

		~File() {
			destroy();
		}

#ifdef HAVE_MOVE
		File(File&& s) NOEXCEPT : fp(s.fp) {
			s.fp = NULLPTR;
		}

		File& operator=(File&& s) NOEXCEPT {
			destroy();
			fp = s.fp;
			s.fp = NULLPTR;
			return *this;
		}
#endif
		void destroy();

		ATTRIBUTE_NONNULL_ bool openread(const char *name);
		ATTRIBUTE_NONNULL_ bool openwrite(const char *name);

		int getch() {
			return std::fgetc(fp);
		}

		bool putch(eix::UChar c) {
			return (std::fputc(c, fp) != EOF);
		}

		bool read(char *s, std::string::size_type len) {
			return (std::fread(s, sizeof(*s), len, fp) == len);
		}

		bool write(const std::string str) {
			return (std::fwrite(static_cast<const void *>(str.c_str()), sizeof(*(str.c_str())), str.size(), fp) == str.size());
		}

		ATTRIBUTE_NONNULL((2)) bool read_string_plain(char *s, std::string::size_type len, std::string *errtext);
		bool write_string_plain(const std::string& str, std::string *errtext);

		bool seekrel(eix::OffsetType offset, std::string *errtext) {
			return seek(offset, SEEK_CUR, errtext);
		}

		bool seekabs(eix::OffsetType offset, std::string *errtext) {
			return seek(offset, SEEK_SET, errtext);
		}

		eix::OffsetType tell();

		void readError(std::string *errtext);
		static void writeError(std::string *errtext);
};

class Database : public File {
		friend class PackageReader;

	private:
		bool counting;
		eix::OffsetType counter;

		ATTRIBUTE_NONNULL((2)) bool read_Part(BasicPart *b, std::string *errtext);
		bool write_Part(const BasicPart& n, std::string *errtext);
		bool write_string_plain(const std::string& str, std::string *errtext);

	protected:
		bool readUChar(eix::UChar *c, std::string *errtext);
		bool writeUChar(eix::UChar c, std::string *errtext);

		/**
		Read a nonnegative number (m_Tp must be big enough)
		**/
		template<typename m_Tp> ATTRIBUTE_NONNULL((2)) bool read_num(m_Tp *ret, std::string *errtext);

		/**
		Write nonnegative number t to fp (undefined behaviour if t < 0)
		**/
		template<typename m_Tp> bool write_num(m_Tp t, std::string *errtext);

		ATTRIBUTE_NONNULL((2)) bool read_string(std::string *s, std::string *errtext);
		bool write_string(const std::string& str, std::string *errtext);

		bool write_hash_string(const StringHash& hash, const std::string& s, std::string *errtext) {
			return write_num(hash.get_index(s), errtext);
		}

		ATTRIBUTE_NONNULL((3)) bool read_hash_string(const StringHash& hash, std::string *s, std::string *errtext) {
			StringHash::size_type i;
			if(likely(read_num(&i, errtext))) {
				*s = hash[i];
				return true;
			}
			return false;
		}

		bool write_hash_words(const StringHash& hash, const WordVec& words, std::string *errtext);

		bool write_hash_words(const StringHash& hash, const std::string& words, std::string *errtext) {
			WordVec word_vec;
			split_string(&word_vec, words);
			return write_hash_words(hash, word_vec, errtext);
		}

		ATTRIBUTE_NONNULL((3)) bool read_hash_words(const StringHash& hash, WordVec *s, std::string *errtext);
		ATTRIBUTE_NONNULL((3)) bool read_hash_words(const StringHash& hash, std::string *s, std::string *errtext);
		bool read_hash_words(std::string *errtext);

		ATTRIBUTE_NONNULL((3)) bool read_iuse(const StringHash& hash, IUseSet *iuse, std::string *errtext);

		ATTRIBUTE_NONNULL((2)) bool read_version(Version *v, const DBHeader& hdr, std::string *errtext);
		ATTRIBUTE_NONNULL((2)) bool write_version(const Version *v, const DBHeader& hdr, std::string *errtext);

		ATTRIBUTE_NONNULL((2)) bool read_depend(Depend *dep, const DBHeader& hdr, std::string *errtext);
		bool write_depend(const Depend& dep, const DBHeader& hdr, std::string *errtext);

		ATTRIBUTE_NONNULL((2, 3)) bool read_category_header(std::string *name, eix::Treesize *h, std::string *errtext);
		bool write_category_header(const std::string& name, eix::Treesize size, std::string *errtext);

		bool write_package(const Package& pkg, const DBHeader& hdr, std::string *errtext);
		bool write_package_pure(const Package& pkg, const DBHeader& hdr, std::string *errtext);

		bool write_hash(const StringHash& hash, std::string *errtext);
		ATTRIBUTE_NONNULL((2)) bool read_hash(StringHash *hash, std::string *errtext);

	public:
		Database() : counting(false), counter(0) {
		}

		ATTRIBUTE_NONNULL_ static void prep_header_hashs(DBHeader *hdr, const PackageTree& tree);

		bool write_header(const DBHeader& hdr, std::string *errtext);
		ATTRIBUTE_NONNULL((2)) bool read_header(DBHeader *hdr, std::string *errtext, DBHeader::DBVersion minver);

		bool write_packagetree(const PackageTree& pkg, const DBHeader& hdr, std::string *errtext);
#if 0
		ATTRIBUTE_NONNULL((2, 4)) bool read_packagetree(PackageTree *tree, const DBHeader& hdr, PortageSettings *ps, std::string *errtext);
#endif
};

template<typename m_Tp> bool Database::read_num(m_Tp *ret, std::string *errtext) {
	int ch(getch());
	if(likely(ch != EOF)) {
		eix::UChar c = static_cast<eix::UChar>(ch);
		// The one-byte case is exceptional w.r.t. to leading 0:
		if(c != MAGICNUMCHAR) {
			*ret = m_Tp(c);
			return true;
		}
		unsigned int toget(1);
		while(likely((ch = getch()) != EOF)) {
			if((c = static_cast<eix::UChar>(ch)) == MAGICNUMCHAR) {
				++toget;
				continue;
			}
			if(c != 0) {
				*ret = static_cast<m_Tp>(c);
			} else {  // leading 0 after MAGICNUMCHAR:
				*ret = static_cast<m_Tp>(MAGICNUMCHAR);
				--toget;
			}
			for(;;) {
				if(toget == 0) {
					return true;
				}
				if(unlikely((ch = getch()) == EOF)) {
					break;
				}
				*ret = ((*ret) << 8) |
					static_cast<m_Tp>(static_cast<eix::UChar>(ch));
				--toget;
			}
			break;
		}
	}
	readError(errtext);
	return false;
}

template<typename m_Tp> bool Database::write_num(m_Tp t, std::string *errtext) {
GCC_DIAG_OFF(sign-conversion)
	eix::UChar c(t & 0xFFU);
GCC_DIAG_ON(sign-conversion)
	// Test the most common case explicitly to speed up:
	if(t == static_cast<m_Tp>(c)) {
		if(counting) {
			if(unlikely(c == MAGICNUMCHAR)) {
				counter += 2;
			} else {
				++counter;
			}
			return true;
		}
		if(likely(putch(c))) {
			if(likely(c != MAGICNUMCHAR)) {
				return true;
			}
			// write leading 0 as flag:
			if(likely(putch(0))) {
				return true;
			}
		}
	} else {
		m_Tp mask(0xFFU);
		unsigned int count(0);
		do {
			mask <<= 8;
GCC_DIAG_OFF(sign-conversion)
			mask |= 0xFFU;
GCC_DIAG_ON(sign-conversion)
			++count;
		} while((t & mask) != t);
		// We have count > 0 here
		if(counting) {
GCC_DIAG_OFF(sign-conversion)
			eix::UChar d((t >> (8*count)) & 0xFFU);
			counter += ((unlikely(d == MAGICNUMCHAR)) ? 2 : 1) + (2 * count);
GCC_DIAG_ON(sign-conversion)
			return true;
		}
		for(unsigned int r(count); ;) {
			if(unlikely(!putch(MAGICNUMCHAR))) {
				break;
			}
			if(--r == 0) {
GCC_DIAG_OFF(sign-conversion)
				eix::UChar d((t >> (8*count)) & 0xFFU);
GCC_DIAG_ON(sign-conversion)
				if(unlikely(!putch(d))) {
					break;
				}
				if(unlikely(d == MAGICNUMCHAR)) {
					// write leading 0 as flag:
					if(unlikely(!putch(0))) {
						break;
					}
				}
				// neither rely on (t>>0)==t nor use count-- when count==0:
				while(--count != 0) {
GCC_DIAG_OFF(sign-conversion)
					if(unlikely(!putch((t >> (8*count)) & 0xFFU))) {
GCC_DIAG_ON(sign-conversion)
						break;
					}
				}
				if(likely(count == 0)) {
					if(likely(putch(c))) {
						return true;
					}
				}
				break;
			}
		}
	}
	writeError(errtext);
	return false;
}

#endif  // SRC_DATABASE_IO_H_
