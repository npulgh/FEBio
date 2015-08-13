// FEArrudaBoyce.cpp: implementation of the FEMArrudaBoyce class.
//
// After Kalliske & Rothert, Eng Computations 14(2)(1997):216-232
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEArrudaBoyce.h"

//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_PARAMETER_LIST(FEArrudaBoyce, FEUncoupledMaterial)
	ADD_PARAMETER2(m_mu, FE_PARAM_DOUBLE, FE_RANGE_GREATER(0.0), "mu");
	ADD_PARAMETER2(m_N , FE_PARAM_DOUBLE, FE_RANGE_GREATER(0.0), "N" );
END_PARAMETER_LIST();

//-----------------------------------------------------------------------------
mat3ds FEArrudaBoyce::DevStress(FEMaterialPoint& mp)
{
	FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();

	const double a[] = {0.5, 0.1, 11.0/350.0, 19.0/1750.0, 519.0/134750.0};

	// deformation gradient
	double J = pt.m_J;

	// left Cauchy-Green tensor and its square
	mat3ds B = pt.DevLeftCauchyGreen();

	// Invariants of B_tilde
	double I1 = B.tr();

	// strain energy derivative
	double f = I1/m_N;
	double W1 = m_mu*(a[0] + (2.0*a[1] + (3*a[2] + (4*a[3] + 5*a[4]*f)*f)*f)*f);

	// T = FdW/dCFt
	mat3ds T = B*W1;

	// deviatoric Cauchy stress is 2/J*dev(T)
	return T.dev()*(2.0/J);
}

//-----------------------------------------------------------------------------
tens4ds FEArrudaBoyce::DevTangent(FEMaterialPoint& mp)
{
	FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();

	const double a[] = {0.5, 0.1, 11.0/350.0, 19.0/1750.0, 519.0/134750.0};

	// deformation gradient
	double J = pt.m_J;
	double Ji = 1.0/J;

	// calculate deviatoric left Cauchy-Green tensor: B = F*Ft
	mat3ds B = pt.DevLeftCauchyGreen();

	// Invariants of B (= invariants of C)
	double I1 = B.tr();

	// --- TODO: put strain energy derivatives here ---
	// W1 = dW/dI1
	// W11 = d2W/dI1^2
	const double f = I1/m_N;
	double W1  = m_mu*(a[0] + (2*a[1] + (3*a[2] + (4*a[3] + 5*a[4]*f)*f)*f)*f);
	double W11 = 2.0*m_mu*(a[1] + (3*a[2] + (6*a[3] + 10*a[4]*f)*f)*f)/m_N;
	// ---

	// calculate dWdC:C
	double WC = W1*I1;

	// calculate C:d2WdCdC:C
	double CWWC = W11*I1*I1;

	// deviatoric cauchy-stress, trs = trace[s]/3
	mat3ds devs = pt.m_s.dev();

	// Identity tensor
	mat3ds I(1,1,1,0,0,0);

	tens4ds IxI = dyad1s(I);
	tens4ds I4  = dyad4s(I);
	tens4ds BxB = dyad1s(B);

	// d2W/dCdC:C
	mat3ds WCCxC = B*(W11*I1);

	tens4ds cw = BxB*(W11*4.0*Ji) - dyad1s(WCCxC, I)*(4.0/3.0*Ji) + IxI*(4.0/9.0*Ji*CWWC);

	tens4ds c = dyad1s(devs, I)*(-2.0/3.0) + (I4 - IxI/3.0)*(4.0/3.0*Ji*WC) + cw;

	return c;
}

//-----------------------------------------------------------------------------
double FEArrudaBoyce::DevStrainEnergyDensity(FEMaterialPoint& mp)
{
	FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
	const double a[] = {0.5, 0.1, 11.0/350.0, 19.0/1750.0, 519.0/134750.0};
    
	// left Cauchy-Green tensor and its square
	mat3ds B = pt.DevLeftCauchyGreen();
    
	// Invariants of B_tilde
	double I1 = B.tr();
    double I1i = I1, ti = 3, Ni = 1;
    
    double sed = a[0]*(I1-3);
    for (int i=1; i<5; ++i) {
        Ni *= m_N;
        ti *= 3;
        I1i *= I1;
        sed += a[i]*(I1i - ti)/Ni;
    }
    sed *= m_mu;
    
    return sed;
}