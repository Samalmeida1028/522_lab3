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

#ifndef GUARD_general_hh
#define GUARD_general_hh

#include <iostream>
#include <map>
#include <string>

void throw_error(const std::string& s);

void throw_warning(const std::string& s);

bool printing_time_info();
void progress_dot(bool dotting);

void message(unsigned level, const std::string& s);
void message(unsigned level, unsigned indent, const std::string& s);

std::string int2string(int i);
std::string double2string(double d, int precision = 3);

std::string string_before(const std::string& str, const std::string& c);
std::string string_after(const std::string& str, const std::string& c);

template <typename key, typename value>
typename std::map<key, value>::const_iterator
find(const std::map<key, value>& /*obj*/,
     typename std::map<key, value>::const_iterator i,
     typename std::map<key, value>::const_iterator iend,
     const value& v) {
  while (i != iend) {
    if (i->second == v)
      return i;
    ++i;
  }
  return iend;
}

bool wildcmp(const std::string& wild, const std::string& str);

#endif // GUARD_general_hh
