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

#ifndef _IMDB_PLUGIN_H
#define _IMDB_PLUGIN_H

#include "targetver.h"
#include <windows.h>
#include "infekt-plugin.h"

#define MYGUID "{157BDD72-24D2-4d7c-94F2-FC463069A74B}"

long PluginSend(infektPluginCallId lCall, long long lParam, void* pParam);

/* from imdb.cpp: */
INFEKT_PLUGIN_METHOD(ImdbMainEventCallback);

#endif /* !_IMDB_PLUGIN_H */
