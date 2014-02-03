//
/* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details. */
//
#include "stdafx.h"
#include "Installer_EFI_gui.h"
using namespace std;
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'\
""version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define MAX_LOADSTRING 100
#define MAX_SLPSTRING 32
//
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hWndButton1;
HWND hWndButton2;
HWND hWndCombobox;
HWND hWndMain;
HCURSOR hCursor;
BOOL bBusy = FALSE;
//
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	//HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINDSLICEFIINSTALLER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if(msg.hwnd == hWndButton1) {
			switch (msg.message) {
			case WM_LBUTTONUP:
				ThreadStart();
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Install, 0, 0, 0);
				break;
			}
		}
		if(msg.hwnd == hWndButton2) {
			switch (msg.message) {
			case WM_LBUTTONUP:
				ThreadStart();
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Uninstall, 0, 0, 0);
				break;
			}
		}
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LARGE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	hInst = hInstance; // Store instance handle in our global variable

	hWndMain = CreateWindow(szWindowClass, szTitle, WS_BORDER|WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 145, NULL, NULL, hInstance, NULL);

	if (!hWndMain)
	{
		return FALSE;
	}
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CLOSE:
		if (bBusy)
		{
			return 0;
		}
		PostQuitMessage(0);
		break;
	case WM_SETCURSOR:
		if(bBusy) {
			return TRUE;
		}
		else {
			hCursor = LoadCursor(NULL, IDC_ARROW);
			if (hCursor) {
				SetCursor(hCursor);
			}
			return TRUE;
		}
	case WM_CREATE:
		{
			hWndButton1 = CreateWindow(L"button", L"Install",    
				WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				10, 80, 80, 25,        
				hWnd, (HMENU) 1, NULL, NULL);
			//
			hWndButton2 = CreateWindow(L"button", L"Uninstall",    
				WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				104, 80, 80, 25,        
				hWnd, (HMENU) 2, NULL, NULL);
			//
			hWndCombobox = CreateWindow(L"combobox", L"SLIC",
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT,
				10, 20, 174, 25,
				hWnd, (HMENU) 3, NULL, NULL);
			//
			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage( hWndMain, WM_SETFONT, (WPARAM)hFont, TRUE );
			SendMessage( hWndButton1, WM_SETFONT, (WPARAM)hFont, TRUE );
			SendMessage( hWndButton2, WM_SETFONT, (WPARAM)hFont, TRUE );
			SendMessage( hWndCombobox, WM_SETFONT, (WPARAM)hFont, TRUE );
			PopulateCombobox(hWndCombobox);
			if (!isEfi() || !Test() || !InitLib(TRUE)) {
				EnableWindow(hWndButton1, FALSE);
				EnableWindow(hWndButton2, FALSE);
				EnableWindow(hWndCombobox, FALSE);
			}
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//
int
	Install (VOID)
{
	ofstream outfile;
	int ret = 1;
	UCHAR Key[16] = {};
	wchar_t EntryName[24] = {};
	wchar_t Description[] = L"WindSLIC\0";
	wchar_t Injector[] = L"Injector\0";
	wchar_t BootFile[] = L"\\BOOTX64.EFI\0";
	wchar_t ResourceType[] = L"RAW\0";
	ULONG BootEntryLength = 0;
	HRSRC hResource;
	HGLOBAL hgResource;
	size_t WindSlicSize;
	size_t SlicSize;
	ULONG EntryId;
	CHAR* SLPString;
	UINT32 SLPLength = 0;
	UINT32 ScanBegin = 0;
	UINT32 ScanEnd = 0;
	UCHAR* WindSlicBuffer;
	UCHAR* SlicBuffer;
	UCHAR* BootEntry;
	LPTSTR ResourceName;
	wchar_t* UId = NULL;
	wchar_t* DosDevice = NULL;
	wchar_t* Directory = NULL;
	wchar_t* OldDirectory = NULL;
	wchar_t* FilePath = NULL;
	wchar_t* BootEntryPath = NULL;
	//
	try {
		DosDevice = new wchar_t[8];
		Directory = new wchar_t[MAX_PATH];
		OldDirectory = new wchar_t[MAX_PATH];
		FilePath = new wchar_t[MAX_PATH];
		BootEntryPath = new wchar_t[MAX_PATH];
		UId = new wchar_t[sizeof(GUID) * 2 + 4]();
		if(!InitLib(TRUE)) {
			throw bad_alloc();
		}
		GetFreeLetter (DosDevice);
		if(UniqueId(&UId) == ERROR_OUTOFMEMORY) {
			throw bad_alloc();
		}
		wsprintf(Directory, L"%s\\EFI\\%s\0", DosDevice, UId);
		wsprintf(OldDirectory, L"%s\\EFI\\WINDSLIC\0", DosDevice);
		wsprintf(FilePath, L"%s%s\0", Directory, BootFile);
		wsprintf(BootEntryPath, L"\\EFI\\%s%s\0", UId, BootFile);
		// get modify firmware variable privilage.
		AcquirePrivilage();
		// install WindSLIC to ESP.
		MountEsp(DosDevice);
		DeleteRecursive(OldDirectory);
		while (DeleteRecursive(Directory) != 0) {
			Sleep(100);
		}
		if (CreateDirectory(Directory, NULL)) {
			// get WindSLIC resource.
			hResource = FindResource(NULL, MAKEINTRESOURCE(130), ResourceType);
			WindSlicSize = SizeofResource(NULL, hResource);
			hgResource = LoadResource(NULL, hResource);
			WindSlicBuffer = new UCHAR[WindSlicSize];
			memcpy(WindSlicBuffer, LockResource(hgResource), WindSlicSize);
			// get SLIC resource.
			switch (SendMessage(hWndCombobox, CB_GETCURSEL, 0, 0)) {
			case 0:
				ResourceName = MAKEINTRESOURCE(131);
				SLPString = "AcerSystem\0";
				ScanBegin = 0x0F0000;
				ScanEnd = 0x0FFFFF;
				break;
			case 1:
				ResourceName = MAKEINTRESOURCE(132);
				SLPString = "alienware\0";
				ScanBegin = 0x0F0000;
				ScanEnd = 0x0FFFFF;
				break;
			case 2:
				ResourceName = MAKEINTRESOURCE(133);
				SLPString = "ASUS_FLASH\0";
				ScanBegin = 0x0FC000;
				ScanEnd = 0x0FFFFF;
				break;
			case 3:
				ResourceName = MAKEINTRESOURCE(134);
				SLPString = "GIGABYTE\0";
				ScanBegin = 0x0F0000;
				ScanEnd = 0x0FFFFF;
				break;
			}
			hResource = FindResource(NULL, ResourceName, L"RAW");
			SLPLength = strlen(SLPString);
			SlicSize = SizeofResource(NULL, hResource);
			SlicBuffer = new UCHAR[SlicSize];
			hgResource = LoadResource(NULL, hResource);
			memcpy(SlicBuffer,LockResource(hgResource), SlicSize);
			// generate encryption key.
			srand((UINT)time(0));
			for (UINT i = 0; i < sizeof(Key); i++) {
				Key[i] = (UCHAR)(rand() % 0x5D) + 0x21;
			}
			// encrypt SLIC.
			UINT pos = sizeof(Key)-1;
			for(UINT i = 0; i < SlicSize; i++) {
				SlicBuffer[i] ^= Key[pos];
				pos--;
				if (pos == -1) {
					pos = sizeof(Key)-1;
				}
			}
			// encrypt key.
			for (UINT i = 0; i < sizeof(Key); i++) {
				Key[i] ^= 0xFF;
			}
			// copy encrypted data.
			for (UINT i = 0; i < WindSlicSize - 6; i++) {
				BOOL slic = FALSE;
				BOOL key = FALSE;
				BOOL slp = FALSE;
				BOOL slplen = FALSE;
				BOOL begin = FALSE;
				BOOL end = FALSE;
				//
				if (WindSlicBuffer[i] == 0x53 && WindSlicBuffer[i+1] == 0x4C
					&& WindSlicBuffer[i+2] == 0x50 && WindSlicBuffer[i+3] == 0x53
					&& WindSlicBuffer[i+4] == 0x54 && WindSlicBuffer[i+5] == 0x52) {
						memset(&WindSlicBuffer[i], 0, MAX_SLPSTRING);
						memcpy(&WindSlicBuffer[i], SLPString, SLPLength);
						slp = TRUE;
				}
				if (WindSlicBuffer[i] == 0x4B && WindSlicBuffer[i+1] == 0x45
					&& WindSlicBuffer[i+2] == 0x59 && WindSlicBuffer[i+3] == 0x4B
					&& WindSlicBuffer[i+4] == 0x45 && WindSlicBuffer[i+5] == 0x59) {
						memcpy(&WindSlicBuffer[i], Key, sizeof(Key));
						key = TRUE;
				}
				if (WindSlicBuffer[i] == 0x53 && WindSlicBuffer[i+1] == 0x4C
					&& WindSlicBuffer[i+2] == 0x49 && WindSlicBuffer[i+3] == 0x43
					&& WindSlicBuffer[i+4] == 0x76 && WindSlicBuffer[i+5] == 0x01) {
						memcpy(&WindSlicBuffer[i], SlicBuffer, SlicSize);
						slic = TRUE;
				}
				if (WindSlicBuffer[i] == 0x41 && WindSlicBuffer[i+1] == 0x41
					&& WindSlicBuffer[i+2] == 0x41 && WindSlicBuffer[i+3] == 0x41) {
						memcpy(&WindSlicBuffer[i], &SLPLength, sizeof(UINT32));
						slplen = TRUE;
				}
				if (WindSlicBuffer[i] == 0x42 && WindSlicBuffer[i+1] == 0x42
					&& WindSlicBuffer[i+2] == 0x42 && WindSlicBuffer[i+3] == 0x42) {
						memcpy(&WindSlicBuffer[i], &ScanBegin, sizeof(UINT32));
						begin = TRUE;
				}
				if (WindSlicBuffer[i] == 0x43 && WindSlicBuffer[i+1] == 0x43
					&& WindSlicBuffer[i+2] == 0x43 && WindSlicBuffer[i+3] == 0x43) {
						memcpy(&WindSlicBuffer[i], &ScanEnd, sizeof(UINT32)); 
						end = TRUE;
				}
				if (key && slic && slp && end && begin && slplen) {
					break;
				}
			}
			outfile.open(FilePath, ios_base::binary);
			if (outfile.is_open()) {
				outfile.write((char*) WindSlicBuffer, WindSlicSize);
				outfile.close();
			}
			delete[] WindSlicBuffer;
			delete[] SlicBuffer;
		}
		else {
			EfiDeleteDescription(Description);
			EfiDeleteDescription(Injector);
			EfiBootmgrAddFirst();
			goto exit;
		}
		// remove any WindSLIC boot entries.
		EfiDeleteDescription(Description);
		EfiDeleteDescription(Injector);
		EfiBootmgrAddFirst();
		// add WindSLIC boot entry.
		EntryId = EfiFreeBootEntry();
		if (EntryId == -1) {
		}
		else {
			wsprintf(EntryName, L"Boot%04d", EntryId);
			if(EfiCreateBootEntry(NULL, &BootEntryLength, Description, BootEntryPath) == ERROR_OUTOFMEMORY) {
				throw bad_alloc();
			}
			BootEntry = new UCHAR[BootEntryLength];
			switch(EfiCreateBootEntry(BootEntry, &BootEntryLength, Description, BootEntryPath)) {
			case ERROR_SUCCESS:
				SetFirmwareEnvironmentVariable(EntryName, EfiGuid, BootEntry, BootEntryLength);
				EfiBootOrderAddFirst(EntryId);
				break;
			case ERROR_OUTOFMEMORY:
				throw bad_alloc();
				break;
			}
			ret = ERROR_SUCCESS;
			delete[] BootEntry;
		}
exit:
		UnmountEsp(DosDevice);
		delete[] UId;
		delete[] BootEntryPath;
		delete[] FilePath;
		delete[] Directory;
		delete[] OldDirectory;
		delete[] DosDevice;
		ThreadFinished();
		InitLib(FALSE);
		return ret;
	}
	catch (bad_alloc) {
		if(NULL != UId) {delete[] UId;}
		if(NULL != BootEntryPath) {delete[] BootEntryPath;}
		if(NULL != FilePath) {delete[] FilePath;}
		if(NULL != Directory) {delete[] Directory;}
		if(NULL != OldDirectory) {delete[] OldDirectory;}
		if(NULL != DosDevice) {delete[] DosDevice;}
		ThreadFinished();
		InitLib(FALSE);
		return ERROR_OUTOFMEMORY;
	}
}
//
int
	Uninstall (VOID)
{
	wchar_t* UId = NULL;
	wchar_t* DosDevice = NULL;
	wchar_t* Directory = NULL;
	wchar_t* OldDirectory = NULL;
	wchar_t Description[] = L"WindSLIC\0";
	wchar_t Injector[] = L"Injector\0";
	try {
		DosDevice = new wchar_t[8];
		Directory = new wchar_t[MAX_PATH];
		OldDirectory = new wchar_t[MAX_PATH];
		UId = new wchar_t[sizeof(GUID) * 2 + 4]();
		if(!InitLib(TRUE)) {
			throw bad_alloc();
		}
		GetFreeLetter (DosDevice);
		if(UniqueId(&UId) == ERROR_OUTOFMEMORY) {
			throw bad_alloc();
		}
		wsprintf(Directory, L"%s\\EFI\\%s\0", DosDevice, UId);
		wsprintf(OldDirectory, L"%s\\EFI\\WINDSLIC\0", DosDevice);
		MountEsp(DosDevice);
		DeleteRecursive(OldDirectory);
		while (DeleteRecursive(Directory) != 0) {
			Sleep(100);
		}
		AcquirePrivilage();
		EfiDeleteDescription(Description);
		EfiDeleteDescription(Injector);
		EfiBootmgrAddFirst();
		UnmountEsp(DosDevice);
		delete[] UId;
		delete[] Directory;
		delete[] OldDirectory;
		delete[] DosDevice;
		ThreadFinished();
		InitLib(FALSE);
		return ERROR_SUCCESS;
	}
	catch (bad_alloc) {
		if(NULL != UId) {delete[] UId;}
		if(NULL != Directory) {delete[] Directory;}
		if(NULL != OldDirectory) {delete[] OldDirectory;}
		if(NULL != DosDevice) {delete[] DosDevice;}
		ThreadFinished();
		InitLib(FALSE);
		return ERROR_OUTOFMEMORY;
	}
}
//
VOID
	PopulateCombobox(HWND hComboBox)
{
	wchar_t* pSlic[] = {L"Acer", L"Alienware", L"Asus", L"Gigabyte"};
	for (UINT i = 0; i < 4; i++) {
		SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)pSlic[i]);
	}
	SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
}
//
VOID
	BumpMouse()
{
	POINT point;
	GetCursorPos(&point);
	SetCursorPos(point.x, point.y);
}
//
VOID
	ThreadFinished(VOID)
{
	bBusy = FALSE;
	EnableWindow(hWndButton1, TRUE);
	EnableWindow(hWndButton2, TRUE);
	EnableWindow(hWndCombobox, TRUE);
	BumpMouse();
}
//
VOID
	ThreadStart(VOID)
{
	bBusy = TRUE;
	hCursor = LoadCursor(NULL, IDC_WAIT);
	if (hCursor) {
		SetCursor(hCursor);
	}
	EnableWindow(hWndButton1, FALSE);
	EnableWindow(hWndButton2, FALSE);
	EnableWindow(hWndCombobox, FALSE);
}
//