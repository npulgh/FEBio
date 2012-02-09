#include "stdafx.h"
#include "FEModel.h"
#include <string>
using namespace std;

//-----------------------------------------------------------------------------
FEModel::FEModel(void)
{
	// --- Analysis Data ---
	m_pStep = 0;
	m_nStep = -1;
	m_nplane_strain = -1;	// don't use plain strain mode
	m_ftime = 0;
	m_ftime0 = 0;
	m_bwopt = 0;

	// add the "zero" loadcurve
	// this is the loadcurve that will be used if a loadcurve is not
	// specified for something that depends on time
	// TODO: I want to get rid of this 
	FELoadCurve* plc = new FELoadCurve();
	plc->Create(2);
	plc->LoadPoint(0).time = 0;
	plc->LoadPoint(0).value = 0;
	plc->LoadPoint(1).time = 1;
	plc->LoadPoint(1).value = 1;
	plc->SetExtendMode(FELoadCurve::EXTRAPOLATE);
	AddLoadCurve(plc);
}

//-----------------------------------------------------------------------------
//! Delete all dynamically allocated data
FEModel::~FEModel(void)
{
	size_t i;
	for (i=0; i<m_Step.size(); ++i) delete m_Step[i]; m_Step.clear();
	for (i=0; i<m_CI.size  (); ++i) delete m_CI [i] ; m_CI.clear  ();
	for (i=0; i<m_MAT.size (); ++i) delete m_MAT[i] ; m_MAT.clear ();
	for (i=0; i<m_LC.size  (); ++i) delete m_LC [i] ; m_LC.clear  ();
	for (i=0; i<m_BF.size  (); ++i) delete m_BF [i] ; m_BF.clear  ();
	for (i=0; i<m_DC.size  (); ++i) delete m_DC [i] ; m_DC.clear  ();
	for (i=0; i<m_FC.size  (); ++i) delete m_FC [i] ; m_FC.clear  ();
	for (i=0; i<m_SL.size  (); ++i) delete m_SL [i] ; m_SL.clear  ();
	for (i=0; i<m_RDC.size (); ++i) delete m_RDC[i] ; m_RDC.clear ();
	for (i=0; i<m_RFC.size (); ++i) delete m_RFC[i] ; m_RFC.clear ();
	for (i=0; i<m_RN.size  (); ++i) delete m_RN [i] ; m_RN.clear  ();
	for (i=0; i<m_NLC.size (); ++i) delete m_NLC[i] ; m_NLC.clear ();
	for (i=0; i<m_Obj.size (); ++i) delete m_Obj[i] ; m_Obj.clear ();
}

//-----------------------------------------------------------------------------
// This function adds a callback routine
//
void FEModel::AddCallback(FEBIO_CB_FNC pcb, void *pd)
{
	FEBIO_CALLBACK cb;
	cb.m_pcb = pcb;
	cb.m_pd = pd;

	m_pcb.push_back(cb);
}

//-----------------------------------------------------------------------------
// Call the callback function if there is one defined
//
void FEModel::DoCallback()
{
	list<FEBIO_CALLBACK>::iterator it = m_pcb.begin();
	for (int i=0; i<(int) m_pcb.size(); ++i, ++it)
	{
		// call the callback function
		(it->m_pcb)(this, it->m_pd);
	}
}

//-----------------------------------------------------------------------------
void FEModel::SetGlobalConstant(const string& s, double v)
{
	m_Const[s] = v;
	return;
}

//-----------------------------------------------------------------------------
double FEModel::GetGlobalConstant(const string& s)
{
	return (m_Const.count(s) ? m_Const.find(s)->second : 0);
}
