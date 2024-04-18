AC_INIT([PHAVerLite],[0.6],[enea.zaffanella@unipr.it],[phaverlite])

# Specify the location of additional local Autoconf macros.
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign no-define dist-xz silent-rules 1.11])

# Generate a configuration header file.
AC_CONFIG_HEADERS([phaverlite-config.h])

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AH_TOP([
/* BEGIN phaverlite-config.h */

#ifndef phaverlite_config_h
#define phaverlite_config_h 1

])

AH_BOTTOM([
#if defined(PHAVERLITE_NDEBUG) && !defined(NDEBUG)
# define NDEBUG PHAVERLITE_NDEBUG
#endif

#endif /* !defined(phaverlite_config_h) */

/* END phaverlite-config.h */
])

# C compiler.

AC_ARG_WITH(cc,
  AS_HELP_STRING([--with-cc=XXX], [use XXX as the C compiler]),
  CC=$with_cc)

# C++ compiler.

AC_ARG_WITH(cxx,
  AS_HELP_STRING([--with-cxx=XXX], [use XXX as the C++ compiler]),
  CXX=$with_cxx)

# Checks for programs.

save_CFLAGS="$CFLAGS"
AC_PROG_CC
CFLAGS="$save_CFLAGS"

AM_CONDITIONAL(GCC, test x"$GCC" = xyes)

save_CXXFLAGS="$CXXFLAGS"
AC_PROG_CXX
CXXFLAGS="$save_CXXFLAGS"

# Check that the C++ compiler supports C++17 features.
AX_CXX_COMPILE_STDCXX(17, noext, mandatory)

AC_PROG_CXXCPP

# Set programs for lexer and parser code
AC_PROG_YACC
AC_PROG_LEX([noyywrap])

# Compilation flags.

COMP_FLAGS=""
OPT_FLAGS=""

enableval=yes
AC_MSG_CHECKING([whether to compile with debug info])
AC_ARG_ENABLE(debugging,
  AS_HELP_STRING([--enable-debugging], [compile with debugging information]))
case "${enableval}" in
yes)
  AC_MSG_RESULT(yes)
  OPT_FLAGS="-g"
  ;;
no)
  AC_MSG_RESULT(no)
  ;;
*)
  AC_MSG_ERROR([bad value ${enableval} for --enable-debugging, needs yes or no])
  ;;
esac
AM_CONDITIONAL(DEBUGGING_ENABLED, test x"$enableval" = xyes)

enableval=no
AC_MSG_CHECKING([whether to enable checking of run-time assertions])
AC_ARG_ENABLE(assertions,
  AS_HELP_STRING([--enable-assertions], [check run-time assertions]))
case "${enableval}" in
yes)
  AC_MSG_RESULT(yes)
  ;;
no)
  AC_MSG_RESULT(no)
  ;;
*)
  AC_MSG_ERROR([bad value ${enableval} for --enable-assertions, needs yes or no])
  ;;
esac
enable_assertions=${enableval}

if test x"$enable_assertions" = xno
then
  AC_DEFINE(PHAVERLITE_NDEBUG, 1, [Assertions are disabled when this is defined.])
  debug_flag="-DNDEBUG=1"
fi

AM_CONDITIONAL(ASSERTIONS_ENABLED, test x"$enable_assertions" = xyes)


arch=no
enableval=standard
AC_MSG_CHECKING([whether to enable optimizations])
AC_ARG_ENABLE(optimization,
  AS_HELP_STRING([--enable-optimization@<:@=LEVEL@:>@],
                 [enable compiler optimizations]))
case "${enableval}" in
sspeed)
  AC_MSG_RESULT(sspeed)
  OPT_FLAGS="$OPT_FLAGS -O3 -fomit-frame-pointer"
  arch=yes
  ;;
speed)
  AC_MSG_RESULT(speed)
  OPT_FLAGS="$OPT_FLAGS -O3"
  arch=yes
  ;;
size)
  AC_MSG_RESULT(size)
  OPT_FLAGS="$OPT_FLAGS -Os"
  arch=yes
  ;;
standard | yes)
  AC_MSG_RESULT(standard)
  OPT_FLAGS="$OPT_FLAGS -O2"
  ;;
mild)
  AC_MSG_RESULT(mild)
  OPT_FLAGS="$OPT_FLAGS -O1"
  ;;
zero | no)
  AC_MSG_RESULT(zero)
  OPT_FLAGS="$OPT_FLAGS -O0"
  ;;
*)
  AC_MSG_ERROR([bad value ${enableval} for --enable-optimization, needs sspeed, speed, size, standard, mild, zero, yes or no])
  ;;
esac

enableval=$arch
AC_MSG_CHECKING([for which architecture to optimize])
AC_ARG_ENABLE(arch,
  AS_HELP_STRING([--enable-arch@<:@=ARCH@:>@],
                 [optimize for architecture ARCH]))
case "${enableval}" in
yes)
  m=`uname -m`
  case $m in
  i?86 | k6 | athlon)
    AC_MSG_RESULT($m)
    OPT_FLAGS="$OPT_FLAGS -march=$m"
    ;;
  *)
    AC_MSG_RESULT(default)
    ;;
  esac
  ;;
