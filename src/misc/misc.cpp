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

#include "stdafx.h"
#include <datetime.h>
#include <misc.h>
#include <versionno.h>
#pragma comment(lib, "rpcrt4")
#include <math.h>
#include <clocale>
#include <locale>
#include "stringformat.h"

using namespace yloader;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}

MISC_API std::mutex yloader::m;
MISC_API int idcount = 0;

std::wostream& yloader::tsprint(const std::wstring& str, std::wostream& os) {
  std::scoped_lock lock(m);
  os << str;
  return os;
}

std::wostream& yloader::tsprint(const std::wostringstream& o, std::wostream& os) {
  return tsprint(o.str(), os);
}

class DateImpl;
class DateTimeImpl;

class DateDurationImpl : public DateDurationAbstr {
  friend class DateImpl;
  friend class DateTimeImpl;

 private:
  boost::gregorian::date_duration m_duration;

 public:
  DateDurationImpl(boost::gregorian::date_duration duration)
      : m_duration(duration) {}

  DateDurationImpl(long days) : m_duration(days) {}

  DateDurationImpl() : m_duration(boost::gregorian::date_duration::unit()) {}

  ~DateDurationImpl() override {}

  long days() const override { return m_duration.days(); }
  bool is_negative() const override { return m_duration.is_negative(); }

