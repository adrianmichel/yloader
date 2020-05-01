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

#include <windows.h>
#include <iostream>
#include <tchar.h>

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
#include <boost/regex.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <condition_variable>

#include <cmdline.h>
#include <yplugin.h>
#include <log.h>
#include <miscfile.h>
#include <yahooeventsink.h>
#include <charactermapping.h>
#include <dataparams.h>
#include <filesymbols.h>
#include <yahoohistdata.h>
#include <downloading.h>
#include <ylog.h>
#include <defaults.h>
#include <yloader.h>
#include <updater.h>
#include <miscwin.h>

// TODO: reference additional headers your program requires here
