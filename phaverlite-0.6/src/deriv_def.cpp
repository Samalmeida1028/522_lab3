#include <phaverlite-config.h>

#include "deriv_def.h"

void pmos_I_DS(surreal& I_DS, surreal& dI_dGS, surreal& dI_dDS,
               const surreal& V_GS, const surreal& V_DS) {
  const double V_tp    =   -0.69;
  const double K_p     =   86e-6;
  const double WdL     =   (240e-6)/(.25e-6);
  const double Omega_P =   -0.07;

  if (V_GS >= V_tp) {
    I_DS = 0;
    dI_dGS = 0;
    dI_dDS = 0;
  } else if (V_DS-V_GS >= -V_tp) {
    I_DS = -K_p*WdL*((-V_GS+V_tp)*(-V_DS)-1/2*V_DS*V_DS)*(1-Omega_P*V_DS);
    dI_dGS = -K_p*WdL*V_DS*(1-Omega_P*V_DS);
    dI_dDS = -K_p*WdL*(V_GS-V_tp-V_DS)*(1-Omega_P*V_DS)+K_p*WdL*(V_GS-V_tp-1/2*V_DS)*V_DS*(-Omega_P);
  } else {
    I_DS = -K_p/2*WdL*(V_GS-V_tp)*(V_GS-V_tp)*(1-Omega_P*V_DS);
    dI_dGS = -K_p*WdL*(V_GS-V_tp)*(1-Omega_P*V_DS);
    dI_dDS = -K_p/2*WdL*(V_GS-V_tp)*(V_GS-V_tp)*(-Omega_P);
  }
}

