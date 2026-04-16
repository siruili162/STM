#include <windows.h>
#include <ctime>
#include <vector>
#include <string>
#include "Config.h"
#include "ProcessUtils.h"
#include "Guard.h"

// 全局变量：密码对话框结果（因为消息循环无法直接返回）
static bool g_passwordAccepted = false;

// 密码窗口过程
LRESULT CALLBACK PasswordWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND hEdit, hStatic;
	static int secondsLeft;
	static UINT_PTR timer;
	
	switch (msg) {
	case WM_CREATE:
		secondsLeft = PASSWORD_TIMEOUT_SEC;
		CreateWindowW(L"STATIC", L"请输入密码：", WS_VISIBLE | WS_CHILD,
					  10, 10, 280, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		hEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
							  10, 35, 280, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindowW(L"BUTTON", L"确定", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
					  80, 70, 60, 25, hWnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);
		CreateWindowW(L"BUTTON", L"取消", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
					  170, 70, 60, 25, hWnd, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);
		hStatic = CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD,
								10, 100, 280, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		timer = SetTimer(hWnd, 1, 1000, NULL);
		SetFocus(hEdit);
		return 0;
		
	case WM_TIMER:
		if (--secondsLeft <= 0) {
			KillTimer(hWnd, timer);
			g_passwordAccepted = false;
			DestroyWindow(hWnd);
		} else {
			wchar_t buf[64];
			swprintf_s(buf, L"剩余 %d 秒", secondsLeft);
			SetWindowTextW(hStatic, buf);
		}
		return 0;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			wchar_t input[256];
			GetWindowTextW(hEdit, input, 256);
			if (PASSWORD == input) {
				KillTimer(hWnd, timer);
				g_passwordAccepted = true;   // 密码正确
				DestroyWindow(hWnd);
			} else {
				MessageBoxW(hWnd, L"密码错误", L"错误", MB_ICONERROR);
				SetWindowTextW(hEdit, L"");
				SetFocus(hEdit);
				// 倒计时继续
			}
		} else if (LOWORD(wParam) == IDCANCEL) {
			KillTimer(hWnd, timer);
			g_passwordAccepted = false;
			DestroyWindow(hWnd);
		}
		return 0;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// 显示密码对话框，返回 true 表示密码正确
bool ShowPasswordDialog() {
	g_passwordAccepted = false;
	WNDCLASSW wc = {};
	wc.lpfnWndProc = PasswordWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"PasswordDialogClass";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassW(&wc);
	
	HWND hWnd = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
								L"PasswordDialogClass", L"STM 安全验证",
								WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
								CW_USEDEFAULT, CW_USEDEFAULT, 310, 150,
								NULL, NULL, wc.hInstance, NULL);
	if (!hWnd) return false;
	
	// 模态消息循环
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return g_passwordAccepted;
}

// 主监控循环
void MonitorLoop() {
	auto blacklist = LoadBlacklist();
	static time_t allowUntil = 0;
	static bool isAllowed = false;
	
	while (true) {
		if (!isAllowed || time(nullptr) >= allowUntil) {
			isAllowed = false;
			bool needAuth = false;
			for (auto& proc : blacklist) {
				if (IsProcessRunning(proc)) {
					needAuth = true;
					break;
				}
			}
			if (needAuth) {
				if (ShowPasswordDialog()) {
					isAllowed = true;
					allowUntil = time(nullptr) + ALLOW_MINUTES * 60;
				} else {
					// 密码错误/超时/取消 -> 杀死所有黑名单进程
					for (auto& proc : blacklist) {
						if (IsProcessRunning(proc))
							KillProcessByName(proc);
					}
				}
			}
		}
		Sleep(CHECK_INTERVAL_MS);
	}
}

// 入口函数（无控制台窗口）
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
	// 释放可能附加的控制台（确保无窗口）
	FreeConsole();
	
	// 防止多实例
	HANDLE hMutex = CreateMutexW(NULL, TRUE, MUTEX_MAIN);
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
	
	// 显示启动提示
	MessageBoxW(NULL, L"STM 安全防护已后台运行", L"STM", MB_OK | MB_ICONINFORMATION);
	
	// 开机自启
	SetAutoStartup();
	
	// 启动守护进程
	StartGuardProcess();
	
	// 确保黑名单文件存在
	EnsureBlacklistExists();
	
	// 启动监控
	MonitorLoop();
	
	ReleaseMutex(hMutex);
	return 0;
}
