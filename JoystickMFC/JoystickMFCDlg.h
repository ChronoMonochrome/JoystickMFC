
// JoystickMFCDlg.h : ���� ���������
//

#pragma once
#include "Resource.h"



#define MAX_BUTTONS		128
// ���������� ���� CJoystickMFCDlg
class CJoystickMFCDlg : public CDialogEx
{
	// ��������
public:
	CJoystickMFCDlg(CWnd* pParent = NULL);	// ����������� �����������

											// ������ ����������� ����
  #ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOYSTICKMFC_DIALOG };
	#endif

	//Joystick Condition Variables
	int m_x;     //X pos of XY box
	int m_y;     //Y pos of XY box
	int m_zr;    //X pos of XY box
	int m_z;     //Y pos of XY box
	int m_h;	 //hat switch number pressed 	

	CPoint m_ptXY;    //Point in IDC_STATIC_BOX Static Box 
	CPoint m_ptZ;	  //Point in IDC_STATIC_Z Static Box
	CPoint m_ptXY0;	  //Initial Point Position 
	double m_scaleX;  //Scale in X direction 
	double m_scaleY;  //Scale in Y direction

	UINT g_NumberOfButtons;          //Number of numerated buttons
	BOOL bButtonStates[MAX_BUTTONS]; //Array of numerated buttons conditions
	
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


														// ����������
protected:
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnBnClickedOk();
	void DrawScene(void);
	int CJoystickMFCDlg::ParseRawInput(PRAWINPUT pRawInput);
	afx_msg void OnBnClickedButton1();
};
