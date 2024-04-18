#include <phaverlite-config.h>

#include "deriv_ops.h"
#include "deriv_def.h"
#include "derivify.h"

void
get_jacobian(const std::string& name,
             TNT::Array1D<double>& x,
             TNT::Array2D<double>& Jx) {
  // Generic function:
  // returns the jacobian (df_i,dx_j) for the function dx/dt=f(x) defined by
  // void get_derivative(TNT::Array1D< surreal > &x,TNT::Array1D< surreal > &dx)
  // in "deriv_def.cpp"

  // for now: h=1;
  const double h=1;

  TNT::Array1D< surreal > xs(x.dim());
  // initialize xs
  for (int j=0;j<x.dim();++j)
  {
    xs[j]=surreal(x[j],0.0);
  };

  TNT::Array1D< surreal > f(x.dim());

  for (int j=0;j<x.dim();++j)
  {
    // let x(j) vary
    xs[j]=surreal(x[j],h);
    get_derivative(name,xs,f);
    for (int i=0;i<x.dim();++i)
    {
      Jx[i][j]=imag(f[i])/h;
    };
    // make x(j) constant again
    xs[j]=surreal(x[j],0.0);
  };
}