  bool operator==(const DateDurationAbstr& duration) const override {
    try {
      return m_duration == dynamic_cast<const DateDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator!=(const DateDurationAbstr& duration) const override {
    try {
      return m_duration != dynamic_cast<const DateDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator>(const DateDurationAbstr& duration) const override {
    try {
      return m_duration > dynamic_cast<const DateDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator<(const DateDurationAbstr& duration) const override {
    try {
      return m_duration < dynamic_cast<const DateDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator>=(const DateDurationAbstr& duration) const override {
    try {
      return m_duration >= dynamic_cast<const DateDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator<=(const DateDurationAbstr& duration) const override {
    try {
      return m_duration <= dynamic_cast<const DateDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateDurationAbstrPtr operator+(const DateDurationAbstr& duration) const override {
    try {
      return std::make_shared< DateDurationImpl>(m_duration + dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateDurationAbstrPtr operator-(const DateDurationAbstr& duration) const override {
    try {
      return std::make_shared< DateDurationImpl >(m_duration - dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  DateDurationAbstrPtr operator/(int divisor) override {
    try {
      return std::make_shared< DateDurationImpl>(m_duration / divisor);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateDurationAbstrPtr operator-=(const DateDurationAbstr& duration) override {
    try {
      return std::make_shared< DateDurationImpl >(m_duration -= dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  DateDurationAbstrPtr operator+=(const DateDurationAbstr& duration) override {
    try {
      return std::make_shared< DateDurationImpl >(m_duration += dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  DateDurationAbstrPtr operator/=(int factor) override {
    try {
      return std::make_shared< DateDurationImpl >(m_duration /= factor);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
};

class DateImpl : public DateAbstr {
  friend class DateTimeImpl;

 private:
  boost::gregorian::date m_date;

 public:
  DateImpl(unsigned int year, unsigned int month, unsigned int day)
      : m_date(year, month, day) {}

  DateImpl(const boost::gregorian::date date) : m_date(date) {}

  DateImpl() {}

  std::wstring to_simple_string() const override {
    return boost::gregorian::to_simple_string_type<TCHAR>(m_date);
  }
  std::wstring to_iso_string() const override {
    return boost::gregorian::to_iso_string_type<TCHAR>(m_date);
  }
  std::wstring to_iso_extended_string() const override {
    return boost::gregorian::to_iso_extended_string_type<TCHAR>(m_date);
  }

  unsigned short year() const override { return m_date.year(); }
  unsigned short month() const override { return m_date.month(); }
  unsigned short day() const override { return m_date.day(); }
  bool is_infinity() const override { return m_date.is_infinity(); }
  bool is_neg_infinity() const override { return m_date.is_neg_infinity(); }
  bool is_pos_infinity() const override { return m_date.is_pos_infinity(); }
  bool is_not_a_date() const override { return m_date.is_not_a_date(); }
  bool is_special() const override { return m_date.is_special(); }

  int week_number() const { return m_date.week_number(); }
  bool operator==(const DateAbstr& date) const override {
    try {
      return m_date == dynamic_cast<const DateImpl&>(date).m_date;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator!=(const DateAbstr& date) const override {
    try {
      return m_date != dynamic_cast<const DateImpl&>(date).m_date;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator>(const DateAbstr& date) const override {
    try {
      return m_date > dynamic_cast<const DateImpl&>(date).m_date;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator<(const DateAbstr& date) const override {
    try {
      return m_date < dynamic_cast<const DateImpl&>(date).m_date;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator>=(const DateAbstr& date) const override {
    try {
      return m_date >= dynamic_cast<const DateImpl&>(date).m_date;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator<=(const DateAbstr& date) const override {
    try {
      return m_date <= dynamic_cast<const DateImpl&>(date).m_date;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateAbstrPtr operator-(const DateDurationAbstr& duration) const override {
    try {
      return std::make_shared< DateImpl >(m_date - dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateAbstrPtr operator+(const DateDurationAbstr& duration) const override {
    try {
      return std::make_shared< DateImpl >(m_date + dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateDurationAbstrPtr operator-(const DateAbstr& date) const override {
    try {
      return std::make_shared< DateDurationImpl >(m_date - dynamic_cast<const DateImpl&>(date).m_date);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateAbstrPtr operator-=(const DateDurationAbstr& duration) override {
    try {
      return std::make_shared< DateImpl>(m_date -= dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateAbstrPtr operator+=(const DateDurationAbstr& duration) override {
    try {
      return std::make_shared< DateImpl>(m_date += dynamic_cast<const DateDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
};

MISC_API Date::Date(const std::wstring& xdate, DateFormat format, const std::wstring& sep) {
  parse(xdate, format, sep);
}

void Date::parse(const std::wstring& xdate, DateFormat format, const std::wstring& sep) {
  std::wstring date = yloader::trim(xdate);

  unsigned int year;
  unsigned int month;
  unsigned int day;

  if (format != xyz) {
    unsigned int first;
    ;
    unsigned int second;
    unsigned int third;

    if (!sep.empty()) {
      Tokenizer tokens(date, sep);

      if (tokens.size() != 3) {
        throw DateException(date, yloader::format(L"Invalid date: \"%1%\"", date));
      }

      first = _ttoi(tokens[0].c_str());
      second = _ttoi(tokens[1].c_str());
      third = _ttoi(tokens[2].c_str());

    }
    else {
      if (date.length() == 6) {
        first = _ttoi(date.substr(0, 2).c_str());
        second = _ttoi(date.substr(2, 2).c_str());
        third = _ttoi(date.substr(4, 2).c_str());
      }
      else if (date.length() == 8) {
        switch (format) {
          case us:
          case european:
            first = _ttoi(date.substr(0, 2).c_str());
            second = _ttoi(date.substr(2, 2).c_str());
            third = _ttoi(date.substr(4, 4).c_str());
            break;
          case iso:
            first = _ttoi(date.substr(0, 4).c_str());
            second = _ttoi(date.substr(4, 2).c_str());
            third = _ttoi(date.substr(6, 2).c_str());
            break;
          default:
            throw DateException(date, L"Unknown format type" );
            break;
        }
      }
      else {
        throw DateException(date, yloader::format(L"Invalid date: \"%1%\"", date));
      }
    }

    switch (format) {
      case us:
        year = third;
        month = first;
        day = second;
        break;
      case european:
        year = third;
        month = second;
        day = first;
        break;
      case iso:
        year = first;
        month = second;
        day = third;
        break;
      default:
        throw DateException(date, L"Unknown format type");
        break;
    }

    year = year < 50 ? year + 2000 : (year < 100 ? year + 1900 : year);

    if (year < 1800 || year > 2100) {
      throw DateException(date, L"Year must be an integer value between 1800 and 2100");
    }

    if (month < 1 || month > 12) {
      throw DateException(date, L"Month must be an integer value between 1 and 12");
    }

    // make sure day is valid for month and year
    if (day < 1 || day > 31) {
      throw DateException(date, L"Day must be an integer value between 1 and 31");
    }
  }
  else {
    static std::wstring months[] = {L"", L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
                                    L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};

    Tokenizer tokens(xdate, sep);

    if (tokens.size() != 3) {
      throw DateException(date, L"Wrong date format");
    }

    unsigned int i = 0;
    for (; i <= 12; i++) {
      if (tokens[1] == months[i]) {
        month = i;
        break;
      }
    }

    if (i > 12) {
      throw DateException(date, L"Wrong date format");
    }

    year = _ttoi(tokens[2].c_str());

    if (year < 30) {
      year += 2000;
    }

    if (year < 100 && year >= 30) {
      year += 1900;
    }

    day = _ttoi(tokens[0].c_str());
  }
  m_date = DateAbstr::make(year, month, day);
}

class TimeDurationImpl : public TimeDurationAbstr {
  friend class DateTimeImpl;

 private:
  boost::posix_time::time_duration m_duration;

 public:
  TimeDurationImpl(long hours, long mins, long secs, long frac_secs)
      : m_duration(hours, mins, secs, frac_secs) {}

  TimeDurationImpl(boost::posix_time::time_duration duration)
      : m_duration(duration) {}

  long hours() const override { return m_duration.hours(); }
  long minutes() const override { return m_duration.minutes(); }
  long seconds() const override { return m_duration.seconds(); }
  long total_seconds() const override { return m_duration.total_seconds(); }
  long fractional_seconds() const override {
    return m_duration.fractional_seconds();
  }
  bool is_negative() const override {
    return m_duration.is_negative();
  }

  bool operator==(const TimeDurationAbstr& duration) const override {
    try {
      return m_duration == dynamic_cast<const TimeDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator!=(const TimeDurationAbstr& duration) const override {
    try {
      return m_duration != dynamic_cast<const TimeDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator>(const TimeDurationAbstr& duration) const override {
    try {
      return m_duration > dynamic_cast<const TimeDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator<(const TimeDurationAbstr& duration) const override {
    try {
      return m_duration < dynamic_cast<const TimeDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator>=(const TimeDurationAbstr& duration) const override {
    try {
      return m_duration >= dynamic_cast<const TimeDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator<=(const TimeDurationAbstr& duration) const override{
    try {
      return m_duration <= dynamic_cast<const TimeDurationImpl&>(duration).m_duration;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  TimeDurationAbstrPtr operator+(const TimeDurationAbstr& duration) const override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration + dynamic_cast<const TimeDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  TimeDurationAbstrPtr operator-(const TimeDurationAbstr& duration) const override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration - dynamic_cast<const TimeDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  TimeDurationAbstrPtr operator/(int divisor) override {
    try {
      return std::make_shared< TimeDurationImpl > (m_duration / divisor);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  TimeDurationAbstrPtr operator*(int factor) override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration * factor);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  TimeDurationAbstrPtr operator-=(const TimeDurationAbstr& duration) override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration -= dynamic_cast<const TimeDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  TimeDurationAbstrPtr operator+=(const TimeDurationAbstr& duration) override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration += dynamic_cast<const TimeDurationImpl&>(duration).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  TimeDurationAbstrPtr operator/=(int factor) override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration /= factor);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  TimeDurationAbstrPtr operator*=(int factor) override {
    try {
      return std::make_shared< TimeDurationImpl >(m_duration *= factor);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
};

class DateTimeImpl : public DateTimeAbstr {
 private:
  boost::posix_time::ptime m_time;

 public:
  DateTimeImpl() : m_time() {}

  DateTimeImpl(boost::posix_time::ptime time) : m_time(time) {}

  DateTimeImpl(const DateImpl& date) : m_time(date.m_date) {}

  DateTimeImpl(const DateImpl& date, const TimeDurationImpl& duration)
      : m_time(date.m_date, duration.m_duration) {}

  DateTimeImpl(const DateTimeImpl& time) : m_time(time.m_time) {}

  bool operator<(const DateTimeAbstr& xtime) const override {
    try {
      return m_time < dynamic_cast<const DateTimeImpl&>(xtime).m_time;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator>(const DateTimeAbstr& xtime) const override {
    try {
      return m_time > dynamic_cast<const DateTimeImpl&>(xtime).m_time;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator>=(const DateTimeAbstr& xtime) const override {
    try {
      return m_time >= dynamic_cast<const DateTimeImpl&>(xtime).m_time;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator<=(const DateTimeAbstr& xtime) const override {
    try {
      return m_time <= dynamic_cast<const DateTimeImpl&>(xtime).m_time;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  bool operator==(const DateTimeAbstr& xtime) const override {
    try {
      return m_time == dynamic_cast<const DateTimeImpl&>(xtime).m_time;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
  bool operator!=(const DateTimeAbstr& xtime) const override {
    try {
      return m_time != dynamic_cast<const DateTimeImpl&>(xtime).m_time;
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  std::wstring to_simple_string() const override {
    return boost::posix_time::to_simple_string_type<TCHAR>(m_time);
  }

  std::wstring to_iso_string() const override {
    return boost::posix_time::to_iso_string_type<TCHAR>(m_time);
  }

  __int64 to_epoch_time() const override {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));

    boost::posix_time::time_duration diff = m_time - epoch;

    return diff.total_seconds();
  }

  DateAbstrPtr date() const override {
    return DateAbstrPtr(new DateImpl(m_time.date()));
  }

  TimeDurationAbstrPtr time_of_day() const override {
    return TimeDurationAbstrPtr(new TimeDurationImpl(m_time.time_of_day()));
  }

  bool is_not_a_date_time() const override { return m_time.is_not_a_date_time(); }

  bool is_infinity() const override { return m_time.is_infinity(); }

  bool is_pos_infinity() const override { return m_time.is_pos_infinity(); }

  bool is_neg_infinity() const override { return m_time.is_neg_infinity(); }

  bool is_special() const override { return m_time.is_special(); }

  TimeDurationAbstrPtr operator-(const DateTimeAbstr& time) const override {
    try {
      return std::make_shared< TimeDurationImpl >(m_time - dynamic_cast<const DateTimeImpl&>(time).m_time);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator+(const DateDurationAbstr& dd) const override {
    try {
      return std::make_shared< DateTimeImpl >(m_time + dynamic_cast<const DateDurationImpl&>(dd).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator+=(const DateDurationAbstr& dd) override {
    try {
      return std::make_shared< DateTimeImpl >(m_time += dynamic_cast<const DateDurationImpl&>(dd).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator-(const DateDurationAbstr& dd) const override {
    try {
      return std::make_shared< DateTimeImpl >(m_time - dynamic_cast<const DateDurationImpl&>(dd).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator-=(const DateDurationAbstr& dd) override {
    try {
      return std::make_shared< DateTimeImpl >(m_time -= dynamic_cast<const DateDurationImpl&>(dd).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator+(const TimeDurationAbstr& td) const override {
    try {
      return std::make_shared< DateTimeImpl >(m_time + dynamic_cast<const TimeDurationImpl&>(td).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator+=(const TimeDurationAbstr& td) override {
    try {
      return std::make_shared < DateTimeImpl >(m_time += dynamic_cast<const TimeDurationImpl&>(td).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator-(const TimeDurationAbstr& td) const override {
    try {
      return std::make_shared < DateTimeImpl >(m_time - dynamic_cast<const TimeDurationImpl&>(td).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }

  DateTimeAbstrPtr operator-=(const TimeDurationAbstr& td) override {
    try {
      return std::make_shared < DateTimeImpl >(m_time -= dynamic_cast<const TimeDurationImpl&>(td).m_duration);
    }
    catch (const std::bad_cast&) {
      assert(false);
      throw;
    }
  }
};

MISC_API DateTimeAbstrPtr DateTimeAbstr::localTimeSubSec() {
  return std::make_shared< DateTimeImpl >(boost::date_time::microsec_clock<boost::posix_time::ptime>::local_time());
}

MISC_API DateTimeAbstrPtr DateTimeAbstr::localTimeSec() {
  return std::make_shared < DateTimeImpl >(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());
}

MISC_API DateTimeAbstrPtr DateTimeAbstr::universalTime() {
  return std::make_shared < DateTimeImpl >(boost::date_time::second_clock<boost::posix_time::ptime>::universal_time());
}

DateDurationAbstrPtr DateDurationAbstr::make(long days) {
  return std::make_shared < DateDurationImpl >(days);
}

DateDurationAbstrPtr DateDurationAbstr::make() {
  return std::make_shared < DateDurationImpl >();
}

DateTimeAbstrPtr DateTimeAbstr::makeFromIsoString(const std::wstring& iso_string) {
  return std::make_shared < DateTimeImpl >(boost::posix_time::from_iso_string(ws2s(iso_string)));
}

DateTimeAbstrPtr DateTimeAbstr::makeFromDelimitedString(const std::wstring& delimitedString) {
  return std::make_shared < DateTimeImpl >(boost::posix_time::time_from_string(ws2s(delimitedString)));
}

DateTimeAbstrPtr DateTimeAbstr::makeFromIsoExtendedString(const std::wstring& delimitedString) {
  boost::posix_time::ptime p(boost::date_time::parse_delimited_time<boost::posix_time::ptime>(ws2s(delimitedString), 'T'));
  return std::make_shared < DateTimeImpl >(p);
}

// YYYYMMDDHHMMSS
DateTimeAbstrPtr DateTimeAbstr::makeFromNonDelimitedString(const std::wstring& nonDelimitedString) {
  if (nonDelimitedString.length() != L"20091007233000"s.length()) {
    return std::make_shared< DateTimeImpl >();
  }
  else {
    std::wstring str(nonDelimitedString);
    str.insert(8, L"T" );
    return makeFromIsoString(str);
  }
}

DateTimeAbstrPtr DateTimeAbstr::makeFromUnixTime(const time_t time) {
  return std::make_shared< DateTimeImpl >(boost::posix_time::from_time_t(time));
}

TimeDurationAbstrPtr TimeDurationAbstr::make(long hours, long mins, long secs, long frac_secs) {
  return std::make_shared< TimeDurationImpl >(hours, mins, secs, frac_secs);
}

DateAbstrPtr DateAbstr::make(unsigned int year, unsigned int month, unsigned int day) {
  return DateAbstrPtr(new DateImpl(year, month, day));
}

DateAbstrPtr DateAbstr::make() { return std::make_shared< DateImpl >(); }

DateTimeAbstrPtr DateTimeAbstr::make(const DateAbstr& date, const TimeDurationAbstr& duration) {
  try {
    return std::make_shared< DateTimeImpl >(dynamic_cast<const DateImpl&>(date), dynamic_cast<const TimeDurationImpl&>(duration));
  }
  catch (const std::bad_cast&) {
    assert(false);
    return 0;
  }
}

DateTimeAbstrPtr DateTimeAbstr::make() {
  return DateTimeAbstrPtr(new DateTimeImpl());
}

DateTimeAbstrPtr DateTimeAbstr::make(const DateAbstr& date) {
  try {
    return std::make_shared< DateTimeImpl >(dynamic_cast<const DateImpl&>(date));
  }
  catch (const std::bad_cast&) {
    assert(false);
    return 0;
  }
}

DateTimeAbstrPtr DateTimeAbstr::make(const DateTimeAbstr& time) {
  try {
    return std::make_shared< DateTimeImpl >(dynamic_cast<const DateTimeImpl&>(time));
  }
  catch (const std::bad_cast&) {
    assert(false);
    return 0;
  }
}

// make special values
MISC_API DateAbstrPtr DateAbstr::makePosInfinity() {
  return std::make_shared< DateImpl >(boost::gregorian::date(boost::date_time::pos_infin));
}
MISC_API DateAbstrPtr DateAbstr::makeNegInfinity() {
  return std::make_shared < DateImpl >(boost::gregorian::date(boost::date_time::neg_infin));
}
MISC_API DateAbstrPtr DateAbstr::makeMaxDate() {
  return std::make_shared < DateImpl >(boost::gregorian::date(boost::date_time::max_date_time));
}
MISC_API DateAbstrPtr DateAbstr::makeMinDate() {
  return std::make_shared < DateImpl >(boost::gregorian::date(boost::date_time::min_date_time));
}
MISC_API DateAbstrPtr DateAbstr::makeNotADate() {
  return std::make_shared < DateImpl >(boost::gregorian::date(boost::date_time::not_a_date_time));
}

MISC_API DateTimeAbstrPtr DateTimeAbstr::makePosInfinity() {
  return std::make_shared < DateTimeImpl >(boost::posix_time::ptime(boost::date_time::pos_infin));
}

MISC_API DateTimeAbstrPtr DateTimeAbstr::makeNegInfinity() {
  return std::make_shared < DateTimeImpl >(boost::posix_time::ptime(boost::date_time::neg_infin));
}
MISC_API DateTimeAbstrPtr DateTimeAbstr::makeMaxDateTime() {
  return std::make_shared < DateTimeImpl >(boost::posix_time::ptime(boost::date_time::max_date_time));
}
MISC_API DateTimeAbstrPtr DateTimeAbstr::makeMinDateTime() {
  return std::make_shared < DateTimeImpl >(boost::posix_time::ptime(boost::date_time::min_date_time));
}
MISC_API DateTimeAbstrPtr DateTimeAbstr::makeNotADateTime() {
  return std::make_shared < DateTimeImpl >(boost::posix_time::ptime(boost::date_time::not_a_date_time));
}

DateAbstrPtr DateAbstr::make(const DateAbstr& date) {
  try {
    return std::make_shared < DateImpl >(dynamic_cast<const DateImpl&>(date));
  }
  catch (const std::bad_cast&) {
    assert(false);
    return 0;
  }
}

TimeDurationAbstrPtr TimeDurationAbstr::make(const TimeDurationAbstr& duration) {
  try {
    return std::make_shared < TimeDurationImpl >(dynamic_cast<const TimeDurationImpl&>(duration));
  }
  catch (const std::bad_cast&) {
    assert(false);
    return 0;
  }
}


class TimerImpl : public TimerAbstr {
 private:
  boost::timer m_timer;
  double m_lastValue;
  bool m_running;

 public:
  TimerImpl() : m_running(true), m_lastValue(0.0) {}

  void restart() {
    m_timer.restart();
    m_running = true;
  }

  double elapsed() const { return m_running ? m_timer.elapsed() : m_lastValue; }

  void stop() {
    if (m_running) {
      m_lastValue = m_timer.elapsed();
      m_running = false;
    }
  }

  bool isStopped() const { return !m_running; }
};

TimerAbstr* TimerAbstr::make() { return new TimerImpl(); }

MISC_API Version const Version::CURRENT(STRPRODUCT_VER);

Version::Version() : Version(0, 0, 0, 0) {}

yloader::Version::Version(const std::wstring& version)
    : Version() {
  parse(version.c_str());
}

MISC_API Version::Version(const TCHAR* version)
    : Version() {
  parse(version);
}

Version::Version(unsigned int major, unsigned int minor, unsigned int revision, unsigned int build)
    : m_major(major), m_minor(minor), m_revision(revision), m_build(build) {}

MISC_API void Version::parse(const TCHAR* version) {
  if (version != nullptr && _tcslen(version) > 0) {
    Tokenizer tokens(version, VERSION_SEPARATORS);

    try {
      for (size_t n = 0; n < tokens.size(); ++n) {
        unsigned int value = boost::lexical_cast<unsigned int>(tokens[n]);
        switch (n) {
          case 0:
            m_major = value;
            break;
          case 1:
            m_minor = value;
            break;
          case 2:
            m_revision = value;
            break;
          case 3:
            m_build = value;
            break;
          default:
            throw VersionException(std::wstring(version) + L" - too many version elements");
        }
      }
    }
    catch (const boost::bad_lexical_cast& e) {
      const char* what = e.what();
      throw VersionException(std::wstring(version) + L", " + yloader::s2ws(e.what()));
    }
  }
}

MISC_API Seconds::Seconds(double seconds)
    : TimeDuration(0, 0, (long)seconds, fmod(seconds, 1.0) * 1000000) {}

MISC_API std::vector<std::string> yloader::cmdLineSplitter(const std::wstring& line) {
  std::vector<std::string> v;

  bool inQuotedString = false;
  bool inUnquotedString = false;
  bool escape = false;

  std::wstring sep(L" \t" );
  std::wstring str;
  unsigned int state = 0;

  for (auto c : line ) {
    switch (state) {
      case 0:
        assert(str.empty());
        switch (c) {
          case L'"':
            state = 1;
            break;
          case L' ':
          case L'\t':
            break;
          default:
            str += c;
            state = 2;
            break;
        }
        break;
      case 1:
        switch (c) {
          case L'"':
            state = 2;
            break;
          case L'\\':
            str += c;
            state = 3;
            break;
          default:
            str += c;
            break;
        }
        break;
      case 2:
        switch (c) {
          case L' ':
          case L'\t':
            // assert( !str.empty() );
            if (!str.empty()) {
              v.push_back(yloader::ws2s(str));
              str.clear();
            }
            state = 0;
            break;
          default:
            str += c;
            break;
        }
        break;
      case 3:
        switch (c) {
          case L'"':
            state = 4;
            break;
          default:
            state = 1;
            break;
        }
        str += c;
        break;
      case 4:
        switch (c) {
          case L' ':
          case L'\t':
            // assert( !str.empty() );
            if (!str.empty()) {
              v.push_back(yloader::ws2s(str));
              str.clear();
            }
            state = 0;
            break;
          default:
            str += c;
            state = 1;
            break;
        }
        break;
      default:
        assert(false);
        break;
    }
  }

  if (!str.empty()) {
    v.push_back(yloader::ws2s(str));
  }
  return v;
}

MISC_API std::wstring yloader::Date::toString(DateFormat format, const std::wstring& separator) const {
  if (is_special()) {
    return L"";
  }
  else {
    unsigned int first;
    unsigned int second;
    unsigned int third;

    switch (format) {
      case us:
        first = month();
        second = day();
        third = year();
        break;
      case european:
        first = day();
        second = month();
        third = year();
        break;
      case iso:
        first = year();
        second = month();
        third = day();
        break;
      case xyz:
        throw DateException(L"", L"Format xyz not supported at this time");
      default:
        throw DateException(L"", L"Unknown format type" );
    }

    return yloader::format(L"%1%%4%%2%%4%%3%", first, second, third, separator);
  }
}
