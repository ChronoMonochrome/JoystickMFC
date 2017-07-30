// JoystickMFCDlg.h : ���� ���������
//

#pragma once
#include "Joystick.h"

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
	
	Joystick joystick;

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
	afx_msg void OnBnClickedButton1();
};
