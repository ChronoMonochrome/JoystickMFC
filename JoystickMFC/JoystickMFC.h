
// JoystickMFC.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CJoystickMFCApp:
// � ���������� ������� ������ ��. JoystickMFC.cpp
//

class CJoystickMFCApp : public CWinApp
{
public:
	CJoystickMFCApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CJoystickMFCApp theApp;