
// MFCApplication2.h : MFCApplication2 Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMFCApplication2App:
// �йش����ʵ�֣������ MFCApplication2.cpp
//

class CMFCApplication2App : public CWinAppEx
{
public:
	CMFCApplication2App();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileOpen();
	afx_msg void OnVideoOpen();
//	afx_msg void OnPicReset();
//	afx_msg void OnFileSave();
};

extern CMFCApplication2App theApp;
extern Mat srcImage;
extern Mat dstImage;
extern VideoCapture video;
