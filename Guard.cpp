#include "Guard.h"
#include "Config.h"
#include "ProcessUtils.h"
#include <shellapi.h>

void StartGuardProcess() {
	// 防止重复启动
	if (IsGuardAlive()) return;
	
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	
	// 命令行参数：--guard
	wchar_t cmdLine[MAX_PATH + 10];
	swprintf_s(cmdLine, L"\"%s\" --guard", path);
	
	STARTUPINFOW si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;  // 隐藏守护进程窗口
	PROCESS_INFORMATION pi;
	
	CreateProcessW(path, cmdLine, NULL, NULL, FALSE,
				   CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

bool IsGuardAlive() {
	HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_GUARD);
	if (hMutex) {
		CloseHandle(hMutex);
		return true;
	}
	return false;
}

bool IsMainAlive() {
	HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_MAIN);
	if (hMutex) {
		CloseHandle(hMutex);
		return true;
	}
	return false;
}

void SetAutoStartup() {
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_CURRENT_USER,
					  L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
					  0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
		
		wchar_t path[MAX_PATH];
		GetModuleFileNameW(NULL, path, MAX_PATH);
		RegSetValueExW(hKey, L"STM", 0, REG_SZ,
					   (BYTE*)path, (wcslen(path) + 1) * sizeof(wchar_t));
		RegCloseKey(hKey);
	}
}

int GuardMain() {
	// 创建互斥体保证只有一个守护实例
	HANDLE hMutex = CreateMutexW(NULL, TRUE, MUTEX_GUARD);
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
	
	// 守护循环
	while (true) {
		if (!IsMainAlive()) {
			// 重启主进程
			wchar_t path[MAX_PATH];
			GetModuleFileNameW(NULL, path, MAX_PATH);
			STARTUPINFOW si = { sizeof(si) };
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION pi;
			CreateProcessW(path, NULL, NULL, NULL, FALSE,
						   CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		Sleep(GUARD_CHECK_INTERVAL_MS);
	}
	return 0;
}
