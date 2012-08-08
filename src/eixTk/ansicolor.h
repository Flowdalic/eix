// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <vaeth@mathematik.uni-wuerzburg.de>

#ifndef SRC_EIXTK_ANSICOLOR_H_
#define SRC_EIXTK_ANSICOLOR_H_ 1

#include <config.h>

#include <ostream>
#include <string>
#include <vector>

// include <iostream> this comment satisfies check_include script

class AnsiColor;

/** A class for using ANSI markers
 * Example:
 * @code
 * AnsiMarker m_inverse(AnsiMarker::amInverse);
 * std::cout << m_inverse << "inverse text" << m_inverse.end() << "normal text";
 * @endcode
 */
class AnsiMarker {
	private:
		friend class AnsiColor;
		static const char *reset_string;
		void calc_string();

	public:
		/** The various markers, amNone is no marker */
		enum Marker { amNone = 0, amBold = 1, amUnderlined = 4, amBlink = 5, amInverse = 7, amIllegal };
		std::vector<Marker> markers;
		bool have_something;
		std::string string_begin;

		AnsiMarker() : have_something(false)
		{ }

		explicit AnsiMarker(const Marker m) : have_something(false)
		{
			if(m != amNone) {
				markers.assign(1, m);
				calc_string();
			}
		}

		static const char *reset() ATTRIBUTE_PURE
		{ return reset_string; }

		const std::string &asString() const
		{ return string_begin; }

		const char *end() const
		{
			if(have_something)
				return reset();
			return "";
		}

		bool initmarker(const std::string &markers_string, std::string *errtext);
};

inline std::ostream&
operator<< (std::ostream& os, AnsiMarker am)
{
	os << am.asString();
	return os;
}

/** A class for using ANSI color codes
 * @note Currently only handles foreground color.
 * Example:
 * @code
 * AnsiColor c_red(AnsiColor::acRed, false);
 * std::cout << "Normal text, " << c_red << "red text" << AnsiColor(AnsiColor::acDefault) << "normal text";
 * @endcode
 */
class AnsiColor {
	public:
		/** The various colors; acDefault is the default color, acNone is no color change */
		enum Color { acDefault = 0, acBlack = 30, acRed, acGreen, acYellow, acBlue, acPurple, acCyan, acGray, acNone, acIllegal };

		/** The current color */
		Color fg;

		/** true -> bright text */
		bool light;

		/** The actual string */
		std::string string_begin;

		/** The actual string */
		bool have_something;

		/** reset */
		static const char *reset()
		{ return AnsiMarker::reset(); }

		/** additional markers */
		AnsiMarker mk;

		AnsiColor() : fg(acNone), light(false), have_something(false)
		{ }

		/** Constructor
		 * @param fg foreground color
		 * @param light brightness (normal, bright)
		 * defaults to no color output */
		AnsiColor(const Color f, const bool l = false) : fg(f), light(l), have_something(false)
		{ calc_string(); }

		void calc_string();

		const std::string &asString() const
		{ return string_begin; }

		bool initcolor(const std::string &color_name, std::string *errtext);

		static void init_static();
};

inline std::ostream&
operator<< (std::ostream& os, AnsiColor ac)
{
	os << ac.asString();
	return os;
}

#endif  // SRC_EIXTK_ANSICOLOR_H_
