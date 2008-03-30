// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <iostream>
#include <string>
#include <vector>

#include <cerrno>

#include <eixTk/formated.h>
#include <eixTk/stringutils.h>

/// Simple exception class with printf-like formating.
class ExBasic : public std::exception
{
	public:
		/// Set name of function where exception is constructed.
		ExBasic(const char *func, const std::string &fmt)
			: m_func(func), m_formated(fmt)
		{ }

		virtual ~ExBasic() throw() { }

		/// return signature of throwing function
		virtual const std::string& from() const throw()
		{ return m_func; }

		/// Return reference to message.
		virtual const std::string& getMessage() const throw()
		{
			if (m_cache.empty())
				m_cache = m_formated.str();
			return m_cache;
		}

		/// @see std::exception::what()
		virtual const char *what() const throw()
		{ return getMessage().c_str(); }

		/// Replace placeholder in format string.
		template<typename T>
		ExBasic &operator % (const T& t)
		{
			m_formated % t;
			return *this;
		}

	private:
		std::string m_func;   ///< Function that threw us.
		eix::format m_formated; ///< Message formating.
		mutable std::string m_cache; ///< Message that we got from m_formated;
};

/// Like ExBasic, but append system error message.
class SysError : public ExBasic
{
	public:
		SysError(const char *func, const std::string& format, int e = 0)
			: ExBasic(func, format),
			  m_error(strerror(e ? e : errno))
		{ }

		virtual ~SysError() throw() { }

		/// Return reference to message.
		virtual const std::string& getMessage() const throw()
		{
			if (m_cache.empty()) {
				m_cache = ExBasic::getMessage();
				if (! m_cache.empty())
					m_cache.append(": ");
				m_cache.append(m_error);
			}
			return m_cache;
		}

		/// Replace placeholder in format string.
		template<typename T>
		SysError &operator % (const T& t)
		{
			ExBasic::operator%(t);
			return *this;
		}

	private:
		const std::string m_error; // result of strerror
		mutable std::string m_cache; // formated message from ExBasic
};

/// Automatically fill in the argument for our exceptions.
#define ExBasic(s) ExBasic(__PRETTY_FUNCTION__, s)
#define SysError(s) SysError(__PRETTY_FUNCTION__, s)

inline std::ostream& operator<< (std::ostream& os, const ExBasic& e)
{ return os << e.from() << ": " << e.getMessage(); }

/// Provide a common look for error-messages for parse-errors in
/// portage.{mask,keywords,..}.
inline void
portage_parse_error(const std::string &file, const int line_nr, const std::string& line, const std::exception &e)
{
	std::cerr << "-- Invalid line in "<< file << "("<< line_nr <<"): \""
	     << line << "\"" << std::endl;

	// Indent the message correctly
	std::vector<std::string> lines = split_string(e.what(), "\n", false);
	for(std::vector<std::string>::iterator i = lines.begin();
		i != lines.end();
		++i)
	{
		std::cerr << "    " << *i << std::endl;
	}
	std::cerr << std::endl;
}

/// Provide a common look for error-messages for parse-errors in
/// portage.{mask,keywords,..}.
template<class Iterator>
inline void
portage_parse_error(const std::string &file, const Iterator &begin, const Iterator &line, const std::exception &e)
{
  portage_parse_error(file, std::distance(begin, line) + 1, *line, e);
}

namespace eix 
{ 
	template<bool B> 
		struct _StaticAssert; 

	template<> 
		struct _StaticAssert<true> 
		{ static void empty(void) { } }; 
} 

/** Static assertion of expr. 
 * 
 * Fail to compile if expr is false because _StaticAssert<T>::empty is only 
 * defined for _StaticAssert<true>::empty.  empty() is a empty function and the 
 * call should get optimized away be the compiler. 
 */ 
#define EIX_STATIC_ASSERT(expr) eix::_StaticAssert<expr>::empty() 

#endif /* __EXCEPTIONS_H__ */
