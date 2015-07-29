
// info_PE.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cinfo_PEApp:
// See info_PE.cpp for the implementation of this class
//

class Cinfo_PEApp : public CWinApp
{
public:
	Cinfo_PEApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cinfo_PEApp theApp;