
// PCAfaceRecognitionView.h : interface of the CPCAfaceRecognitionView class
//
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "ConsoleWnd.h"

#pragma once


class CPCAfaceRecognitionView : public CView
{
protected: // create from serialization only
	CPCAfaceRecognitionView();
	DECLARE_DYNCREATE(CPCAfaceRecognitionView)

// Attributes
public:
	CPCAfaceRecognitionDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CPCAfaceRecognitionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:

	ConsoleWnd con;
	afx_msg void OnFacerecognitionTrain();
	afx_msg void OnFacerecognitionTest();
	afx_msg void OnTest2();
	afx_msg void OnFileOpen();

	LARGE_INTEGER m_start, m_end, m_frequency;
	double m_result;
	CString m_cs;
	int	m_nFilterLoad;
	bool display;
	ATL::CImage m_probe;
	CString m_id;
	ATL::CImage m_match[10];
	int m_probeID, m_matchID[10];
	
};

#ifndef _DEBUG  // debug version in PCAfaceRecognitionView.cpp
inline CPCAfaceRecognitionDoc* CPCAfaceRecognitionView::GetDocument() const
   { return reinterpret_cast<CPCAfaceRecognitionDoc*>(m_pDocument); }
#endif

