#ifndef GUARD_deriv_def_h
#define GUARD_deriv_def_h

#include "tnt.h"
#include "derivify.h"

void pmos_I_DS(surreal& I_DS, surreal& dI_dGS, surreal& dI_dDS,
               const surreal& V_GS, const surreal& V_DS);

void get_derivative(const std::string& name,
                    TNT::Array1D< double > &x,
                    TNT::Array1D< double > &dx);

void get_derivative(const std::string& name,
                    TNT::Array1D< surreal > &x,
                    TNT::Array1D< surreal > &dx);

#endif
