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

#include "StdAfx.h"
#include ".\yahoodata.h"

#define PRECISION 0.0001

Bar::Bar(const DatasourceBar& bar, bool adjust, bool verify,
         bool volume0invalid, double volumeMultiplier) {
  _close = bar.close;
  _open = bar.open;
  _high = bar.high;
  _low = bar.low;
  _volume = bar.volume * volumeMultiplier;

  // format: y-m-d (iso) or Jan 25, 2007 (for Google)

  std::wstring date = bar.date;
  _date = getDate(date, yloader::iso);

  if (_date.isNotADate()) _date = getDate(date, yloader::xyz);

  if (_date.isNotADate()) _date = getDate(date, yloader::us);

  _error = none;

  if (verify) {
    if (_low == 0) _error = low_0;

    if (_high == 0) _error = high_0;

    if (_open == 0) _error = open_0;

    if (_close == 0) _error = close_0;

    if (_volume == 0 && volume0invalid) _error = volume_0;

    if (_open > _high && (_open - _high) > PRECISION)
      _error = open_greater_than_high;

    if (_close > _high && (_close - _high) > PRECISION)
      _error = close_greater_than_high;

    if (_low > _high && (_low - _high) > PRECISION)
      _error = low_greater_than_high;

    if (_open < _low && (_low - _open) > PRECISION)
      _error = open_lower_than_low;

    if (_high < _low && (_low - _high) > PRECISION)
      _error = high_lower_than_low;

    if (_close < _low && (_low - _close) > PRECISION)
      _error = close_lower_than_low;
  }
}
