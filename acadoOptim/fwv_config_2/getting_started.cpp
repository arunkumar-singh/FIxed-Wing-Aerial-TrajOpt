#include <acado_code_generation.hpp>

int main( )
{
	USING_NAMESPACE_ACADO

	
	DifferentialState   x;      
	DifferentialState   y;      
	DifferentialState   z;      
	DifferentialState psi;	   
	DifferentialState v;
	DifferentialState phi;
	DifferentialState gamma;	

	Control a;                 
	Control phidot;
	Control gammadot;
	
	const double g = 10.0;  
	const double gamma_max = (40.0 * M_PI) / 180.0;
	const double phi_max = (40.0 * M_PI) / 180.0;
	const double v_max = 16.0;
	const double v_min = 12.0;
	
	const double a_max = 3.0;
	const double gammadot_max = (25.0 * M_PI) / 180.0;
	const double phidot_max = (25.0 * M_PI) / 180.0;
	

	DifferentialEquation f; 

	f << dot( x ) == v * cos(gamma) * cos(psi);
	f << dot( y ) == v * cos(gamma) * sin(psi);
	f << dot( z ) == - v * sin(gamma);
	f << dot( psi ) == (g/v) * tan(phi);
	f << dot(v) == a;
	f << dot(phi) == phidot;
	f << dot(gamma) == gammadot;
	

	// Reference functions and weighting matrices:
	Function h, hN;
	h << x << y << z;
	hN << x << y << z;

	// Provide defined weighting matrices:
	DMatrix W = eye<double>( h.getDim() );
	DMatrix WN = eye<double>( hN.getDim() );
		
	WN *= 100;
	
	OCP ocp(0.0, 13.8, 100);

	ocp.subjectTo( f );
	ocp.subjectTo( 0, a == 0);	
	ocp.subjectTo( -a_max <= a <= a_max );
	ocp.subjectTo( v_min <= v <= v_max );	
	ocp.subjectTo( -gammadot_max <= gammadot <= gammadot_max );
	ocp.subjectTo( -gamma_max <= gamma <= gamma_max );	
	ocp.subjectTo( -phidot_max <= phidot <= phidot_max );
	ocp.subjectTo( -phi_max <= phi <= phi_max);

	
	
	double obsx[13] = {10, 45, 50 ,70 , 110, 15 ,40 ,60, 100, 120, 80, 100, 0};
	double obsy[13] = {55, 50, 10, 70, 45, 100, 110, 105, 95, 80, 120, 0, 20}; 
	double obsz = 0.0;
	double A[13] =    {5, 5, 10, 5, 5, 12, 5, 7, 5, 5, 9, 5, 5}; 
	double B[13] =    {5 , 5, 10, 5, 5, 12, 5, 7, 6, 7, 9, 5, 5}; 
	double C[13] =    {35, 35, 65, 85, 25, 50, 55, 35, 50, 65, 75, 40, 45};
		
	for(int i = 0; i <13; i++)	
	{
		double AA = A[i]*A[i];
		double BB = B[i]*B[i];
		double CC = C[i]*C[i];	
		ocp.subjectTo(-pow(x - obsx[i],2)*BB*CC - pow(y - obsy[i],2)*AA*CC - pow(z - obsz,2)*BB*AA  <= -AA*BB*CC*1.0000001);
	}	

	ocp.minimizeLSQ(W, h);
	ocp.minimizeLSQEndTerm(WN, hN);

	// Export the code:
	OCPexport mpc( ocp );

	mpc.set( NUM_INTEGRATOR_STEPS, 30 );
	mpc.set( GENERATE_TEST_FILE, NO );
	mpc.set( GENERATE_MAKE_FILE, YES );
	//mpc.set( HESSIAN_APPROXIMATION, GAUSS_NEWTON );        
    //mpc.set( DISCRETIZATION_TYPE, SINGLE_SHOOTING );
    //mpc.set( INTEGRATOR_TYPE, INT_RK4 );
    //mpc.set( QP_SOLVER, QP_QPOASES );

	if (mpc.exportCode( "fwv_config_2" ) != SUCCESSFUL_RETURN)
		exit( EXIT_FAILURE );

	mpc.printDimensionsQP( );

	return EXIT_SUCCESS;
}
