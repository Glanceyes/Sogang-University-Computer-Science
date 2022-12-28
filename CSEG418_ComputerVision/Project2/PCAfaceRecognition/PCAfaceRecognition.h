
// PCAfaceRecognition.h : main header file for the PCAfaceRecognition application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPCAfaceRecognitionApp:
// See PCAfaceRecognition.cpp for the implementation of this class
//

class CPCAfaceRecognitionApp : public CWinApp
{
public:
	CPCAfaceRecognitionApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPCAfaceRecognitionApp theApp;
