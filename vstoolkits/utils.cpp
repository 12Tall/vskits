#include "pch.h"
#include "utils.h"
#include <windows.h>
#include <tlhelp32.h>

struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}
BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle))
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}

HWND find_main_window(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.window_handle;
}


// 取其他进程模块句柄
HMODULE GetRemoteModuleHandle(DWORD pid, LPCTSTR moduleName){
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	MODULEENTRY32 mod32;
	mod32.dwSize = sizeof(mod32);

	BOOL hasNext = Module32First(snapshot, &mod32);
	HMODULE handle = NULL;
	while (hasNext)
	{
		if (StrCmp(mod32.szModule, moduleName) == 0)
		{
			handle = mod32.hModule;
			break;
		}
		hasNext = Module32Next(snapshot, &mod32);
	}

	CloseHandle(snapshot);
	return handle;
}




void GetAppList(CComboBox* processList){
	// 0. 清空下拉菜单选项
	processList->ResetContent();

	// 1. 创建进程快照
	HANDLE hPS = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hPS) {
		MessageBox(NULL, L"无法获取进程快照",L"程序错误", MB_OK);
		return;
	}

	// 2. 创建用于存储进程信息的结构
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	int i = 0;  // 下拉框的索引

	if (!Process32First(hPS, &pe32)) {
		MessageBox(NULL, L"找不到可用进程", L"程序错误", MB_OK);
		CloseHandle(hPS);
		return;
	}
		
	do{
		if (find_main_window(pe32.th32ProcessID)>0) {
			processList->AddString(pe32.szExeFile);
			processList->SetItemData(i++, pe32.th32ProcessID);
		}
	} while (Process32Next(hPS, &pe32));
	CloseHandle(hPS);
	processList->SetCurSel(0);
	return;

}

void LoadLibraryOrNot(CComboBox* cmx, CEdit* editor, BOOL unload)
{
	HANDLE hProcess = NULL, hThread = NULL;
	WCHAR dllName[MAX_PATH];
	DWORD lenDllName = MAX_PATH;
	DWORD index = cmx->GetCurSel();
	DWORD pid = cmx->GetItemData(index);
	editor->GetWindowText(dllName, lenDllName);
	lenDllName = (lstrlen(dllName) + 1) * sizeof(WCHAR);


	// 打开目标进程
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) {
		OutputDebugString(TEXT("未找到目标进程!\r\n"));
		exit(-1);
	}

	// 在目标进程开辟内存，并返回指向该内存的地址
	LPVOID addrRemoteString = VirtualAllocEx(hProcess, NULL, lenDllName, MEM_COMMIT, PAGE_READWRITE);
	if (addrRemoteString == NULL)
	{
		OutputDebugString(TEXT("远程开辟内存失败\r\n"));
		CloseHandle(hProcess);
		exit(-1);
	}

	BOOL res = WriteProcessMemory(hProcess, addrRemoteString, dllName, lenDllName, NULL);
	if (!res)
	{
		OutputDebugString(TEXT("远程写入内存失败\r\n"));
		CloseHandle(hProcess);
		exit(-1);
	}
	if (unload) {
		WCHAR ch = '\\';
		WCHAR* index = StrRChrI(dllName,NULL, ch)+1;
		HANDLE mod = GetRemoteModuleHandle(pid, index);
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, mod, 0, NULL);
	}
	else {
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, addrRemoteString, 0, NULL);
	}
	if (!hThread) {
		OutputDebugString(TEXT("无法获取LoadLibrary 模块地址\r\n"));
		CloseHandle(hProcess);
		exit(-1);
	}
	WaitForSingleObject(hThread, INFINITE);
	DWORD exitCode;
	GetExitCodeThread(hThread, &exitCode);

	CloseHandle(hThread);
	VirtualFreeEx(hProcess, addrRemoteString, 0, MEM_RELEASE);
	CloseHandle(hProcess);
}


