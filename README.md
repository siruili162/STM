# STM - Security Threat Monitor

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows-blue)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)](https://isocpp.org/)

**STM** (Security Threat Monitor) is a lightweight Windows background utility that protects your system by monitoring and controlling unauthorized processes. When a blacklisted process is detected, STM prompts for a password; if the password is incorrect or times out, the process is immediately terminated. After successful authentication, the process is allowed to run for a configurable period. STM also features auto-startup and a dual-process self-protection mechanism.

---

## 🇨🇳 中文文档

### 功能特性

- 🔍 **进程黑名单** – 通过 `blacklist.txt` 管理需要监控的进程名（不区分大小写）
- 🔐 **密码保护** – 检测到黑名单进程时弹出密码输入窗口，默认密码 `123456`（可在源码修改）
- ⏱️ **超时杀进程** – 10 秒内未输入正确密码，自动终止违规进程
- ⏳ **临时放行** – 密码正确后，放行指定时长（默认 60 分钟），期间不再提示
- 🚀 **开机自启** – 自动写入注册表，系统启动后静默运行
- 🛡️ **双进程守护** – 主进程与守护进程互相监视，任一被结束都会立即重启，防止被任务管理器轻易终止
- 🖥️ **无窗口运行** – 纯后台运行，无控制台窗口，仅弹出密码对话框

### 快速开始

#### 下载与运行

1. 从 [Releases](https://github.com/siruili162/STM/releases) 下载最新 `STM.exe`
2. 将 `STM.exe` 放置到任意目录（建议 `C:\Program Files\STM\`）
3. 首次运行会在同目录生成 `blacklist.txt`，默认包含 `Plain Craft Launcher 2.exe`
4. 双击运行，点击“确定”后程序进入后台

#### 自定义黑名单

编辑 `blacklist.txt`，每行一个进程名（包含 `.exe` 后缀），例如：
notepad.exe
taskmgr.exe
malware.exe

#### 修改密码与放行时间

打开 `Config.h`，修改以下常量后重新编译：
```cpp
const std::wstring PASSWORD = L"your_password";
const int ALLOW_MINUTES = 120;   // 放行 120 分钟
const int PASSWORD_TIMEOUT_SEC = 15;  // 倒计时 15 秒
