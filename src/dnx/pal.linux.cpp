// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

#include "stdafx.h"
#include <assert.h>
#include <dlfcn.h>

LPTSTR GetNativeBootstrapperDirectory()
{
    LPTSTR szPath = (LPTSTR)calloc(PATH_MAX + 1, sizeof(TCHAR));
    ssize_t ret = readlink("/proc/self/exe", szPath, PATH_MAX);

    assert(ret != -1);

    // readlink does not null terminate the path
    szPath[ret] = _T('\0');

    size_t lastSlash = 0;

    for (size_t i = 0; i < ret && szPath[i] != _T('\0'); i++)
    {
        if (szPath[i] == _T('/'))
        {
            lastSlash = i;
        }
    }

    szPath[lastSlash] = _T('\0');

    return szPath;
}

void WaitForDebuggerToAttach()
{
    // TODO: Implement this.  procfs will be able to tell us this.
}
