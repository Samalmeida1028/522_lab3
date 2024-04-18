#ifndef GUARD_deriv_ops_h
#define GUARD_deriv_ops_h

#include "tnt.h"
#include <string>

// Derivative Computation
void get_jacobian(const std::string& name,
                  TNT::Array1D<double>& x,
                  TNT::Array2D<double>& Jx);

#endif
