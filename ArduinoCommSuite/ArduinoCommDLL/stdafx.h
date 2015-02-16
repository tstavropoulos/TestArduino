// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WINDOWS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#endif // _WINDOWS

#ifdef _UNIX
    #include <sys/time.h>
    #include <queue>
    #include <fcntl.h>
    #include <cstdio>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <limits.h>
    #include <unistd.h>
#endif // _UNIX

#include <cstring>
#include <math.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <thread>
#include <memory>
