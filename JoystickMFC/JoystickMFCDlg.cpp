// JoystickMFCDlg.cpp : implementation file
///////////////////////////////////////////////////////////////////////////////
//Based on:
// Raw Input API sample showing joystick support
//
// Author: Alexander Böcken
// Date:   04/22/2011
//
// Copyright 2011 Alexander Böcken
//
///////////////////////////////////////////////////////////////////////////////
//MSVS 2015 pro implementation V.Petrov 2016
/////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include "stdafx.h"
#include "JoystickMFCDlg.h"
#include "Joystick.h"
#include <hidsdi.h>
#include "Resource.h"


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

														// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
// CJoystickMFCDlg dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int idBtn[] =
{
	IDC_CHECK1,
	IDC_CHECK2,
	IDC_CHECK3,
	IDC_CHECK4,
	IDC_CHECK5,
	IDC_CHECK6,
	IDC_CHECK7,
	IDC_CHECK8,
	IDC_CHECK9,
	IDC_CHECK10,
	IDC_CHECK11,
	IDC_CHECK12,
	IDC_CHECK13,
};

int idHat[] =
{
	IDC_HAT1,
	IDC_HAT2,
	IDC_HAT3,
	IDC_HAT4,
	IDC_HAT5,
	IDC_HAT6,
	IDC_HAT7,
	IDC_HAT8,
};

void DrawCrossXOR(CDC * pDC, CPoint pt)
{
	int r2 = pDC->SetROP2(R2_NOTXORPEN);
	pDC->MoveTo(pt + CSize(0, 5));
	pDC->LineTo(pt + CSize(0, -5));
	pDC->MoveTo(pt + CSize(5, 0));
	pDC->LineTo(pt + CSize(-5, 0));
	pDC->SetROP2(r2);
}
// Диалоговое окно CAboutDlg используется для описания сведений о приложении




void CJoystickMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

CJoystickMFCDlg::CJoystickMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JOYSTICKMFC_DIALOG, pParent)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

BEGIN_MESSAGE_MAP(CJoystickMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_INPUT()
	ON_BN_CLICKED(IDOK, &CJoystickMFCDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CJoystickMFCDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


BOOL CJoystickMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);
	
	//
	// Joystick Initialization
	//

	CStatic * pWnd = (CStatic *)GetDlgItem(IDC_STATIC_XY);
	try {
		this->joystick = Joystick(m_hWnd, pWnd);
	} catch (CString e) {
		MessageBox(e, "Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}

void CJoystickMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialogEx::OnSysCommand(nID, lParam);

}

void CJoystickMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CJoystickMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CJoystickMFCDlg::OnRawInput(UINT nInputcode, HRAWINPUT lParam)
{
	if (this->joystick.GetAndParseRawInput(nInputcode, lParam) < 0)
		return;

	for (int i = 0; i < 12; i++)  //Update All the Numerated Check Boxes
	{
		CheckDlgButton(idBtn[i], joystick.bButtonStates[i] ? BST_CHECKED : BST_UNCHECKED);
		if (i == 1)
			CheckDlgButton(idBtn[12], joystick.bButtonStates[i] ? BST_CHECKED : BST_UNCHECKED);
	}
	for (int i = 0; i < 8; i++)   //Update All the UnNumerated Check Boxes
		CheckDlgButton(idHat[i], i == joystick.m_h ? BST_CHECKED : BST_UNCHECKED);


	CPoint pt = joystick.m_ptXY0 + CSize((int)(joystick.m_x*joystick.m_scaleX), (int)(joystick.m_y*joystick.m_scaleY));     //Calc Joystick XY position
	CPoint ptZ = joystick.m_ptXY0 + CSize((int)(joystick.m_zr*joystick.m_scaleX), (int)(joystick.m_z*joystick.m_scaleY));   //Calc Joystick zr and z position
	if (pt != joystick.m_ptXY || ptZ != joystick.m_ptZ) //If any position changed
	{
		DrawScene();                  //Clear the Cross in Previous position
		joystick.m_ptXY = pt;                  //Apply new pos as current
		joystick.m_ptZ = ptZ;
		DrawScene();                  //Draw Cross in the new pos

		CStatic * pWnd = (CStatic *)GetDlgItem(IDC_STATIC_XY_TEXT);//Get Text Info Window
		CString aString;
		aString.Format(_T("x = %d  y = %d"), joystick.m_x, joystick.m_y);//Type position
		pWnd->SetWindowText(aString);                  //Show new position

		pWnd = (CStatic *)GetDlgItem(IDC_STATIC_Z_TEXT);//Get Text Info Window
		aString.Format(_T("z = %d  rz = %d"), joystick.m_z, joystick.m_zr);//Type position
		pWnd->SetWindowText(aString);                    //Show new position
	}

	CDialogEx::OnRawInput(nInputcode, lParam);
}

void CJoystickMFCDlg::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CJoystickMFCDlg::DrawScene(void)
{

	CStatic * pWnd = (CStatic *)GetDlgItem(IDC_STATIC_XY);
	DrawCrossXOR(pWnd->GetDC(), joystick.m_ptXY);
	pWnd = (CStatic *)GetDlgItem(IDC_STATIC_Z);
	DrawCrossXOR(pWnd->GetDC(), joystick.m_ptZ);
}
////////////////////////////////////////////////////////////



void CJoystickMFCDlg::OnBnClickedButton1()
{
	CAboutDlg dlg;
	dlg.DoModal();
}
