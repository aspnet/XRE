// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

#include "xplat.h"
#include "TraceWriter.h"

void GetNativeBootstrapperDirectory(LPTSTR szPath);
void WaitForDebuggerToAttach();
bool IsTracingEnabled();
void SetConsoleHost();
BOOL GetAppBasePathFromEnvironment(LPTSTR szPath);
BOOL GetFullPath(LPCTSTR szPath, LPTSTR szFullPath);
HMODULE LoadNativeHost(LPCTSTR szHostModuleName);
BOOL FreeNativeHost(HMODULE hHost);
FARPROC GetEntryPointFromHost(HMODULE hModule, LPCSTR lpProcName);
int CallApplicationMain(const dnx::char_t* moduleName, const char* functionName, CALL_APPLICATION_MAIN_DATA* data, TraceWriter traceWriter);

#ifndef SetEnvironmentVariable
BOOL SetEnvironmentVariable(LPCTSTR lpName, LPCTSTR lpValue);
#endif //SetEnvironmentVariable
