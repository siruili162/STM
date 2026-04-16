#pragma once
#include <windows.h>
#include <vector>
#include <string>

// 检查进程是否存在（不区分大小写）
bool IsProcessRunning(const std::wstring& processName);

// 终止所有匹配名称的进程
void KillProcessByName(const std::wstring& processName);

// 从 blacklist.txt 加载黑名单（每行一个进程名）
std::vector<std::wstring> LoadBlacklist();

// 确保黑名单文件存在并包含默认项
void EnsureBlacklistExists();