no)
  AC_MSG_RESULT(default)
  ;;
*)
  AC_MSG_RESULT($enableval)
  OPT_FLAGS="$OPT_FLAGS -march=$enableval"
  ;;
esac


CFLAGS="$COMP_FLAGS $OPT_FLAGS $CFLAGS"
CXXFLAGS="$COMP_FLAGS $OPT_FLAGS $CXXFLAGS"

# Allow additions to C compilation flags.
AC_ARG_WITH(cflags,
  AS_HELP_STRING([--with-cflags=XXX],
                 [add XXX to the options for the C compiler]),
  CFLAGS="$CFLAGS $with_cflags")

# Allow additions to C++ compilation flags.
AC_ARG_WITH(cxxflags,
  AS_HELP_STRING([--with-cxxflags=XXX],
                 [add XXX to the options for the C++ compiler]),
  CXXFLAGS="$CXXFLAGS $with_cxxflags")

# Libtool.
LT_PREREQ([1.5.24])
LT_INIT([dlopen])

# Make sure libtool is used for linking.
AC_USE_LIBTOOL

# We want to compile with lots of warnings enabled.
if test x"$GCC" = xyes
then
  CFLAGS="$CFLAGS -Wall -Wextra"
fi
if test x"$GXX" = xyes
then
  CXXFLAGS="$CXXFLAGS -Wall -Wextra"
fi

# Checks on the GMP library.
AC_CHECK_GMP
if test x"$have_gmp" = xno
then
  AC_MSG_ERROR([Cannot find GMP version 4.1.3 or higher.])
else
  if test x"$have_gmpxx" = xno
  then
    AC_MSG_ERROR([GMP compiled without enabling the C++ interface.])
  fi
fi
extra_includes="${extra_includes}${extra_includes:+ }${gmp_include_options}"
extra_libraries="${extra_libraries}${extra_libraries:+ }${gmp_library_options}"

# Checks on the Flint library.
AC_CHECK_FLINT
if test x"$have_flint" = xno
then
  AC_MSG_ERROR([Cannot find Flint version 2.5.2 or higher.])
fi
extra_includes="${extra_includes}${extra_includes:+ }${flint_include_options}"
extra_libraries="${extra_libraries}${extra_libraries:+ }${flint_library_options}"

# Allow to specify the GNU MPFR installation directory
AC_ARG_WITH(mpfr,
  AS_HELP_STRING([--with-mpfr=DIR], [use the GNU MPFR installed in DIR]),
  mpfr_dir=$with_mpfr)
AC_ARG_WITH(mpfr-include,
  AS_HELP_STRING([--with-mpfr-include=DIR],
		 [search for GNU MPFR headers in DIR]))
AC_ARG_WITH(mpfr-lib,
  AS_HELP_STRING([--with-mpfr-lib=DIR],
		 [search for GNU MPFR library objects in DIR]))
if test -n "$with_mpfr"
then
  mpfr_include_options="-I$with_mpfr/include"
  mpfr_library_options="-L$with_mpfr/lib"
fi
if test -n "$with_mpfr_include"
then
  mpfr_include_options="-I$with_mpfr_include"
fi
if test -n "$with_mpfr_lib"
then
  mpfr_library_options="-L$with_mpfr_lib"
fi
extra_includes="${extra_includes}${extra_includes:+ }${mpfr_include_options}"
extra_libraries="${extra_libraries}${extra_libraries:+ }${mpfr_library_options}"

# Allow to specify the PPLite installation directory
AC_ARG_WITH(pplite,
  AS_HELP_STRING([--with-pplite=DIR], [use the PPLite installed in DIR]),
  pplite_dir=$with_pplite)
AC_ARG_WITH(pplite-include,
  AS_HELP_STRING([--with-pplite-include=DIR],
		 [search for PPLite headers in DIR]))
AC_ARG_WITH(pplite-lib,
  AS_HELP_STRING([--with-pplite-lib=DIR],
		 [search for PPLite library objects in DIR]))
if test -n "$with_pplite"
then
  pplite_include_options="-I$with_pplite/include"
  pplite_library_options="-L$with_pplite/lib"
fi
if test -n "$with_pplite_include"
then
  pplite_include_options="-I$with_pplite_include"
fi
if test -n "$with_pplite_lib"
then
  pplite_library_options="-L$with_pplite_lib"
fi
extra_includes="${extra_includes}${extra_includes:+ }${pplite_include_options}"
extra_libraries="${extra_libraries}${extra_libraries:+ }${pplite_library_options}"

# Completing extra_libraries (note: order matters for static builds):
# link with PPLite, FLINT, GNU MPFR, GMP.
extra_libraries="${extra_libraries}${extra_libraries:+ }-lpplite -lflint -lmpfr -lgmpxx -lgmp"

AC_SUBST(extra_includes)
AC_SUBST(extra_libraries)

AC_SUBST(debug_flag)

AC_CONFIG_FILES(Makefile
                src/Makefile
                )
AC_OUTPUT
