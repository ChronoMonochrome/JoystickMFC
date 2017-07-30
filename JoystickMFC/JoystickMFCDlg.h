// JoystickMFCDlg.h : файл заголовка
//

#pragma once
#include "Joystick.h"

// диалоговое окно CJoystickMFCDlg
class CJoystickMFCDlg : public CDialogEx
{
	// Создание
public:
	CJoystickMFCDlg(CWnd* pParent = NULL);	// стандартный конструктор

											// Данные диалогового окна
  #ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOYSTICKMFC_DIALOG };
	#endif
	
	Joystick joystick;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


														// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
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
