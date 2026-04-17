#include "ProcessUtils.h"
#include <tlhelp32.h>
#include <fstream>
#include <codecvt>
#include <locale>

bool IsProcessRunning(const std::wstring& processName) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) return false;
	
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(pe);
	bool found = false;
	
	if (Process32FirstW(snapshot, &pe)) {
		do {
			if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
				found = true;
				break;
			}
		} while (Process32NextW(snapshot, &pe));
	}
	CloseHandle(snapshot);
	return found;
}

void KillProcessByName(const std::wstring& processName) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) return;
	
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(pe);
	if (Process32FirstW(snapshot, &pe)) {
		do {
			if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
				if (hProcess) {
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
				}
			}
		} while (Process32NextW(snapshot, &pe));
	}
	CloseHandle(snapshot);
}

std::vector<std::wstring> LoadBlacklist() {
	std::vector<std::wstring> list;
	std::ifstream file("blacklist.txt");
	if (!file.is_open()) return list;
	
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		// 去除末尾回车符
		if (line.back() == '\r') line.pop_back();
		std::wstring wline(line.begin(), line.end());
		list.push_back(wline);
	}
	return list;
}

void EnsureBlacklistExists() {
	std::ifstream file("blacklist.txt");
	if (!file.good()) {
		std::ofstream out("blacklist.txt");
		out << "example.exe" << std::endl;
		out.close();
	}
}
