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

#pragma warning(disable : 4800 4275 4251 4244 4003)

#include <assert.h>
#include <list>
#include <vector>
#include "strings.h"
#include <memory>

namespace yloader {

using StringPtr = std::shared_ptr<std::wstring>;
/** \brief pointer to a list of strings
 *
 */

using StrList = std::list<std::wstring>;
using StrListPtr = std::shared_ptr<StrList>;
using ConstStringPtr = std::shared_ptr<const std::wstring>;
using StringPtrVector = std::vector<StringPtr>;

}  // namespace yloader