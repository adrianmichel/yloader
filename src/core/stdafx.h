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
#define _WIN32_WINNT 0x501 // XP
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include <windows.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
// TODO: reference additional headers your program requires here

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <boost\algorithm\string.hpp>
#include <nlohmann/json.hpp>

#include <misc.h>
#include <strings.h>
#include <datetime.h>
#include <thread.h>
#include <filesymbols.h>


#include <log.h>
#include <hinstance.h>
#include <yloader.h>
#include <exceptions.h>
#include <versionno.h>

#include <ypluginhelper.h>
#include <tokenizer.h>
#include <miscwin.h>

#include <fstream>

//#include <yloadercore.h>

inline std::wstring dateToUSString( yloader::Date date, const std::wstring& defaultDate )
{
	if( date.isNotADate() )
	{
		return defaultDate;
	}
	else
	{
		std::wostringstream o;

		o << date.month() << L"/" << date.day() << L"/" << date.year();
		return o.str();
	}
}

inline std::wstring dateToUSString( yloader::Date date, const yloader::Date& defaultDate )
{
	std::wostringstream def;

	def  << defaultDate.month() << L"/" << defaultDate.day() << L"/" << defaultDate.year();

	return dateToUSString( date, def.str() );
}


