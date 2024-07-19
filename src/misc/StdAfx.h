/*
Copyright(C) 2020  YLoader.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501 // XP
#endif

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <process.h>

#pragma warning (disable:4800 4275 4251 4244 4003)
#pragma warning (push)

#define BOOST_TIMER_ENABLE_DEPRECATED

#include <boost\date_time\posix_time\ptime.hpp>
#include <boost\date_time\posix_time\posix_time.hpp>
#include <boost\date_time\time_clock.hpp>
#include <boost\timer.hpp>

//#define BOOST_THREAD_USE_LIB
//#define BOOST_ALL_NO_LIB
#include <boost\thread.hpp>
//#undef BOOST_THREAD_USE_LIB
//#undef BOOST_ALL_NO_LIB
#pragma warning (pop)

#include <tokenizer.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

