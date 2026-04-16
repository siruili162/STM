#pragma once
#include <windows.h>

// 启动守护进程（创建自身副本，带 --guard 参数）
void StartGuardProcess();

// 检查守护进程是否存活（通过互斥体）
bool IsGuardAlive();

// 检查主进程是否存活（通过互斥体）
bool IsMainAlive();

// 守护进程的主函数（由 CreateProcess 调用）
int GuardMain();

// 设置开机自启动（写入注册表）
void SetAutoStartup();
