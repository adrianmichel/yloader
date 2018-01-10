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

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501 // XP
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

#include <strings.h>

#include <exception>

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

#include <iostream>
#include <fstream>

#include <misc.h>
#include <tokenizer.h>
#include <misc.h>
#include <versionno.h>
#include <defaults.h>
