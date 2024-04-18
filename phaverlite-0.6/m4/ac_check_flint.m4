dnl A function to check for the existence and usability of Flint.
dnl Copyright (C) 2017 Enea Zaffanella (enea.zaffanella@unipr.it)
dnl
dnl pplite is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 3 of the License, or (at your
dnl option) any later version.
dnl
dnl pplite is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

AC_DEFUN([AC_CHECK_FLINT],
[
AC_ARG_WITH(flint,
  AS_HELP_STRING([--with-flint=DIR],
		 [search for libflint in DIR/include and DIR/lib]))

AC_ARG_WITH(flint-include,
  AS_HELP_STRING([--with-flint-include=DIR],
		 [search for libflint headers in DIR]))

AC_ARG_WITH(flint-lib,
  AS_HELP_STRING([--with-flint-lib=DIR],
		 [search for libflint library objects in DIR]))

if test -n "$with_flint"
then
  flint_include_options="-I$with_flint/include"
  flint_library_paths="$with_flint/lib"
  flint_library_options="-L$flint_library_paths"
fi

if test -n "$with_flint_include"
then
  flint_include_options="-I$with_flint_include"
fi

if test -n "$with_flint_lib"
then
  flint_library_paths="$with_flint_lib"
  flint_library_options="-L$flint_library_paths"
fi

ac_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $flint_include_options"
ac_save_LIBS="$LIBS"
LIBS="$LIBS $flint_library_options -lflint"

AC_LANG_PUSH(C++)

AC_MSG_CHECKING([for the Flint library])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <flint/flint.h>
#include <flint/fmpz.h>
#include <string>

#if __FLINT_VERSION < 2 || (__FLINT == 2 && __FLINT_VERSION_MINOR < 5) || (__FLINT_VERSION == 2 && __FLINT_VERSION_MINOR == 5 && __FLINT_VERSION_PATCHLEVEL < 2)
#FLINT version 2.5.2 or higher is required
#endif

int
main() {
  fmpz_t mp;
  fmpz_init(mp);
  fmpz_set_str(mp, "123456789", 10);
  std::string s = fmpz_get_str(0, 10, mp);
  return (s == "123456789") ? 0 : -1;
}
]])],
  AC_MSG_RESULT(yes)
  ac_cv_have_flint=yes,
  AC_MSG_RESULT(no)
  ac_cv_have_flint=no)

AC_LANG_POP(C++)
LIBS="$ac_save_LIBS"
CPPFLAGS="$ac_save_CPPFLAGS"

])
