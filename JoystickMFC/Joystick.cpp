#include "stdafx.h"
#include "Joystick.h"
#include <hidsdi.h>


#define SAFE_FREE(p)	{ if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }

Joystick::Joystick()
{
}


Joystick::Joystick(HWND hWnd, CStatic * pWnd)
{
	this->pPreparsedData = NULL;
	this->pButtonCaps = NULL;
	this->pValueCaps = NULL;
	this->hHeap = NULL;
	this->m_x = 0;
	this->m_y = 0;
	this->m_z = 0;
	this->m_zr = 0;

	//
	// Joystick Initialization of Alexander Böcken
	//

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 1;
	rid.usUsage = 4;	// Joystick
	rid.dwFlags = 0;
	rid.hwndTarget = hWnd;
	if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
	{
		throw CString("No Joystick Available.");
	}
	/////////////////////////////////////////////////////////////

	CRect rect;
	pWnd->GetClientRect(&rect);
	this->m_ptXY0 = CPoint(rect.Width() / 2, rect.Height() / 2);
	this->m_scaleX = rect.Width() / 256.;
	this->m_scaleY = rect.Height() / 256.;
	this->m_ptXY = this->m_ptZ = CPoint(-1000, -1000);
}


Joystick::~Joystick()
{
}


int  Joystick::SaveFree(BOOL bRslt)
{
	SAFE_FREE(this->pPreparsedData);
	SAFE_FREE(this->pButtonCaps);
	SAFE_FREE(this->pValueCaps);
	if (!bRslt)
		this->m_nPress = 0;
	return bRslt;
}

int Joystick::GetAndParseRawInput(UINT nInputcode, HRAWINPUT lParam)
{
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &this->bufferSize, sizeof(RAWINPUTHEADER));

	this->hHeap = GetProcessHeap();
	this->pRawInput = (PRAWINPUT)HeapAlloc(this->hHeap, 0, this->bufferSize);
	if (!this->pRawInput)
		return -1;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, this->pRawInput, &this->bufferSize, sizeof(RAWINPUTHEADER));
	this->ParseRawInput(this->pRawInput);

	HeapFree(this->hHeap, 0, this->pRawInput);

	return 0;
}

/////////////////////////////////////////////
int Joystick::ParseRawInput(PRAWINPUT pRawInput)
{
	HIDP_CAPS            Caps;
	USHORT               capsLength;
	ULONG                i, usageLength, value;

	this->hHeap = GetProcessHeap();

	//
	// Get the preparsed data block
	//

	if (GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &this->bufferSize) != 0)
		return this->SaveFree(FALSE);

	if (!(this->pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(this->hHeap, 0, this->bufferSize)))
		return this->SaveFree(FALSE);
	if ((int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, this->pPreparsedData, &this->bufferSize) <0)
		return this->SaveFree(FALSE);

	//
	// Get the joystick's capabilities
	//

	// Button caps
	if (HidP_GetCaps(this->pPreparsedData, &Caps) != HIDP_STATUS_SUCCESS)
		return this->SaveFree(FALSE);

	if (!(pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps)))
		return this->SaveFree(FALSE);

	capsLength = Caps.NumberInputButtonCaps;
	if (HidP_GetButtonCaps(HidP_Input, this->pButtonCaps, &capsLength, this->pPreparsedData) != HIDP_STATUS_SUCCESS)
		return this->SaveFree(FALSE);

	this->g_NumberOfButtons = this->pButtonCaps->Range.UsageMax - this->pButtonCaps->Range.UsageMin + 1;

	// Value caps
	if (!(this->pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps)))
		return this->SaveFree(FALSE);
	capsLength = Caps.NumberInputValueCaps;
	if (HidP_GetValueCaps(HidP_Input, this->pValueCaps, &capsLength, this->pPreparsedData) != HIDP_STATUS_SUCCESS)
		return this->SaveFree(FALSE);

	//
	// Get the pressed buttons
	//

	usageLength = this->g_NumberOfButtons;
	if (
		HidP_GetUsages(
			HidP_Input, this->pButtonCaps->UsagePage, 0, usage, &usageLength, this->pPreparsedData,
			(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
		) != HIDP_STATUS_SUCCESS)
		return this->SaveFree(FALSE);

	ZeroMemory(this->bButtonStates, sizeof(this->bButtonStates));
	for (i = 0; i < usageLength; i++)
		this->bButtonStates[usage[i] - this->pButtonCaps->Range.UsageMin] = TRUE;

	//
	// Get the state of discrete-valued-controls
	//

	for (i = 0; i < Caps.NumberInputValueCaps; i++)
	{
		if (
			HidP_GetUsageValue(
				HidP_Input, this->pValueCaps[i].UsagePage, 0, this->pValueCaps[i].Range.UsageMin, &value, this->pPreparsedData,
				(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
			) != HIDP_STATUS_SUCCESS)
			return this->SaveFree(FALSE);

		switch (this->pValueCaps[i].Range.UsageMin)
		{
		case 0x30:	// X-axis
			this->m_x = (LONG)value - 128;
			break;

		case 0x31:	// Y-axis
			this->m_y = (LONG)value - 128;
			break;

		case 0x32: // Rotate-Z
			this->m_zr = (LONG)value - 128;
			break;

		case 0x35: // Z-axis
			this->m_z = (LONG)value - 128;
			break;

		case 0x39:	// Hat Switch
			this->m_h = value;
			break;
		}
	}

	return this->SaveFree(TRUE);
}
///////////////////////////////////////////////////////////