	const dimension_type V_pos=0;
	const dimension_type I_pos=1;
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

	surreal V=x[V_pos];
	surreal I=x[I_pos];

	const double scaleI=1000; // 1000;

	double h=0;
	if (real(V)>=0 && real(V)<=0.055)
	h=h1*V*V*V+h2*V*V+h3*V;
	else if (real(V)>=0.055 && real(V)<=0.35)
	h=h4*V*V*V+h5*V*V+h6*V+h7;
	else if (real(V)>=0.35)
	h=h8*V*V*V+h9*V*V+h10*V+h11;

	surreal dV=(scalet/C)*(-h+I/scaleI);
	surreal dI=(scalet/L)*(-V-I/scaleI/G+Vin)*scaleI;

	dx[V_pos]=dV;
	dx[I_pos]=dI;
