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

#include "StdAfx.h"
#include ".\yahoodata.h"

#define PRECISION 0.0001

Bar::Bar(const DatasourceBar& bar, bool adjust, bool verify,
         bool volume0invalid, double volumeMultiplier) {
  m_close = bar.close;
  m_open = bar.open;
  m_high = bar.high;
  m_low = bar.low;
  m_volume = bar.volume * volumeMultiplier;

  // format: y-m-d (iso) or Jan 25, 2007 (for Google)

  std::wstring date = bar.date;
  m_date = getDate(date, yloader::iso);

  if (m_date.isNotADate()) {
    m_date = getDate(date, yloader::xyz);
  }

  if (m_date.isNotADate()) {
    m_date = getDate(date, yloader::us);
  }

  m_error = none;

  if (verify) {
    if (m_low == 0) {
      m_error = low_0;
    }

    if (m_high == 0) {
      m_error = high_0;
    }

    if (m_open == 0) {
      m_error = open_0;
    }

    if (m_close == 0) {
      m_error = close_0;
    }

    if (m_volume == 0 && volume0invalid) {
      m_error = volume_0;
    }

    if (m_open > m_high && (m_open - m_high) > PRECISION) {
      m_error = open_greater_than_high;
    }

    if (m_close > m_high && (m_close - m_high) > PRECISION) {
      m_error = close_greater_than_high;
    }

    if (m_low > m_high && (m_low - m_high) > PRECISION) {
      m_error = low_greater_than_high;
    }

    if (m_open < m_low && (m_low - m_open) > PRECISION) {
      m_error = open_lower_than_low;
    }

    if (m_high < m_low && (m_low - m_high) > PRECISION) {
      m_error = high_lower_than_low;
    }

    if (m_close < m_low && (m_low - m_close) > PRECISION) {
      m_error = close_lower_than_low;
    }
  }
}
