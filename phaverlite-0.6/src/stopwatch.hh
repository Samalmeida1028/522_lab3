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

#ifndef GUARD_stopwatch_hh
#define GUARD_stopwatch_hh

#include <string>
#include <ctime>

class stopwatch {
public:
  stopwatch();
  explicit stopwatch(const std::string& s);
  stopwatch(unsigned level, const std::string& s);
  ~stopwatch();

  void report();
  void report_delta();
  void report_delta(const std::string& s);

  double value();
  double delta();

private:
  clock_t start;
  clock_t report_time;
  std::string name;
  unsigned verbose_level;
};

#endif // GUARD_stopwatch_hh
