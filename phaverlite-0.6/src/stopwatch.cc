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

#include "stopwatch.hh"
#include "parameters.hh"
#include "general.hh"

#include <iostream>
#include <time.h>

using std::cout;
using std::endl;

stopwatch::stopwatch(unsigned level, const std::string& s)
  : start(clock()), report_time(clock()),
    name(s), verbose_level(level) {}

stopwatch::stopwatch() : stopwatch(0, "") {}
stopwatch::stopwatch(const std::string& s) : stopwatch(0, s) {}

stopwatch::~stopwatch() {
  if (printing_time_info())
    message(verbose_level,
            "Time in " + name + ": " + double2string(value()) + " secs");
}

double
stopwatch::value() {
  clock_t total = clock() - start; // get elapsed time
  return (long double)(total) / (long double)(CLOCKS_PER_SEC);
}

double
stopwatch::delta() {
  // returns the time since the last report or delta call
  clock_t total = clock() - report_time; //get elapsed time
  report_time = clock();
  return (long double)(total) / (long double)(CLOCKS_PER_SEC);
}

void
stopwatch::report() {
  // output the clock time and remember the report time
  report_time = clock();
  cout << "Current time in " << name << ": " << value() << "s" <<endl;
}

void
stopwatch::report_delta() {
  // output the time since last report and remember the report time
  cout << "Delta-Time in " << name << ": " << delta() << "s" << endl;
}

void
stopwatch::report_delta(const std::string& s) {
  // output the time since last report and remember the report time
  cout << "Delta-Time in " << name << " after " << s << ": "
       << delta() << "s" << endl;
}
