/* PHAVerLite: PHAVer + PPLite.
   Copyright (C) 2018 Goran Frehse <goranf@gmail.com>
   Copyright (C) 2019-2023 Enea Zaffanella <enea.zaffanella@unipr.it>

This file is part of PHAVerLite.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <phaverlite-config.h>

#include "general.hh"
#include "parameters.hh"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

void throw_error(const string& s) {
  cout << "ERROR: " << s << endl;
  cout << "       (while parsing line " << line_number << ")." << endl;
  abort();
}

void throw_warning(const string& s) {
  cout << "\n======================\n";
  cout << "WARNING: " << s << endl;
  cout << "======================\n";
}

bool printing_time_info() {
  return (VERBOSE_LEVEL & 1) != 0;
}

unsigned get_indentation(unsigned level) {
  unsigned indent = 0;
  level /= 1000;
  while (level > 1) {
    level /= 2;
    indent += 2;
  }
  return indent;
}

void progress_dot(bool dotting) {
  static unsigned PROGRESS_DOT_COUNT = 0;
  if ((VERBOSE_LEVEL & 10) == 0)
    return;
  if (!dotting) {
    PROGRESS_DOT_COUNT = 0;
    cout << endl;
    return;
  }
  if (PROGRESS_DOT_COUNT == 0)
    cout << string(get_indentation(VERBOSE_LEVEL), ' ');
  ++PROGRESS_DOT_COUNT;
  cout << "." << std::flush;
  if (PROGRESS_DOT_COUNT >= 50) {
    PROGRESS_DOT_COUNT = 0;
    cout << endl;
  }
}

void message(unsigned level, unsigned indent, const string& s) {
  if (VERBOSE_LEVEL < level)
    return;
  cout << string(indent, ' ') << s << endl;
}

void message(unsigned int level, const string& s) {
  if (VERBOSE_LEVEL < level)
    return;
  auto indent = get_indentation(level);
  if (level < 4000 && (level % 10 == 0)) {
    cout << endl;
    message(level, indent, s);
    message(level, indent, string(s.size(), '-'));
    cout << endl;
  } else
    message(level, indent, s);
}

string int2string(const int i) {
  std::stringstream ss;
  ss << i;
  return ss.str();
}

string double2string(const double d, int precision) {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(precision) << d;
  return ss.str();
}

string string_before(const string& str, const string& c) {
  // Return all chars before encountering c.
  // Note: By definition, str is returned if c is empty or c is not found.
  //       This is so that string_before + c + string_after == str
  if (c.size() == 0)
    return str;
  string::size_type i = str.find(c);
  return (i == string::npos) ? str : str.substr(0, i);
}

string string_after(const string& str, const string& c) {
  // Return all chars after encountering c.
  // Note: By definition, nothing is returned if c is empty or c is not found.
  //       This is so that string_before + c + string_after == str
  if (c.size() == 0)
    return "";
  string::size_type i = str.find(c);
  if (i == string::npos || (i+1 == str.size()))
    return "";
  return str.substr(i + c.size(), str.size() - (i + c.size()));
}


bool wildcmp(const string& wild, const string& str) {
  // taken from http://www.codeproject.com/string/wildcmp.asp
  // which is adapted from code by Jack Handy - jakkhandy@hotmail.com
  uint cp=0, mp=0;

  uint i=0;
  uint j=0;
  while (i < str.length() && j < wild.length() && wild[j] != '$') {
    if ((wild[j] != str[i]) && (wild[j] != '?'))
      return false;
    i++;
    j++;
  }

  while (i<str.length()) {
    if (j<wild.length() && wild[j] == '$') {
      if ((j++)>=wild.length())
        return true;
      mp = j;
      cp = i+1;
    }
    else if (j<wild.length() && (wild[j] == str[i] || wild[j] == '?')) {
      j++;
      i++;
    } else {
      j = mp;
      i = cp++;
    }
  }

  while (j<wild.length() && wild[j] == '$') {
    j++;
  }
  return j>=wild.length();
}
