// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <martin@mvath.de>

#include "eixTk/percentage.h"
#include <config.h>

#include <string>

#include "eixTk/formated.h"
#include "eixTk/likely.h"

using std::string;

static string percent_to_string(PercentStatus::Percentage i);
static string percent_to_string(PercentStatus::Percentage i, string::size_type wanted);

static string percent_to_string(PercentStatus::Percentage i) {
	return (eix::format() % i);
}

static string percent_to_string(PercentStatus::Percentage i, string::size_type wanted) {
	string s(percent_to_string(i));
	if(s.size() < wanted) {
		return string(wanted - s.size(), ' ') + s;
	}
	return s;
}

void PercentStatus::init() {
	m_finished = false;
	m_size = 0;
	m_total = m_current = 0;
	m_format.clear();
	m_append.clear();
	m_total_s.clear();
}

void PercentStatus::init(const string& header) {
	m_verbose = false;
	m_format = header;
	reprint();
}

void PercentStatus::init(const string& format, Percentage total) {
	m_verbose = true;
	m_format = format;
	m_total = total;
	m_total_s = percent_to_string(total);
	reprint();
}

void PercentStatus::next(const string& append_string) {
	m_append = append_string;
	next();
	reprint();
}

void PercentStatus::next() {
	if((!m_verbose) || m_finished) {
		return;
	}
	if(m_current != m_total) {
		++m_current;
		if(m_current == m_total) {
			m_finished = true;
		}
	}
}

void PercentStatus::finish(const string& append_string) {
	m_append.clear();
	m_finished = true;
	if(m_verbose) {
		reprint();
	}
	eix::say(true) % append_string;
}

void PercentStatus::reprint() {
	string out;
	if(m_verbose) {
		if(m_finished) {
			out.assign(eix::format(m_format)
				% m_total_s
				% m_total_s
				% "100");
		} else {
			Percentage p(0);
			if(likely(m_total != 0)) {
				p = (m_current * 100) / m_total;
			}
			out.assign(eix::format(m_format)
				% percent_to_string(m_current, m_total_s.size())
				% m_total_s
				% percent_to_string(p, 3));
		}
	} else {
		out.assign(m_format);
	}
	out.append(m_append);
	if(m_size == 0) {
		eix::print(true) % out;
		m_size = out.size();
	} else {
		if(out.size() < m_size) {
			string::size_type difference = m_size - out.size();
			m_size = out.size();
			out.append(difference, ' ');
			out.append(difference, '\b');
		} else {
			m_size = out.size();
		}
		eix::print("\r%s", true) % out;
	}
}

void PercentStatus::interprint_start() {
	eix::say_empty(true);
	m_size = 0;
}

