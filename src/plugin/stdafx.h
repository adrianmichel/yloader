/*
Copyright (C) 2020  YLoader.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501  // XP
#endif

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be
                                            // explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>  // MFC core and standard components

// TODO: reference additional headers your program requires here

#include <boost\static_assert.hpp>
#include <boost\type_traits.hpp>

#include <log.h>
#include <exceptions.h>
#include <misc.h>
#include <plugin.h>
#include <strings.h>
//#include <common.h>
