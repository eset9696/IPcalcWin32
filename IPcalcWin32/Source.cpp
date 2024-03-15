#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetInfo(HWND hwnd);
VOID GetIPBytes(DWORD dwIPaddress, CHAR sz_IPaddress[]);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC) DlgProc, 0);
}



BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IP_ADDRESS);
		SetFocus(hIPaddress);
		HWND hSpinPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hSpinPrefix, UDM_SETRANGE, 0, MAKELPARAM(32, 0));
	}
		break;
	case WM_NOTIFY:
	{
		/*HWND hIPaddress = GetDlgItem(hwnd, IDC_IP_ADDRESS);
		HWND hStaticInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
		if ((HWND)wParam == hIPaddress)
		{
			DWORD dwIPaddress = 0;
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			CHAR info[256] = {};
			sprintf(info, "%i", dwIPaddress);
			SendMessage(hStaticInfo, WM_SETTEXT, 0, (LPARAM)info);
		}*/
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IP_ADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IP_MASK);
		HWND hStaticInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		switch (wParam)
		{
		case IDC_IP_ADDRESS:
		{
			DWORD dwIPaddress = 0;
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			DWORD first = FIRST_IPADDRESS(dwIPaddress);
			DWORD dwMask = 0;
			if (first > 0 && first < 128) dwMask = 0xFF000000;
			else if (first >= 128 && first < 192) dwMask = 0xFFFF0000;
			else if (first >= 192 && first < 224) dwMask = 0xFFFFFF00;
			SendMessage(hIPmask, IPM_SETADDRESS, 0, (LPARAM)dwMask);
			CHAR info[256] = {};
			sprintf(info, "%i", first);
			SendMessage(hStaticInfo, WM_SETTEXT, 0, (LPARAM)info);
		}
		case IDC_IP_MASK:
		{
			DWORD dwIPmask = 0;
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			if (dwIPmask == 0) break;
			int i = 32;
			for (; dwIPmask & 0x01 ^ 1; i--) dwIPmask>>=1;
			CHAR prefix[5];
			sprintf(prefix, "%i", i);
			SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)prefix);
			GetInfo(hwnd);
		}
		break;
		}

	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_PREFIX:
		{
			if (HIWORD(wParam) == EN_UPDATE)
			{
				HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
				CONST INT SIZE = 5;
				CHAR sz_prefix[SIZE] = {};
				SendMessage(hEditPrefix, WM_GETTEXT, SIZE, (LPARAM)&sz_prefix);
				DWORD dwPrefix = atoi(sz_prefix);
				DWORD dwIPmask = UINT_MAX;
				dwIPmask >>= (32 - dwPrefix);
				dwIPmask <<= (32 - dwPrefix);
				SendMessage(GetDlgItem(hwnd, IDC_IP_MASK), IPM_SETADDRESS, 0, (LPARAM)dwIPmask);
			}
			break;
		}
		case IDOK:
		{	
			GetInfo(hwnd);
			break;
		}
		case IDCANCEL: 
			EndDialog(hwnd, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}

VOID GetInfo(HWND hwnd)
{
	HWND hIPaddress = GetDlgItem(hwnd, IDC_IP_ADDRESS);
	HWND hIPmask = GetDlgItem(hwnd, IDC_IP_MASK);
	HWND hStaticInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
	DWORD dwIPaddress = 0;
	DWORD dwIPmask = 0;
	SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
	SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);

	CONST INT SIZE = 256;
	CHAR sz_AllInfo[SIZE] = {};
	CHAR sz_Info[SIZE] = {};

	DWORD dwNetIPaddress = dwIPaddress & dwIPmask;
	CHAR sz_NetworkAddress[16] = {};
	GetIPBytes(dwNetIPaddress, sz_NetworkAddress);

	sprintf(sz_Info, "Network address:\t%s\n", sz_NetworkAddress);
	strcat(sz_AllInfo, sz_Info);

	DWORD dwBroadcastAddress = dwIPaddress | ~dwIPmask;
	CHAR sz_BroadcastAddress[16] = {};
	GetIPBytes(dwBroadcastAddress, sz_BroadcastAddress);
	sprintf(sz_Info, "Broadcast address:\t%s\n", sz_BroadcastAddress);
	strcat(sz_AllInfo, sz_Info);

	SendMessage(hStaticInfo, WM_SETTEXT, 0, (LPARAM)sz_AllInfo);
}

VOID GetIPBytes(DWORD dwIPaddress, CHAR sz_IPaddress[])
{
	UCHAR bytes[5] = {};
	for(int i = 0; i < 4; i++)
	{
		bytes[i] = dwIPaddress & 0xFF;
		dwIPaddress >>= 8;
	}
	for(int i = 4 - 1; i >=0; i--)
	{
		CHAR byte[4];
		sprintf(byte, "%i", bytes[i]);
		strcat(sz_IPaddress, byte);
		if(i != 0)strcat(sz_IPaddress, ".");
	}
}