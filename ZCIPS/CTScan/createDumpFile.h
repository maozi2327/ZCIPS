#pragma once
#pragma once
#include <windows.h>
#include <imagehlp.h>
#include <cstdlib>
#include <tchar.h>

#pragma comment(lib, "dbghelp.lib")
inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
	if (pModuleName == 0)
	{
		return FALSE;
	}
	WCHAR szFileName[_MAX_FNAME] = L"";
	_wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);
	if (wcsicmp(szFileName, L"ntdll") == 0)
		return TRUE;
	return FALSE;
}

inline BOOL CALLBACK MiniDumpCallback(PVOID pParam,
	const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
	if (pInput == 0 || pOutput == 0)
		return FALSE;
	switch (pInput->CallbackType)
	{
	case ModuleCallback:
		if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
			if (!IsDataSectionNeeded(pInput->Module.FullPath))
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		return TRUE;
	default:;
	}
	return FALSE;
}

//����Dump�ļ�
inline void CreateMiniDump(EXCEPTION_POINTERS* pep, LPCTSTR strFileName)
{
	HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = FALSE;
		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam = 0;
		MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)0x0000ffff;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, NULL, &mci);
		CloseHandle(hFile);
	}
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}
BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL)
		return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");

	if (pOrgEntry == NULL)
		return FALSE;
	unsigned char newJump[100];
	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
	newJump[0] = 0xE9;  // JMP absolute
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
	return bRet;
}

void DisableSetUnhandledExceptionFilter()

{

	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),

		"SetUnhandledExceptionFilter");

	if (addr)

	{

		unsigned char code[16];

		int size = 0;

		code[size++] = 0x33;

		code[size++] = 0xC0;

		code[size++] = 0xC2;

		code[size++] = 0x04;

		code[size++] = 0x00;



		DWORD dwOldFlag, dwTempFlag;

		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);

		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);

		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);

	}

}

LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS *pException)
{
	SYSTEMTIME syt;
	GetLocalTime(&syt);
	TCHAR strDumpFile[MAX_PATH];
	_stprintf_s(strDumpFile, MAX_PATH, 
		_T("[%04d-%02d-%02d %02d��%02d��%02d].dmp"), syt.wYear, syt.wMonth, syt.wDay, syt.wHour, syt.wMinute, syt.wSecond);

	TCHAR szMbsFile[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szMbsFile, MAX_PATH);
	TCHAR* pFind = _tcsrchr(szMbsFile, '\\');

	if (pFind)
	{
		*(pFind + 1) = 0;
		_tcscat(szMbsFile, strDumpFile);
		CreateMiniDump(pException, strDumpFile);
	}
	// TODO_DJ: MiniDumpWriteDump
	FatalAppExit(-1, _T("Fatal Error"));
	return EXCEPTION_CONTINUE_SEARCH;
}
//�����쳣����
void RunCrashHandler()
{
	SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
	//DisableSetUnhandledExceptionFilter();
	//PreventSetUnhandledExceptionFilter();
}