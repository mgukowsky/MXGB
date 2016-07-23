#pragma once

#include <Windows.h>
#include "../defs.h"

namespace {

//Local variables
const u16 IDM_FILE_OPEN = 1, IDM_FILE_CLOSE = 2, IDM_FILE_EXIT = 3, IDM_HELP_ABOUT = 101;

const char * const wndClassName = "MXGBWND";

const DWORD wsStyles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
						wsExStyles = WS_EX_CLIENTEDGE;

HWND mainHwnd = nullptr;
HMENU hmenuFile = nullptr, hmenuHelp = nullptr, hmenuBar = nullptr;


//The callback for the main window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_COMMAND:

			switch (LOWORD(wParam)) {
				case IDM_FILE_OPEN:
					appCore.pick_file();
					break;

				case IDM_FILE_EXIT:
					SendMessage(hwnd, WM_CLOSE, NULL, NULL);
					break;

				case IDM_HELP_ABOUT:
					MessageBeep(MB_ICONINFORMATION);
					MessageBox(nullptr, "Thanks for using MXGB, a Game Boy emulator by Matt Gukowsky \xA9 2016", "About", MB_ICONQUESTION | MB_OK | MB_TASKMODAL);
					break;
			}
			break;

		case WM_DESTROY:
			appCore.appOptions.shouldRun = false;
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}



const bool create_menu() {
	hmenuFile = CreateMenu();
	hmenuHelp = CreateMenu();
	hmenuBar = CreateMenu();

	if (!hmenuFile || !hmenuHelp || !hmenuBar)
		return false;

	AppendMenu(hmenuFile, MF_STRING, IDM_FILE_OPEN, "&Open ROM...");
	AppendMenu(hmenuFile, MF_STRING | MF_GRAYED, IDM_FILE_CLOSE, "&Close ROM...");
	AppendMenu(hmenuFile, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hmenuFile, MF_STRING, IDM_FILE_EXIT, "&Exit");

	AppendMenu(hmenuHelp, MF_STRING, IDM_HELP_ABOUT, "&About");

	AppendMenu(hmenuBar, MF_POPUP, (UINT_PTR)hmenuFile, "&File");
	AppendMenu(hmenuBar, MF_POPUP, (UINT_PTR)hmenuHelp, "&Help");
	SetMenu(mainHwnd, hmenuBar);

	return true;
}

FORCEINLINE const bool impl_create_window() {

	//Window class boilerplate
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbClsExtra = NULL;
	wcex.cbWndExtra = NULL;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = wndClassName;
	wcex.lpszMenuName = nullptr;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpfnWndProc = WndProc;

	if (!RegisterClassEx(&wcex)) {
		appCore.alert_err("Unable to register window class");
		return false;
	}

	//Make sure the window size is adjusted to compensate for the menu we will add
	RECT rc;
	ZeroMemory(&rc, sizeof(RECT));
	rc.right = DEFAULTS::WINDOW_WIDTH;
	rc.bottom = DEFAULTS::WINDOW_HEIGHT;
	
	if (!AdjustWindowRectEx(&rc, wsStyles, TRUE, wsExStyles))
		appCore.alert_err("Failed to adjust window rect; games may not display properly within the window"); //nonfatal error

	mainHwnd = CreateWindowEx(wsExStyles, wndClassName, appTitle, wsStyles, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right, rc.bottom, nullptr, nullptr, GetModuleHandle(NULL), nullptr);

	if (!mainHwnd) {
		appCore.alert_err("Failed to create main window");
		return false;
	}

	if (!create_menu()) {
		appCore.alert_err("Failed to create window menu bar");
		return false;
	}

	ShowWindow(mainHwnd, SW_SHOW);

	return true;
}

FORCEINLINE void impl_destroy_window() {
	if (mainHwnd)
		DestroyWindow(mainHwnd);

	if (hmenuFile)
		DestroyMenu(hmenuFile);

	if (hmenuHelp)
		DestroyMenu(hmenuHelp);

	if (hmenuBar) {
		DestroyMenu(hmenuBar);
	}
}

FORCEINLINE void impl_msgbox(const char * const msg) {
	MessageBox(nullptr, msg, "Message", MB_OK | MB_ICONINFORMATION);
}

FORCEINLINE void impl_errbox(const char * const msg) {
	MessageBox(nullptr, msg, "ERROR", MB_OK | MB_ICONERROR);
}

FORCEINLINE void impl_sleep(const u32 ms) {
	Sleep(ms);
}

FORCEINLINE void impl_pump_events() {
	static MSG msg;

	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

FORCEINLINE const bool impl_pick_file() {
	OPENFILENAME ofn;
	char buff[BUFSIZ];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ZeroMemory(&buff, BUFSIZ);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = mainHwnd;
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if (!GetOpenFileName(&ofn)) {
		appCore.alert_err("Failed to open file");
		appCore.appOptions.activeFile.clear();
		return false;

	} else {
		appCore.appOptions.activeFile = buff;
		return true;
	}
}

} /* namespace */