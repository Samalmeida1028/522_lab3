bin_PROGRAMS = phaverlite phaverlite_static

AM_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/src \
@extra_includes@

phaverlite_LDADD = @extra_libraries@

phaverlite_static_LDFLAGS = -all-static $(phaverlite_LDFLAGS)
phaverlite_static_LDADD = $(phaverlite_LDADD)
phaverlite_static_SOURCES = $(phaverlite_SOURCES)

AM_YFLAGS = -d

parser_files = \
lexer.ll \
parser.yy

BUILT_SOURCES = \
lexer.cc \
parser.cc \
parser.hh

CLEANFILES = $(BUILT_SOURCES)

phaverlite_SOURCES = \
$(parser_files) \
automaton.cc \
automaton_compose.cc \
automaton_reach.cc \
automaton_refine.cc \
automaton.hh \
clock_val_set.cc \
clock_val_set.hh \
convex_clock_val_set.cc \
convex_clock_val_set.hh \
deriv_def.cpp \
deriv_def.h \
deriv_def_osc.h \
derivify.cpp derivify.h \
deriv_ops.cpp \
deriv_ops.h \
extended_pplite.cc \
extended_pplite.hh \
fp_interface.cpp \
fp_interface.h \
general.cc \
general.hh \
linear_vtef.cc \
linear_vtef.hh \
location.cc \
location.hh \
Makefile.am \
PFunction.cc \
PFunction.hh \
parameters.cc \
parameters.hh \
phaverlite.cc \
rat_aff_expr.cc \
rat_aff_expr.hh \
stopwatch.cc \
stopwatch.hh \
symb_states.cc \
symb_states.hh \
symb_states_type.cc \
symb_states_type.hh \
tnt_array1d.h \
tnt_array1d_utils.h \
tnt_array2d.h \
tnt_array2d_utils.h \
tnt_array3d.h \
tnt_array3d_utils.h \
tnt_cmat.h \
tnt_fortran_array1d.h \
tnt_fortran_array1d_utils.h \
tnt_fortran_array2d.h \
tnt_fortran_array2d_utils.h \
tnt_fortran_array3d.h \
tnt_fortran_array3d_utils.h \
tnt.h \
tnt_i_refvec.h \
tnt_math_utils.h \
tnt_sparse_matrix_csr.h \
tnt_stopwatch.h \
tnt_subscript.h \
tnt_vec.h \
tnt_version.h \
transition.cc \
transition.hh \
varid_map.cc \
varid_map.hh
