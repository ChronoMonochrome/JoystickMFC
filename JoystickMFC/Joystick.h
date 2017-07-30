#include <hidsdi.h>

#define MAX_BUTTONS		128

#pragma once
class Joystick
{
public:
	HANDLE               hHeap;
	PHIDP_PREPARSED_DATA pPreparsedData;
	PHIDP_BUTTON_CAPS    pButtonCaps;
	PHIDP_VALUE_CAPS     pValueCaps;
	USAGE    usage[MAX_BUTTONS];

	PRAWINPUT pRawInput = NULL;
	UINT      bufferSize = 0;

	int m_nPress;
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

	Joystick();
	Joystick(HWND hWnd, CStatic * pWnd);
	~Joystick();
	int  Joystick::SaveFree(BOOL bRslt);
	int Joystick::GetAndParseRawInput(UINT nInputcode, HRAWINPUT lParam);
	int Joystick::ParseRawInput(PRAWINPUT pRawInput);
};

