/*
Copyright (C) 2017  YLoader.com

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

#include <iostream>
#include <tchar.h>

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>


// TODO: reference additional headers your program requires here
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <fstream>

#include <boost\program_options\cmdline.hpp>
#include <boost/program_options/environment_iterator.hpp> 
#include <boost/program_options/eof_iterator.hpp> 
#include <boost/program_options/errors.hpp> 
#include <boost/program_options/option.hpp> 
#include <boost/program_options/options_description.hpp> 
#include <boost/program_options/parsers.hpp> 
#include <boost/program_options/positional_options.hpp> 
#include <boost/program_options/value_semantic.hpp> 
#include <boost/program_options/variables_map.hpp> 
#include <boost/program_options/version.hpp>

#include <misc.h>
#include <httprequest.h>

#include <filesymbols.h>