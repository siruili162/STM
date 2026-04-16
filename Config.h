#pragma once
#include <string>

// 安全配置（修改源码即可更改密码）
const std::wstring PASSWORD = L"139888";        // 访问密码
const int ALLOW_MINUTES = 60;                   // 授权时间（分钟）
const int PASSWORD_TIMEOUT_SEC = 10;            // 密码输入超时（秒）
const int CHECK_INTERVAL_MS = 800;              // 进程检测间隔（毫秒）
const int GUARD_CHECK_INTERVAL_MS = 2000;       // 守护检查间隔（毫秒）

// 互斥体名称
const wchar_t MUTEX_MAIN[] = L"Global\\STM_Main_Instance";
const wchar_t MUTEX_GUARD[] = L"Global\\STM_Guard_Instance";
