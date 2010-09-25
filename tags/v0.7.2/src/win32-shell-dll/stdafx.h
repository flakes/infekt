/**
 * Copyright (C) 2010 cxxjoe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

#pragma once

#define _TARGETVER_VISTA
#include "targetver.h"

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <objbase.h>
#include <thumbcache.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <limits>
#include <map>
#include <io.h>

#include <boost/shared_ptr.hpp>

#include <cairo-win32.h>
#include <pcre.h>
#include <inttypes.h>

#define _tstring wstring
#include "util.h"
#include "theme_api.h"

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
