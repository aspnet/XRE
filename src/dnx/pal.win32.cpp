// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

#include "stdafx.h"
#include "dnx.h"
#include "xplat.h"
#include "TraceWriter.h"
#include "utils.h"
#include <sstream>

void GetNativeBootstrapperDirectory(LPTSTR szPath)
{
    DWORD dirLength = GetModuleFileName(NULL, szPath, MAX_PATH);
    for (dirLength--; dirLength >= 0 && szPath[dirLength] != _T('\\'); dirLength--);
    szPath[dirLength + 1] = _T('\0');
}

void WaitForDebuggerToAttach()
{
    if (!IsDebuggerPresent())
    {
        ::_tprintf_s(_T("Process Id: %ld\r\n"), GetCurrentProcessId());
        ::_tprintf_s(_T("Waiting for the debugger to attach...\r\n"));

        // Do not use getchar() like in corerun because it doesn't work well with remote sessions
        while (!IsDebuggerPresent())
        {
            Sleep(250);
        }

        ::_tprintf_s(_T("Debugger attached.\r\n"));
    }
}

bool IsTracingEnabled()
{
    TCHAR szTrace[2];
    DWORD nEnvTraceSize = GetEnvironmentVariable(_T("DNX_TRACE"), szTrace, 2);
    bool m_fVerboseTrace = (nEnvTraceSize == 1);
    if (m_fVerboseTrace)
    {
        szTrace[1] = _T('\0');
        return _tcsnicmp(szTrace, _T("1"), 1) == 0;
    }

    return false;
}

void SetConsoleHost()
{
    TCHAR szConsoleHost[2];
    DWORD nEnvConsoleHostSize = GetEnvironmentVariable(_T("DNX_CONSOLE_HOST"), szConsoleHost, 2);
    if (nEnvConsoleHostSize == 0)
    {
        SetEnvironmentVariable(_T("DNX_CONSOLE_HOST"), _T("1"));
    }
}

BOOL GetAppBasePathFromEnvironment(LPTSTR pszAppBase)
{
    DWORD dwAppBase = GetEnvironmentVariable(_T("DNX_APPBASE"), pszAppBase, MAX_PATH);
    return dwAppBase != 0 && dwAppBase < MAX_PATH;
}

BOOL GetFullPath(LPCTSTR szPath, LPTSTR pszNormalizedPath)
{
    DWORD dwFullAppBase = GetFullPathName(szPath, MAX_PATH, pszNormalizedPath, nullptr);
    if (!dwFullAppBase)
    {
        ::_tprintf_s(_T("Failed to get full path of application base: %s\r\n"), szPath);
        return FALSE;
    }
    else if (dwFullAppBase > MAX_PATH)
    {
        ::_tprintf_s(_T("Full path of application base is too long\r\n"));
        return FALSE;
    }

    return TRUE;
}

int CallApplicationMain(const dnx::char_t* moduleName, const char* functionName, CALL_APPLICATION_MAIN_DATA* data, TraceWriter traceWriter)
{
    bool fVerboseTrace = true;

    HMODULE hostModule;
    try
    {
        hostModule = LoadLibraryEx(moduleName, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (!hostModule)
        {
            throw std::runtime_error(std::string("Failed to load: ")
                .append(dnx::utils::to_std_string(moduleName)));
        }

        traceWriter.Write(dnx::xstring(_X("Loaded module: ")).append(moduleName), true);

        auto pfnCallApplicationMain = (FnCallApplicationMain)GetProcAddress(hostModule, functionName);
        if (!pfnCallApplicationMain)
        {
            std::ostringstream oss;
            oss << "Failed to find function " << functionName << "in" << dnx::utils::to_std_string(moduleName);
            throw std::runtime_error(oss.str());
        }

        traceWriter.Write(dnx::xstring(_X("Found export: ")).append(moduleName), true);

        HRESULT hr = pfnCallApplicationMain(data);
        FreeLibrary(hostModule);
        return SUCCEEDED(hr) ? data->exitcode : hr;
    }
    catch (...)
    {
        if (hostModule)
        {
            FreeLibrary(hostModule);
        }

        throw;
    }
}