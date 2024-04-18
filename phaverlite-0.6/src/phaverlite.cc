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

#include "parameters.hh"
#include "general.hh"
#include "stopwatch.hh"

// Parser related stuff
#ifndef NDEBUG
#define YYDEBUG 1
#endif

#include "clock_val_set.hh"
#include "rat_aff_expr.hh"
#include "symb_states_type.hh"
#include "parser.hh"

#include <iostream>
#include <string>
void parse_file(const std::string& filename);
int yylex_destroy();

const char* const phaverlite_name = "PHAVerLite 0.6";

void welcome_message() {
  std::cout << phaverlite_name
            << " (compiled " << __DATE__ << ", " << __TIME__ << ")"
            << std::endl;
}

void help_message() {
  std::ostream& os = std::cout;
  using std::endl;
  os << "Usage: phaverlite [OPTIONS] <filename>"
     << " [ [OPTIONS] <filename> ... ]\n\n";
  os << "Command line options:\n";
  os << " -v  detailed output and timers, e.g. -v32011:\n";
  os << "     -vXXXXXX : higher numbers yield more info, "
    "in exponential scale.\n";
  os << "                Default is 8001, 32000 is detailed, 256000 "
    "is for debugging.\n";
  os << "     -vXXXXX1 : Shows timers\n";
  os << "     -vXXXX1X : Shows progress-dots (.....)\n";
#ifndef NDEBUG
  os << " -y  : debug parser\n";
#endif
  os << "\n";
  os << "PHAVerLite is derived from PHAVer and uses PPLite\n";
  os << "(note: only a subset of PHAVer's functions are supported)\n\n";
  os << "Info on PHAVer (e.g., its syntax): ";
  os << "www-verimag.imag.fr/~frehse/phaver_web/\n";
  os << "Info on PPLite: github.com/ezaffanella/PPLite\n\n";
  os << "Copyright (C) 2018 Goran Frehse\n";
  os << "Copyright (C) 2019-2023 Enea Zaffanella" << endl;
}

void
process_flag(const char* flag) {
  switch (flag[0]) {
  case 'h':
    // output more info, then quit
    welcome_message();
    help_message();
    break;
  case 'v' :
    // verbose level, higher is more
    if (strlen(flag) < 2)
      throw_warning("Illegal flag: -v<integer> required");
    else {
      VERBOSE_LEVEL = atoi(flag+1);
      message(256000, "Setting output level to "
              + int2string(VERBOSE_LEVEL) + ".");
    }
    break;
  case 'd' :
    // debug output, extremely verbose
    if (strlen(flag) < 2)
      DEBUG_OUTPUT = 1;
    else {
      DEBUG_OUTPUT = atoi(flag+1);
      message(256000, "Setting debug output level to "
              + int2string(DEBUG_OUTPUT) + ".");
    }
    break;
  case 'y' :
#ifndef NDEBUG
    yydebug = 1;
#else
    throw_warning("Debug parser mode not available for optimized build");
#endif
    break;

  case 'p':
    {
      // poly kind: example "-p=Poly", "-p=F_Poly" (no white space!)
      const auto sz = strlen(flag);
      if (sz < 3 || flag[1] != '=')
        throw_error("Error on flag -p: usage -p=<poly_kind_name>");
      std::string name(flag+2, flag+sz);
      if (!set_poly_kind(name, true))
        throw_error("Error on flag -p: invalid poly kind name");
    }
    break;

  default:
    std::cerr << "Warning: illegal flag (" << flag << ") ignored\n";
    break;
  }
}

void
process_flags(int argc, const char* argv[]) {
  if (argc == 1) {
    welcome_message();
    help_message();
    return;
  }

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      // process as a flag.
      process_flag(argv[i] + 1);
    } else {
      // process as input file
      std::string filename = argv[i];
      message(2001, "Parsing file " + filename);
      parse_file(filename);
    }
  }
}

int main(int argc, const char* argv[]) {

#ifndef NDEBUG
  yydebug = 0;
#endif

  {
    std::cout << phaverlite_name << ": starting" << std::endl;
    stopwatch sw(1, "PHAVerLite");
    process_flags(argc, argv);
  }

#if 0 // NITPICKER CLEANUP (not really needed)
  // Cleanup allocated lex resources.
  yylex_destroy();
#endif
  return 0;
}
