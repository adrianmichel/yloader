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

#include <datetime.h>
#include <crypt.h>
#include "timestamp.h"

#define TIMESTAMP(n) TIMESTAMP##n
#define TIMESTAMP_PW(n) TIMESTAMP_PW##n

inline yloader::Date ds(const char* password, const char* timestamp) {
  yloader::uint32_t k[4] = {0, 0, 0, 0};

  for (unsigned int n = 0; n < 4; ++n) {
    for (unsigned int m = 0; m < 8; ++m) {
      char c(password[n + m * 4]);

      k[n] += (c >= '0' && c <= '9')
                  ? c - '0'
                  : ((c >= 'a' || c <= 'f') ? c - 'a' : c - 'A');
    }
  }

  yloader::Crypt crypt(k, true, true);

  return yloader::Date(yloader::s2ws(*crypt.de(timestamp)));
}