void get_derivative(const std::string& name,
                    TNT::Array1D< surreal > &x,
                    TNT::Array1D< surreal > &dx) {
  // Generic function:
  // returns the derivative (dx/dt) at point x
  if (name == "Osc") {
    const unsigned int V_pos=0;
    const unsigned int I_pos=1;
    const double scalet=1e-8;

    const double C=1e-12;
    const double G=5e-3;
    const double L=1e-6;
    const double Vin=0.3;

    const double h1=6.0105184072126220886551465063862;
    const double h2=-.99173553719008264462809917355372;
    const double h3=.54545454545454545454545454545455e-1;
    const double h4=.69248668180399272672582017743889e-1;
    const double h5=-.42068565919592558148593575779413e-1;
    const double h6=.39991105874180579968416115247096e-2;
    const double h7=.89578508243026036958230609967156e-3;
    const double h8=.26337448559670781893004115226337;
    const double h9=-.27654320987654320987654320987654;
    const double h10=.96790123456790123456790123456790e-1;
    const double h11=-.11181069958847736625514403292181e-1;

    const double scaleI=1000;

    surreal V=x[V_pos];
    surreal I=x[I_pos]/scaleI;

    double h=0;
    if (real(V)>=0 && real(V)<=0.055)
      h=h1*V*V*V+h2*V*V+h3*V;
    else if (real(V)>=0.055 && real(V)<=0.35)
      h=h4*V*V*V+h5*V*V+h6*V+h7;
    else if (real(V)>=0.35)
      h=h8*V*V*V+h9*V*V+h10*V+h11;

    surreal dV=(scalet/C)*(-h+I);
    surreal dI=(scalet/L)*(-V-I/G+Vin);

    dx[V_pos]=dV;
    dx[I_pos]=dI*scaleI;
  }
  else if (name == "Osc_t") {
    const unsigned int V_pos=0;
    const unsigned int I_pos=1;
    const unsigned int t_pos=2;
    // const double scalet=1e-8;
    const double scalet=1e-8;

    const double C=1e-12;
    const double G=5e-3;
    const double L=1e-6;
    const double Vin=0.3;

    const double h1=6.0105184072126220886551465063862;
    const double h2=-.99173553719008264462809917355372;
    const double h3=.54545454545454545454545454545455e-1;
    const double h4=.69248668180399272672582017743889e-1;
    const double h5=-.42068565919592558148593575779413e-1;
    const double h6=.39991105874180579968416115247096e-2;
    const double h7=.89578508243026036958230609967156e-3;
    const double h8=.26337448559670781893004115226337;
    const double h9=-.27654320987654320987654320987654;
    const double h10=.96790123456790123456790123456790e-1;
    const double h11=-.11181069958847736625514403292181e-1;

    const double scaleI=1000; // 1000;

    surreal V=x[V_pos];
    surreal I=x[I_pos]/scaleI;

    double h=0;
    if (real(V)>=0 && real(V)<=0.055)
      h=h1*V*V*V+h2*V*V+h3*V;
    else if (real(V)>=0.055 && real(V)<=0.35)
      h=h4*V*V*V+h5*V*V+h6*V+h7;
    else if (real(V)>=0.35)
      h=h8*V*V*V+h9*V*V+h10*V+h11;

    surreal dV=(scalet/C)*(-h+I);
    surreal dI=(scalet/L)*(-V-I/G+Vin);

    dx[V_pos]=dV;
    dx[I_pos]=dI*scaleI;
    dx[t_pos]=10;
  }
  else if (name == "vco_3d") {
    const unsigned int V1_pos=0;
    const unsigned int V2_pos=1;
    const unsigned int I1_pos=2;
    const double scalet=1e-10;
    const double scaled=10;

    // VCO Circuit
    const double V_DD    =   1.8;
    const double I_b     =   18e-3;
    const double C       =   3.43e-12;
    // const double V_ctrl  =   0;
    const double L       =   2.857e-9;
    const double R       =   3.7;

    surreal V1=x[V1_pos];
    surreal V2=x[V2_pos];
    surreal I1=x[I1_pos]/scaled;

    surreal I_DS1=0;
    surreal I_DS2=0;
    surreal dI1_dGS=0;
    surreal dI1_dDS=0;
    surreal dI2_dGS=0;
    surreal dI2_dDS=0;
    pmos_I_DS(I_DS1,dI1_dGS,dI1_dDS,V2-V_DD,V1-V_DD);
    pmos_I_DS(I_DS2,dI2_dGS,dI2_dDS,V1-V_DD,V2-V_DD);

    surreal dV1=-1/(C)*(I_DS1+I1);
    surreal dV2=-1/(C)*(I_DS2+I_b-I1);
    surreal dI1=1/(2*L)*(V1-V2-R*(2*I1-I_b));

    dx[V1_pos]=scalet*dV1;
    dx[V2_pos]=scalet*dV2;
    dx[I1_pos]=scalet*scaled*dI1;
  }
  else if (name == "vco") {
    const unsigned int V1_pos=0;
    const unsigned int V2_pos=1;
    const unsigned int dV1_pos=2;
    const unsigned int dV2_pos=3;
    const double scalet=1e-10;
    const double scaled=1e-10;

    // VCO Circuit
    const double V_DD    =   1.8;
    // const double I_b     =   18e-3;
    const double C       =   3.43e-12;
    // const double V_ctrl  =   0;
    const double L       =   2.857e-9;
    const double R       =   3.7;

    surreal V1=x[V1_pos];
    surreal V2=x[V2_pos];
    surreal dV1=x[dV1_pos]/scaled;
    surreal dV2=x[dV2_pos]/scaled;

    surreal I_DS1=0;
    surreal I_DS2=0;
    surreal dI1_dGS=0;
    surreal dI1_dDS=0;
    surreal dI2_dGS=0;
    surreal dI2_dDS=0;
    pmos_I_DS(I_DS1,dI1_dGS,dI1_dDS,V2-V_DD,V1-V_DD);
    pmos_I_DS(I_DS2,dI2_dGS,dI2_dDS,V1-V_DD,V2-V_DD);

    surreal dI1=dI1_dGS*dV2+dI1_dDS*dV1;
    surreal dI2=dI2_dGS*dV1+dI2_dDS*dV2;

    surreal ddV1=1/(2*L*C)*(V2+R*I_DS2+R*C*dV2-V1-2*L*dI1-R*I_DS1-R*C*dV1);
    surreal ddV2=1/(2*L*C)*(V1+R*I_DS1+R*C*dV1-V2-2*L*dI2-R*I_DS2-R*C*dV2);

    dx[V1_pos]=scalet*dV1;
    dx[V2_pos]=scalet*dV2;
    dx[dV1_pos]=scalet*scaled*ddV1;
    dx[dV2_pos]=scalet*scaled*ddV2;
  }
  else if (name == "vco_t") {
    const unsigned int V1_pos=0;
    const unsigned int V2_pos=1;
    const unsigned int dV1_pos=2;
    const unsigned int dV2_pos=3;
    const unsigned int t_pos=4;
    const double scalet=1e-10;
    const double scaled=1e-10;

    // VCO Circuit
    const double V_DD    =   1.8;
    // const double I_b     =   18e-3;
    const double C       =   3.43e-12;
    // const double V_ctrl  =   0;
    const double L       =   2.857e-9;
    const double R       =   3.7;

    surreal V1=x[V1_pos];
    surreal V2=x[V2_pos];
    surreal dV1=x[dV1_pos]/scaled;
    surreal dV2=x[dV2_pos]/scaled;

    surreal I_DS1=0;
    surreal I_DS2=0;
    surreal dI1_dGS=0;
    surreal dI1_dDS=0;
    surreal dI2_dGS=0;
    surreal dI2_dDS=0;
    pmos_I_DS(I_DS1,dI1_dGS,dI1_dDS,V2-V_DD,V1-V_DD);
    pmos_I_DS(I_DS2,dI2_dGS,dI2_dDS,V1-V_DD,V2-V_DD);

    surreal dI1=dI1_dGS*dV2+dI1_dDS*dV1;
    surreal dI2=dI2_dGS*dV1+dI2_dDS*dV2;

    surreal ddV1=1/(2*L*C)*(V2+R*I_DS2+R*C*dV2-V1-2*L*dI1-R*I_DS1-R*C*dV1);
    surreal ddV2=1/(2*L*C)*(V1+R*I_DS1+R*C*dV1-V2-2*L*dI2-R*I_DS2-R*C*dV2);

    dx[V1_pos]=scalet*dV1;
    dx[V2_pos]=scalet*dV2;
    dx[dV1_pos]=scalet*scaled*ddV1;
    dx[dV2_pos]=scalet*scaled*ddV2;
    dx[t_pos]=1;
  }
  else if (name == "circ") {
    const unsigned int V_pos=0;
    const unsigned int I_pos=1;

    surreal V=x[V_pos];
    surreal I=x[I_pos];

    surreal dV=-I;
    surreal dI=V;

    dx[V_pos]=dV;
    dx[I_pos]=dI;
  }
  else if (name == "ellipse") {
    const unsigned int V_pos=0;
    const unsigned int I_pos=1;

    surreal V=x[V_pos];
    surreal I=x[I_pos];

    surreal dV=-0.3*I;
    surreal dI=2*V;

    dx[V_pos]=dV;
    dx[I_pos]=dI;
  }
}

void get_derivative(const std::string& name,
                    TNT::Array1D< double > &x,
                    TNT::Array1D< double > &dx) {
  TNT::Array1D< surreal > sx(x.dim());
  TNT::Array1D< surreal > sdx(dx.dim());
  for (int i = 0; i < x.dim(); ++i) {
    sx[i] = x[i];
    sdx[i] = dx[i];
  }
  get_derivative(name, sx, sdx);
  for (int i = 0; i < x.dim(); ++i) {
    x[i] = sx[i];
    dx[i] = sdx[i];
  }
}
