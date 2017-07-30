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

#include "stdafx.h"
#include "JoystickMFCDlg.h"
#include <hidsdi.h>
#include "Resource.h"
#include "afxdialogex.h"


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

#define SAFE_FREE(p)	{ if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }


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



static HANDLE               hHeap;
static 	PHIDP_PREPARSED_DATA pPreparsedData;
static 	PHIDP_BUTTON_CAPS    pButtonCaps;
static 	PHIDP_VALUE_CAPS     pValueCaps;
static USAGE    usage[MAX_BUTTONS];

int m_nPress = 0;

int  SaveFree(BOOL bRslt)
{
	SAFE_FREE(pPreparsedData);
	SAFE_FREE(pButtonCaps);
	SAFE_FREE(pValueCaps);
	if (!bRslt)
		m_nPress = 0;
	return bRslt;
}

void DrawCrossXOR(CDC * pDC, CPoint pt)
{
	int r2 = pDC->SetROP2(R2_NOTXORPEN);
	pDC->MoveTo(pt + CSize(0, 5));
	pDC->LineTo(pt + CSize(0, -5));
	pDC->MoveTo(pt + CSize(5, 0));
	pDC->LineTo(pt + CSize(-5, 0));
	pDC->SetROP2(r2);
}
/////////////////////////////////////////////
int CJoystickMFCDlg::ParseRawInput(PRAWINPUT pRawInput)
{
	HIDP_CAPS            Caps;
	USHORT               capsLength;
	UINT                 bufferSize;
	ULONG                i, usageLength, value;

	pPreparsedData = NULL;
	pButtonCaps = NULL;
	pValueCaps = NULL;
	hHeap = GetProcessHeap();

	//
	// Get the preparsed data block
	//

	if (GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) != 0)
		return SaveFree(FALSE);

	if (!(pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, bufferSize)))
		return SaveFree(FALSE);
	if ((int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) <0)
		return SaveFree(FALSE);

	//
	// Get the joystick's capabilities
	//

	// Button caps
	if (HidP_GetCaps(pPreparsedData, &Caps) != HIDP_STATUS_SUCCESS)
		return SaveFree(FALSE);

	if (!(pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps)))
		return SaveFree(FALSE);

	capsLength = Caps.NumberInputButtonCaps;
	if (HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS)
		return SaveFree(FALSE);

	g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

	// Value caps
	if (!(pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps)))
		return SaveFree(FALSE);
	capsLength = Caps.NumberInputValueCaps;
	if (HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS)
		return SaveFree(FALSE);

	//
	// Get the pressed buttons
	//

	usageLength = g_NumberOfButtons;
	if (
		HidP_GetUsages(
			HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
			(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
			) != HIDP_STATUS_SUCCESS)
		return SaveFree(FALSE);

	ZeroMemory(bButtonStates, sizeof(bButtonStates));
	for (i = 0; i < usageLength; i++)
		bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;


	//
	// Get the state of discrete-valued-controls
	//

	for (i = 0; i < Caps.NumberInputValueCaps; i++)
	{
		if (
			HidP_GetUsageValue(
				HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
				(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
				) != HIDP_STATUS_SUCCESS)
			return SaveFree(FALSE);

		switch (pValueCaps[i].Range.UsageMin)
		{
		case 0x30:	// X-axis
			m_x = (LONG)value - 128;
			break;

		case 0x31:	// Y-axis
			m_y = (LONG)value - 128;
			break;

		case 0x32: // Z-axis
			m_z = (LONG)value - 128;
			break;

		case 0x35: // Rotate-Z
			m_zr = (LONG)value - 128;
			break;

		case 0x39:	// Hat Switch
			m_h = value;
			break;
		}
	}//for (i = 0; i < Caps.NumberInputValueCaps; i++)


	
	return SaveFree(TRUE);

}
///////////////////////////////////////////////////////////


// Диалоговое окно CAboutDlg используется для описания сведений о приложении


CJoystickMFCDlg::CJoystickMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JOYSTICKMFC_DIALOG, pParent)
	, m_scaleX(1)
	, m_scaleY(1)
	, m_x(0)
	, m_y(0)
	, m_z(0)
	, m_zr(0)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CJoystickMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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
	// Joystick Initialization of Alexander Böcken
	//
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 1;
	rid.usUsage = 4;	// Joystick
	rid.dwFlags = 0;
	rid.hwndTarget = m_hWnd;
	if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
	{
		MessageBox(_T("No Joistick Available."), _T("Error:"), MB_OK | MB_ICONINFORMATION);
	}
	/////////////////////////////////////////////////////////////

	CStatic * pWnd = (CStatic *)GetDlgItem(IDC_STATIC_XY);
	CRect rect;
	pWnd->GetClientRect(&rect);
	m_ptXY0 = CPoint(rect.Width() / 2, rect.Height() / 2);
	m_scaleX = rect.Width() / 256.;
	m_scaleY = rect.Height() / 256.;
	m_ptXY = m_ptZ = CPoint(-1000, -1000);
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
	PRAWINPUT pRawInput = NULL;
	UINT      bufferSize = 0;
	HANDLE    hHeap;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

	hHeap = GetProcessHeap();
	pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
	if (!pRawInput)
		return;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
	ParseRawInput(pRawInput);

	HeapFree(hHeap, 0, pRawInput);

	for (int i = 0; i < 12; i++)  //Update All the Numerated Check Boxes
	{
		CheckDlgButton( idBtn[i], bButtonStates[i] ? BST_CHECKED : BST_UNCHECKED);
		if (i == 1)
			CheckDlgButton( idBtn[12], bButtonStates[i] ? BST_CHECKED : BST_UNCHECKED);
	}
	for (int i = 0; i < 8; i++)   //Update All the UnNumerated Check Boxes
		CheckDlgButton(idHat[i], i == m_h ? BST_CHECKED : BST_UNCHECKED);


	CPoint pt = m_ptXY0 + CSize((int)(m_x*m_scaleX), (int)(m_y*m_scaleY));     //Calc Joystick XY position
	CPoint ptZ = m_ptXY0 + CSize((int)(m_zr*m_scaleX), (int)(m_z*m_scaleY));   //Calc Joystick zr and z position
	if (pt != m_ptXY || ptZ != m_ptZ) //If any position changed
	{
		DrawScene();                  //Clear the Cross in Previous position
		m_ptXY = pt;                  //Apply new pos as current
		m_ptZ = ptZ;
		DrawScene();                  //Draw Cross in the new pos

		CStatic * pWnd = (CStatic *)GetDlgItem(IDC_STATIC_XY_TEXT);//Get Text Info Window
		CString aString;
		aString.Format(_T("x = %d  y = %d"), m_x, m_y);//Type position
		pWnd->SetWindowText(aString);                  //Show new position

		pWnd = (CStatic *)GetDlgItem(IDC_STATIC_Z_TEXT);//Get Text Info Window
		aString.Format(_T("z = %d  rz = %d"), m_z, m_zr);//Type position
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
	DrawCrossXOR(pWnd->GetDC(), m_ptXY);
	pWnd = (CStatic *)GetDlgItem(IDC_STATIC_Z);
	DrawCrossXOR(pWnd->GetDC(), m_ptZ);
}
////////////////////////////////////////////////////////////



void CJoystickMFCDlg::OnBnClickedButton1()
{
	CAboutDlg dlg;
	dlg.DoModal();
}
