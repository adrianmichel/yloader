/*
Copyright(C) 2017  YLoader.com

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

#include "sharedptr.h"
#include "strings.h"

#ifdef MISC_EXPORTS
#define MISC_API __declspec(dllexport)
#else
#define MISC_API __declspec(dllimport)
#endif

using namespace std::string_literals;

namespace yloader {

/**\defgroup TimeDate Time and Date related definition
 * Time, date, duration, timer classes
 * @{
 */

/* @cond XXXXX */
class TimeDurationAbstr;
using TimeDurationAbstrPtr = std::shared_ptr<TimeDurationAbstr>;

class TimeDurationAbstr {
 public:
  virtual ~TimeDurationAbstr() {}
  MISC_API static TimeDurationAbstrPtr make(long hours, long mins, long secs,
                                            long frac_sec);
  MISC_API static TimeDurationAbstrPtr make(const TimeDurationAbstr& date);

  virtual long hours() const = 0;
  virtual long minutes() const = 0;
  virtual long seconds() const = 0;
  virtual long total_seconds() const = 0;
  virtual long fractional_seconds() const = 0;
  virtual bool is_negative() const = 0;
  //	virtual time_duration invert_sign() const = 0;
  virtual bool operator==(const TimeDurationAbstr& duration) const = 0;
  virtual bool operator!=(const TimeDurationAbstr& duration) const = 0;
  virtual bool operator>(const TimeDurationAbstr& duration) const = 0;
  virtual bool operator<(const TimeDurationAbstr& duration) const = 0;
  virtual bool operator>=(const TimeDurationAbstr& duration) const = 0;
  virtual bool operator<=(const TimeDurationAbstr& duration) const = 0;
  virtual TimeDurationAbstrPtr operator+(
      const TimeDurationAbstr& duration) const = 0;
  virtual TimeDurationAbstrPtr operator-(
      const TimeDurationAbstr& duration) const = 0;
  virtual TimeDurationAbstrPtr operator/(int factor) = 0;
  virtual TimeDurationAbstrPtr operator*(int factor) = 0;
  virtual TimeDurationAbstrPtr operator-=(
      const TimeDurationAbstr& duration) = 0;
  virtual TimeDurationAbstrPtr operator+=(
      const TimeDurationAbstr& duration) = 0;
  virtual TimeDurationAbstrPtr operator/=(int factor) = 0;
  virtual TimeDurationAbstrPtr operator*=(int factor) = 0;
};

class DateDurationAbstr;
using DateDurationAbstrPtr = std::shared_ptr<DateDurationAbstr>;

class DateDurationAbstr {
 public:
  virtual ~DateDurationAbstr() {}

  MISC_API static DateDurationAbstrPtr make(long days);
  MISC_API static DateDurationAbstrPtr make();

  virtual long days() const = 0;
  virtual bool is_negative() const = 0;

  virtual bool operator==(const DateDurationAbstr& duration) const = 0;
  virtual bool operator!=(const DateDurationAbstr& duration) const = 0;
  virtual bool operator>(const DateDurationAbstr& duration) const = 0;
  virtual bool operator<(const DateDurationAbstr& duration) const = 0;
  virtual bool operator>=(const DateDurationAbstr& duration) const = 0;
  virtual bool operator<=(const DateDurationAbstr& duration) const = 0;
  virtual DateDurationAbstrPtr operator+(
      const DateDurationAbstr& duration) const = 0;
  virtual DateDurationAbstrPtr operator-(
      const DateDurationAbstr& duration) const = 0;
  virtual DateDurationAbstrPtr operator/(int factor) = 0;
  virtual DateDurationAbstrPtr operator-=(
      const DateDurationAbstr& duration) = 0;
  virtual DateDurationAbstrPtr operator+=(
      const DateDurationAbstr& duration) = 0;
  virtual DateDurationAbstrPtr operator/=(int factor) = 0;
};

class DateAbstr;
using DateAbstrPtr = std::shared_ptr<DateAbstr>;

class DateAbstr {
  friend class Date;

 public:
  virtual ~DateAbstr() {}
  MISC_API static DateAbstrPtr make(unsigned int year, unsigned int month,
                                    unsigned int day);
  MISC_API static DateAbstrPtr make(const DateAbstr& date);
  MISC_API static DateAbstrPtr make();

  virtual unsigned short year() const = 0;
  virtual unsigned short month() const = 0;
  virtual unsigned short day() const = 0;
  virtual bool is_infinity() const = 0;
  virtual bool is_neg_infinity() const = 0;
  virtual bool is_pos_infinity() const = 0;
  virtual bool is_not_a_date() const = 0;
  virtual bool is_special() const = 0;
  virtual int week_number() const = 0;

  virtual bool operator==(const DateAbstr& date) const = 0;
  virtual bool operator!=(const DateAbstr& date) const = 0;
  virtual bool operator>(const DateAbstr& date) const = 0;
  virtual bool operator<(const DateAbstr& date) const = 0;
  virtual bool operator>=(const DateAbstr& date) const = 0;
  virtual bool operator<=(const DateAbstr& date) const = 0;
  virtual DateAbstrPtr operator-(const DateDurationAbstr& duration) const = 0;
  virtual DateAbstrPtr operator+(const DateDurationAbstr& duration) const = 0;
  virtual DateDurationAbstrPtr operator-(const DateAbstr& date) const = 0;
  virtual DateAbstrPtr operator-=(const DateDurationAbstr& duration) = 0;
  virtual DateAbstrPtr operator+=(const DateDurationAbstr& duration) = 0;
  virtual std::wstring to_simple_string() const = 0;
  virtual std::wstring to_iso_string() const = 0;
  virtual std::wstring to_iso_extended_string() const = 0;

  // make special values
  MISC_API static DateAbstrPtr makePosInfinity();
  MISC_API static DateAbstrPtr makeNegInfinity();
  MISC_API static DateAbstrPtr makeMaxDate();
  MISC_API static DateAbstrPtr makeMinDate();
  MISC_API static DateAbstrPtr makeNotADate();
};

class DateTimeAbstr;
using DateTimeAbstrPtr = std::shared_ptr<DateTimeAbstr>;

class DateTimeAbstr {
 public:
  virtual ~DateTimeAbstr() {}
  virtual std::wstring to_simple_string() const = 0;
  virtual std::wstring to_iso_string() const = 0;
  virtual __int64 to_epoch_time() const = 0;
  virtual bool operator<(const DateTimeAbstr& xtime) const = 0;
  virtual bool operator>(const DateTimeAbstr& xtime) const = 0;
  virtual bool operator>=(const DateTimeAbstr& xtime) const = 0;
  virtual bool operator<=(const DateTimeAbstr& xtime) const = 0;
  virtual bool operator==(const DateTimeAbstr& xtime) const = 0;
  virtual bool operator!=(const DateTimeAbstr& xtime) const = 0;

  //	virtual TimeAbstrPtr operator+
  virtual DateAbstrPtr date() const = 0;
  virtual TimeDurationAbstrPtr time_of_day() const = 0;

  MISC_API static DateTimeAbstrPtr make();
  MISC_API static DateTimeAbstrPtr make(const DateAbstr& date,
                                        const TimeDurationAbstr& duration);
  MISC_API static DateTimeAbstrPtr make(const DateAbstr& date);
  MISC_API static DateTimeAbstrPtr make(const DateTimeAbstr& time);
  MISC_API static DateTimeAbstrPtr makeFromIsoString(
      const std::wstring& iso_string);
  MISC_API static DateTimeAbstrPtr makeFromDelimitedString(
      const std::wstring& delimitedString);
  MISC_API static DateTimeAbstrPtr makeFromIsoExtendedString(
      const std::wstring& delimitedString);
  MISC_API static DateTimeAbstrPtr makeFromNonDelimitedString(
      const std::wstring& delimitedString);
  MISC_API static DateTimeAbstrPtr makeFromUnixTime(const time_t time);
  // make special values
  MISC_API static DateTimeAbstrPtr makePosInfinity();
  MISC_API static DateTimeAbstrPtr makeNegInfinity();
  MISC_API static DateTimeAbstrPtr makeMaxDateTime();
  MISC_API static DateTimeAbstrPtr makeMinDateTime();
  MISC_API static DateTimeAbstrPtr makeNotADateTime();

  MISC_API static DateTimeAbstrPtr localTimeSec();
  MISC_API static DateTimeAbstrPtr localTimeSubSec();
  MISC_API static DateTimeAbstrPtr universalTime();

  virtual bool is_not_a_date_time() const = 0;
  virtual bool is_infinity() const = 0;
  virtual bool is_pos_infinity() const = 0;
  virtual bool is_neg_infinity() const = 0;
  virtual bool is_special() const = 0;
  virtual TimeDurationAbstrPtr operator-(const DateTimeAbstr& time) const = 0;
  virtual DateTimeAbstrPtr operator+(const DateDurationAbstr& dd) const = 0;
  virtual DateTimeAbstrPtr operator+=(const DateDurationAbstr& dd) = 0;
  virtual DateTimeAbstrPtr operator-(const DateDurationAbstr& dd) const = 0;
  virtual DateTimeAbstrPtr operator-=(const DateDurationAbstr& dd) = 0;
  virtual DateTimeAbstrPtr operator+(const TimeDurationAbstr& td) const = 0;
  virtual DateTimeAbstrPtr operator+=(const TimeDurationAbstr& td) = 0;
  virtual DateTimeAbstrPtr operator-(const TimeDurationAbstr& rhs) const = 0;
  virtual DateTimeAbstrPtr operator-=(const TimeDurationAbstr& td) = 0;
};

/* @endcond */

/**
 * Duration class
 *
 * Implements the duration concept, i.e. the "distance" between two time points
 *
 * @see DateTime
 * @see Date
 * @see Timer
 */
class TimeDuration {
  friend class DateTime;

 private:
  TimeDuration(TimeDurationAbstrPtr duration) : m_duration(duration) {}

 private:
  TimeDurationAbstrPtr m_duration;

 public:
  /**
   * Constructor taking the duration numerical value as parameters (hours,
   * minutes, seconds and fractions of a second)
   *
   * The fractions are in microseconds. It is not guaranteed that the target OS
   * or machine supports this resolution, but the capability is there
   * nonetheless.
   *
   * @param hours    duration in hours
   * @param mins     duration in minutes
   * @param secs     duration in seconds
   * @param frac_sec duration in fraction of a second (microsecond)
   */
  TimeDuration(long hours, long mins, long secs = 0, long frac_sec = 0)
      : m_duration(TimeDurationAbstrPtr(
            TimeDurationAbstr::make(hours, mins, secs, frac_sec))) {}

  /**
   * Copy constructor
   *
   * Constructs a duration from another duration
   *
   * @param duration The source duration
   */
  TimeDuration(const TimeDuration& duration) {
    m_duration =
        TimeDurationAbstrPtr(TimeDurationAbstr::make(*(duration.m_duration)));
  }

  /**
   * Get the number of normalized hours
   *
   * Ex: time_duration( 1, 2, 3 ).hours is 1
   *
   * @return the number of normalized hours
   */
  long hours() const { return m_duration->hours(); }
  /**
   * Get the number of normalized minutes
   *
   * Ex: time_duration( 1, 2, 3 ).minutes is 2
   *
   * @return the number of normalized minutes
   */
  long minutes() const { return m_duration->minutes(); }
  /**
   * Get the number of normalized seconds
   *
   * Ex: time_duration( 1, 2, 3 ).seconds is 1
   *
   * @return the number of normalized seconds
   */
  long seconds() const { return m_duration->seconds(); }
  /**
   * Get the total number of seconds truncating any fractional seconds
   *
   * Ex: time_duration(1,2,3,10).total_seconds() is (1*3600) + (2*60) + 3 ==
   * 3723
   *
   * @return
   */
  long total_seconds() const { return m_duration->total_seconds(); }
  long totalSeconds() const { return m_duration->total_seconds(); }
  /**
   * Get the number of fractional seconds
   *
   * Ex: time_duration(1,2,3, 1000).fractional_seconds() is 1000
   *
   * @return
   */
  long fractional_seconds() const { return m_duration->fractional_seconds(); }
  long fractionalSeconds() const { return m_duration->fractional_seconds(); }
  /**
   * Returns true if the duration is negtive
   *
   * Ex: time_duration(-1,0,0).is_negative() is true
   *
   * @return true if the duration is negative
   */
  bool is_negative() const { return m_duration->is_negative(); }
  bool isNegative() const { return m_duration->is_negative(); }
  //	virtual time_duration invert_sign() const

  /**
   * Operator ==
   *
   * Compares two durations and returns true if the current duration is == to
   * the argument duration
   *
   * @param duration duration to compare to
   *
   * @return true if ==, false otherwise
   */
  bool operator==(const TimeDuration& duration) const {
    return m_duration->operator==(*(duration.m_duration));
  }
  /**
   * Operator !=
   *
   * Compares two durations and returns true if the current duration is != from
   * the argument duration
   *
   * @param duration  duration to compare to
   * @return true if != false otherwise
   */
  bool operator!=(const TimeDuration& duration) const {
    return m_duration->operator!=(*(duration.m_duration));
  }
  /**
   * Operator >
   *
   * Compares two durations and returns true if the current duration is > than
   * the argument duration
   *
   * @param duration  duration to compare to
   * @return true if >, false otherwise
   */
  bool operator>(const TimeDuration& duration) const {
    return m_duration->operator>(*(duration.m_duration));
  }
  /**
   * Operator <
   *
   * Compares two durations and returns true if the current duration is < than
   * the argument duration
   *
   * @param duration  duration to compare to
   * @return true if < false otherwise
   */
  bool operator<(const TimeDuration& duration) const {
    return m_duration->operator<(*(duration.m_duration));
  }
  /**
   * Operator >=
   *
   * Compares two durations and returns true if the current duration is >= than
   * the argument duration
   *
   * @param duration  duration to compare to
   * @return true if >= false otherwise
   */
  bool operator>=(const TimeDuration& duration) const {
    return m_duration->operator>=(*(duration.m_duration));
  }
  /**
   * Operator <=
   *
   * Compares two durations and returns true if the current duration is <= than
   * the argument duration
   *
   * @param duration  duration to compare to
   * @return true if <= false otherwise
   */
  bool operator<=(const TimeDuration& duration) const {
    return m_duration->operator<=(*(duration.m_duration));
  }
  /**
   * Adds two time durations, and returns the result
   *
   * @param duration The time duration to be added to the current time duration
   *
   * @return The time duration result of the addition
   */
  TimeDuration operator+(const TimeDuration& duration) const {
    return m_duration->operator+(*(duration.m_duration));
  }
  /**
   * Subtracts a time durations from the current time duration and returns the
   * result
   *
   * @param duration The time duration to be subtracted from the current time
   * duration
   *
   * @return The time duration result of the subtraction
   */
  TimeDuration operator-(const TimeDuration& duration) const {
    return m_duration->operator-(*(duration.m_duration));
  }
  /**
   * Divides the current time duration by an integer value, and discards the
   * reminder
   *
   * Returns the resulting time duration
   *
   * @param divisor The divisor
   *
   * @return The time duration result of the division
   */
  TimeDuration operator/(int divisor) { return *m_duration / divisor; }
  /**
   * Multiplies the current time duration by an integer factor
   *
   * Returns the time duration result of the multiplication
   *
   * @param factor the factor to multiply with
   *
   * @return The time duration result of the multiplication
   */
  TimeDuration operator*(int factor) { return *m_duration * factor; }
  /**
   * Subtracts a time duration from the current time duration and assigns the
   * result to the current duration. Also returns the result of the operation
   *
   * @param duration The time duration to be subtracted from the current time
   * duration
   *
   * @return The time duration result of the subtraction
   */
  TimeDuration operator-=(const TimeDuration& duration) {
    return m_duration->operator-=(*(duration.m_duration));
  }
  /**
   * Adds a time duration to the current time duration and assigns the result
   * to the current duration. Also returns the result of the operation
   *
   * @param duration The time duration to be added to the current time duration
   *
   * @return The time duration result of the addition
   */
  TimeDuration operator+=(const TimeDuration& duration) {
    return m_duration->operator+=(*(duration.m_duration));
  }
  /**
   * Divides the current time duration by an integer value, and discards the
   * reminder
   *
   * Assigns the result to the current time duration and returns the resulting
   * time duration
   *
   * @param divisor The divisor
   *
   * @return The time duration result of the division
   */
  TimeDuration operator/=(int divisor) { return *m_duration /= divisor; }
  /**
   * Multiplies the current time duration by an integer factor
   *
   * Assigns the result to the current time duration and returns the resulting
   * time duration
   *
   * @param factor the factor to multiply with
   *
   * @return The time duration result of the multiplication
   */
  TimeDuration operator*=(int factor) { return *m_duration *= factor; }
  const std::wstring toString() const {
    std::wostringstream h, m, s;

    h << hours();

    if (minutes() < 10 && minutes() >= 0) {
      m << L"0" << minutes();
    }
    else if (minutes() > -10 && minutes() < 0) {
      m << L"-0" << -minutes();
    }
    else {
      m << minutes();
    }

    if (seconds() < 10 && seconds() >= 0) {
      s << L"0" << seconds();
    }
    else if (seconds() > -10 && seconds() < 0) {
      s << L"-0" << -seconds();
    }
    else {
      s << seconds();
    }

    return h.str() + L":" + m.str() + L":" + s.str();
  }
};

/**
 * Convenience class used to create a TimeDuration from a number of hours
 */
class Hours : public TimeDuration {
 public:
  /**
   * Constructor that thakes the hours duration
   *
   * @param hours  Number of hours
   */
  Hours(long hours) : TimeDuration(hours, 0, 0, 0) {}
};

/**
 * Convenience class used to create a TimeDuration from a number of minutes
 */
class Minutes : public TimeDuration {
 public:
  /**
   * Constructor that thakes the minutes duration
   *
   * @param hours  Number of minutes
   */
  Minutes(long minutes) : TimeDuration(0, minutes, 0, 0) {}
};

/**
 * Convenience class used to create a TimeDuration from a number of seconds
 */
class Seconds : public TimeDuration {
 public:
  /**
   * Constructor that thakes the seconds duration
   *
   * @param hours  Number of seconds
   */
  Seconds(long seconds) : TimeDuration(0, 0, seconds, 0) {}

  /**
   * Constructor that thakes the seconds duration
   *
   * The seconds duration is represented as a double, with the integer part
   * being the number of seconds and the fractional part the fraction of seconds
   *
   * @param hours  Number of seconds
   */
  MISC_API Seconds(double seconds);
};

/**
 * Convenience class used to create a TimeDuration from a number of miliseconds
 */
class Miliseconds : public TimeDuration {
 public:
  /**
   * Constructor that thakes the miliseconds duration
   *
   * @param hours  Number of miliseconds
   */
  Miliseconds(long miliseconds) : TimeDuration(0, 0, 0, miliseconds * 1000) {}
};

/**
 * Convenience class used to create a TimeDuration from a number of microseconds
 */
class Microseconds : public TimeDuration {
 public:
  /**
   * Constructor that thakes the microseconds duration
   *
   * @param hours  Number of microseconds
   */
  Microseconds(long microseconds) : TimeDuration(0, 0, 0, microseconds) {}
};

class Date;

/**
 * A date duration (or days) class
 *
 * Useful when doing date arithmetic, such as adding or subtracting
 * days to a date, or subtracting 2 dates.
 *
 * Can be positive or negative
 */
class DateDuration {
  friend Date;
  friend DateTime;

 private:
  DateDurationAbstrPtr m_duration;

 private:
  DateDuration(DateDurationAbstrPtr duration) : m_duration(duration) {}

 public:
  /**
   * Constructor - takes a number of days as argument
   *
   * The number of days can be positive or negative
   *
   * @param days   number of days
   */
  DateDuration(long days) : m_duration(DateDurationAbstr::make(days)) {}

  /**
   * Default constructor - makes a DateDuration equal to the date duration unit
   * or 1 day
   */
  DateDuration() : m_duration(DateDurationAbstr::make()) {}

  /**
   * The nuber of days represented by the date duration
   *
   * @return number of days
   */
  long days() const { return m_duration->days(); }

  /**
   * Indicates whether the date duration is negative
   *
   * @return true if negative, false otherwise
   */
  bool is_negative() const { return m_duration->is_negative(); }
  bool isNegative() const { return m_duration->is_negative(); }

  /**
   * Operator ==
   *
   * Compares two date durations and returns true if the current date duration
   * is == to the argument date duration
   *
   * @param duration
   *
   * @return true if ==, false otherwise
   */
  bool operator==(const DateDuration& duration) const {
    return *m_duration == *(duration.m_duration);
  }
  /**
   * Operator !=
   *
   * Compares two date durations and returns true if the current date duration
   * is != than the argument date duration
   *
   * @param duration
   *
   * @return true if !=, false otherwise
   */
  bool operator!=(const DateDuration& duration) const {
    return *m_duration != *(duration.m_duration);
  }
  /**
   * Operator >
   *
   * Compares two date durations and returns true if the current date duration
   * is > than the argument date duration
   *
   * @param duration
   *
   * @return true if >, false otherwise
   */
  bool operator>(const DateDuration& duration) const {
    return *m_duration > *(duration.m_duration);
  }
  /**
   * Operator <
   *
   * Compares two date durations and returns true if the current date duration
   * is < than the argument date duration
   *
   * @param duration
   *
   * @return true if <, false otherwise
   */
  bool operator<(const DateDuration& duration) const {
    return *m_duration < *(duration.m_duration);
  }
  /**
   * Operator >=
   *
   * Compares two date durations and returns true if the current date duration
   * is >= than the argument date duration
   *
   * @param duration
   *
   * @return true if >=, false otherwise
   */
  bool operator>=(const DateDuration& duration) const {
    return *m_duration >= *(duration.m_duration);
  }
  /**
   * Operator <=
   *
   * Compares two date durations and returns true if the current date duration
   * is <= than the argument date duration
   *
   * @param duration
   *
   * @return true if <=, false otherwise
   */
  bool operator<=(const DateDuration& duration) const {
    return *m_duration <= *(duration.m_duration);
  }

  /**
   * Adds two date durations, and returns the result
   *
   * @param duration The date duration to be added to the current date duration
   *
   * @return The date duration result of the addition
   */
  DateDuration operator+(const DateDuration& duration) const {
    return m_duration->operator+(*(duration.m_duration));
  }
  /**
   * Subtracts a date durations from the current date duration and returns the
   * result
   *
   * @param duration The date duration to be subtracted from the current date
   * duration
   *
   * @return The date duration result of the subtraction
   */
  DateDuration operator-(const DateDuration& duration) const {
    return m_duration->operator-(*(duration.m_duration));
  }
  /**
   * Divides the current date duration by an integer value, and discards the
   * reminder
   *
   * Returns the resulting date duration
   *
   * @param divisor The divisor
   *
   * @return The date duration result of the division
   */
  DateDuration operator/(int divisor) { return *m_duration / divisor; }
  /**
   * Subtracts a date duration from the current date duration and assigns the
   * result to the current duration. Also returns the result of the operation
   *
   * @param duration The date duration to be subtracted from the current date
   * duration
   *
   * @return The date duration result of the subtraction
   */
  DateDuration operator-=(const DateDuration& duration) {
    return m_duration->operator-=(*(duration.m_duration));
  }
  /**
   * Adds a date duration to the current date duration and assigns the result
   * to the current duration. Also returns the result of the operation
   *
   * @param duration The date duration to be added to the current date duration
   *
   * @return The date duration result of the addition
   */
  DateDuration operator+=(const DateDuration& duration) {
    return m_duration->operator+=(*(duration.m_duration));
  }
  /**
   * Divides the current date duration by an integer value, and discards the
   * reminder
   *
   * Assigns the result to the current date duration and returns the resulting
   * date duration
   *
   * @param divisor The divisor
   *
   * @return The date duration result of the division
   */
  DateDuration operator/=(int divisor) { return *m_duration /= divisor; }
};

class Days : public DateDuration {
 public:
  Days(long days) : DateDuration(days) {}
};

class DateException {
 private:
  const std::wstring m_date;
  const std::wstring m_message;

 public:
  DateException(const std::wstring& date, const std::wstring& message)
      : m_date(date), m_message(message) {}

  const std::wstring date() const { return m_date; }

  const std::wstring message() const {
    std::wostringstream o;

    o << m_message << L": " << m_date;
    return o.str();
  }
};

enum DateFormat {
  us,        // m/d/y
  european,  // d/m/y
  iso,       // y/m/d
  xyz        // 25-Jan-2007
};

inline std::wstring dateFormatToString(DateFormat dateFormat, const std::wstring& sep) {
  switch (dateFormat) {
    case us:
      return L"US (m"s + sep + L"d" + sep + L"y)";
    case european:
      return L"EU (d"s + sep + L"m" + sep + L"y)";
    case iso:
      return L"ISO (y"s + sep + L"m" + sep + L"d)";
    case xyz:
      return L"d"s + sep + L"m" + sep + L"y (ex: 25" + sep + L"Jan" + sep + L"2020)";
    default:
      return L"not a valid date format";
  }
}

/**
 * Date class
 *
 * Implements date functionality
 *
 * @see DateTime
 * @see DateDuration
 * @see Timer
 */
class Date {
  friend class DateTime;

 private:
  DateAbstrPtr m_date;

 private:
   void parse(const std::wstring& xdate, DateFormat format, const std::wstring& sep);

 protected:
  Date(DateAbstrPtr date) : m_date(date) {}

 public:
  /**
   * Constructor that takes a year, month and day as parameters
   *
   * @param year   The year
   * @param month  The month
   * @param day    The day
   */
  Date(unsigned int year, unsigned int month, unsigned int day)
      : m_date(DateAbstr::make(year, month, day)) {}

  /**
   * Copy constructor
   *
   * Creates an Date object from an existing Date object
   *
   * @param date   The source date
   */
  Date(const Date& date) {
    m_date = DateAbstrPtr(DateAbstr::make(*(date.m_date)));
  }

  /**
   * Default constructor
   *
   * Creates a Date object set to not_a_date
   *
   */
  Date() { m_date = DateAbstrPtr(DateAbstr::make()); }

  // if year is xx < 100, the year will be considered to be 20xx, for ex 06 will
  // be 2006. sep indicates whether the fields are separated or not. Accepted
  // separators: / and -
//  MISC_API explicit Date( const std::wstring& date, DateFormat format, bool
//  sep ) throw( DateException );
#define DEF_DATE_SEP L"/-"
  MISC_API explicit Date(const std::wstring& date, DateFormat format = us, const std::wstring& separator = DEF_DATE_SEP);

  MISC_API Date(const std::wstring& date, const std::wstring& format);
  /**
   * Get the year part of the date
   *
   * @return the year
   */
  unsigned short year() const { return m_date->year(); }
  /**
   * Get the month part of the date
   *
   * @return the month
   */
  unsigned short month() const { return m_date->month(); }
  /**
   * Get the day part of the date
   *
   * @return the day
   */
  unsigned short day() const { return m_date->day(); }
  /**
   * Returns true if date is either positive or negative infinity
   *
   * @return true if positive or negative infinity
   */
  bool is_infinity() const { return m_date->is_infinity(); }
  bool isInfinity() const { return m_date->is_infinity(); }
  /**
   * Returns true if date is negative infinity
   *
   * @return true if negative infinity
   */
  bool is_neg_infinity() const { return m_date->is_neg_infinity(); }
  bool isNegInfinity() const { return m_date->is_neg_infinity(); }
  /**
   * Returns true if date is positive infinity
   *
   * @return true if date is positive infinity
   */
  bool is_pos_infinity() const { return m_date->is_pos_infinity(); }
  bool isPosInfinity() const { return m_date->is_pos_infinity(); }
  /**
   * Returns true if date is not a valid date
   *
   * @return true if date is not a valid date
   */
  bool is_not_a_date() const { return m_date->is_not_a_date(); }
  bool isNotADate() const { return m_date->is_not_a_date(); }
  /**
   * returns the ISO 8601 week number for date
   *
   * @return the ISO 8601 week number for date
   */
  int week_number() const { return m_date->week_number(); }
  int weekNumber() const { return m_date->week_number(); }

  bool is_special() const { return m_date->is_special(); }
  bool isSpecial() const { return m_date->is_special(); }
  /**
   * Operator ==
   *
   * Compares two dates and returns true if the current date is == to the
   * argument date
   *
   * @param date  date to compare to
   * @return true if ==, false otherwise
   */
  bool operator==(const Date& date) const {
    return m_date->operator==(*(date.m_date));
  }
  /**
   * Operator !=
   *
   * Compares two dates and returns true if the current date is != than the
   * argument date
   *
   * @param date  date to compare to
   * @return true if !=, false otherwise
   */
  bool operator!=(const Date& date) const {
    return m_date->operator!=(*(date.m_date));
  }
  /**
   * Operator >
   *
   * Compares two dates and returns true if the current date is > than the
   * argument date
   *
   * @param date  date to compare to
   * @return true if >, false otherwise
   */
  bool operator>(const Date& date) const {
    return m_date->operator>(*(date.m_date));
  }
  /**
   * Operator <
   *
   * Compares two dates and returns true if the current date is < than the
   * argument date
   *
   * @param date  date to compare to
   * @return true if <, false otherwise
   */
  bool operator<(const Date& date) const {
    return m_date->operator<(*(date.m_date));
  }
  /**
   * Operator >=
   *
   * Compares two dates and returns true if the current date is >= than the
   * argument date
   *
   * @param date  date to compare to
   * @return true if >=, false otherwise
   */
  bool operator>=(const Date& date) const {
    return m_date->operator>=(*(date.m_date));
  }
  /**
   * Operator <=
   *
   * Compares two dates and returns true if the current date is <= than the
   * argument date
   *
   * @param date  date to compare to
   * @return true if <=, false otherwise
   */
  bool operator<=(const Date& date) const {
    return m_date->operator<=(*(date.m_date));
  }

  /**
   * Adds a DateDuration to the current Date
   *
   * @param duration the date duration to be added to the current date
   *
   * @return The new date
   */
  Date operator+(const DateDuration& duration) const {
    return m_date->operator+(*(duration.m_duration));
  }
  /**
   * Subtracts a DateDuration from the current Date
   *
   * @param duration the date duration to be subtracted from the current date
   *
   * @return The new date
   */
  Date operator-(const DateDuration& duration) const {
    return m_date->operator-(*(duration.m_duration));
  }
  /**
   * Subtracts a Date from the current Date. The result is a DateDuration
   *
   * @param date   The Date to subtract from the current Date
   *
   * @return The difference between the two dates
   */
  DateDuration operator-(const Date& date) const {
    return m_date->operator-(*(date.m_date));
  }

  /**
   * Adds a DateDuration to the current Date
   *
   * @param duration the date duration to be added to the current date
   *
   * @return The new date
   */
  Date operator+=(const DateDuration& duration) {
    return m_date->operator+=(*(duration.m_duration));
  }
  /**
   * Subtracts a DateDuration from the current Date
   *
   * @param duration the date duration to be subtracted from the current date
   *
   * @return The new date
   */
  Date operator-=(const DateDuration& duration) {
    return m_date->operator-=(*(duration.m_duration));
  }
  /**
   * assignment operator
   */
  const Date& operator=(const Date& date) {
    if (this != &date) m_date = DateAbstr::make(*(date.m_date));
    return *this;
  }
  const Date& operator++() {
    *this += Days(1);
    return *this;
  }
  const Date& operator--() {
    *this -= Days(1);
    return *this;
  }
  const Date operator++(int) {
    Date temp = *this;
    *this += Days(1);
    return temp;
  }
  const Date operator--(int) {
    Date temp = *this;
    *this -= Days(1);
    return temp;
  }

  operator bool() const { return !isNotADate(); }
  /**
   * To YYYY-mmm-DD string where mmm 3 char month name. ex: 2002-Jan-01
   *
   * @return The string representation of the date
   */
  std::wstring to_simple_string() const { return m_date->to_simple_string(); }
  std::wstring toString() const { return m_date->to_simple_string(); }
  /**
   * To YYYYMMDD where all components are integers. ex: 20020131
   *
   * @return The string representation of the date
   */
  std::wstring to_iso_string() const { return m_date->to_iso_string(); }
  /**
   * To YYYY-MM-DD where all components are integers. Ex: 2002-01-31
   *
   * @return The string representation of the date
   */
  std::wstring to_iso_extended_string() const {
    return m_date->to_iso_extended_string();
  }

  MISC_API std::wstring toString(
      DateFormat format, const std::wstring& separator = L"/") const;
};

class PosInfinityDate : public Date {
 public:
  PosInfinityDate() : Date(DateAbstr::makePosInfinity()) {}
};

class NegInfinityDate : public Date {
 public:
  NegInfinityDate() : Date(DateAbstr::makeNegInfinity()) {}
};

class MinDate : public Date {
 public:
  MinDate() : Date(DateAbstr::makeMinDate()) {}
};

class MaxDate : public Date {
 public:
  MaxDate() : Date(DateAbstr::makeMaxDate()) {}
};

class NotADate : public Date {};

/**
 * Time class
 *
 * A time. The time theoretical resolution is 1 nanosecond, but it depends
 * on the OS and the machine it's running on.
 *
 * A time can be interpreted as a date + time of day
 */
class DateTime {
 private:
  DateTimeAbstrPtr m_date_time;

 protected:
  DateTime(DateTimeAbstrPtr date_time) : m_date_time(date_time) {}

 public:
  /**
   * Default constructor - Creates a DateTime object initialized to
   * not_a_date_time
   */
  DateTime() : m_date_time(DateTimeAbstrPtr(DateTimeAbstr::make())) {}
  /**
   * Constructor that takes a date and a duration as parameters
   *
   * @param date     The date
   * @param duration The duration
   */
  DateTime(const Date& date, const TimeDuration& duration)
      : m_date_time(DateTimeAbstrPtr(
            DateTimeAbstr::make(*date.m_date, *duration.m_duration))) {}

  DateTime(const std::wstring& extendedIso)
      : m_date_time(DateTimeAbstrPtr(
            DateTimeAbstr::makeFromIsoExtendedString(extendedIso))) {}

  /**
   * Constructs a time from a date
   *
   * The time thus constructed will have all the time elements 0
   *
   * @param date
   */
  DateTime(const Date& date)
      : m_date_time(DateTimeAbstrPtr(DateTimeAbstr::make(*date.m_date))) {}

  DateTime(const time_t& time)
      : m_date_time(DateTimeAbstrPtr(DateTimeAbstr::makeFromUnixTime(time))) {}

  /**
   * Copy constructor
   *
   * Constructs an DateTime object from another DateTime
   *
   * @param time   The source DateTime
   */
  DateTime(const DateTime& time)
      : m_date_time(DateTimeAbstrPtr(DateTimeAbstr::make(*(time.m_date_time)))) {}

  /**
   * Operator <
   *
   * Compares two times and returns true if the current time is < than the
   * argument time
   *
   * @param xtime  time to compare to
   * @return true if <, false otherwise
   */
  bool operator<(const DateTime& xtime) const {
    return m_date_time->operator<(*(xtime.m_date_time));
  }
  /**
   * Operator >
   *
   * Compares two times and returns true if the current time is > than the
   * argument time
   *
   * @param xtime  time to compare to
   * @return true if >, false otherwise
   */
  bool operator>(const DateTime& xtime) const {
    return m_date_time->operator>(*(xtime.m_date_time));
  }
  /**
   * Operator >=
   *
   * Compares two times and returns true if the current time is >= than the
   * argument time
   *
   * @param xtime  time to compare to
   * @return true if >=, false otherwise
   */
  bool operator>=(const DateTime& xtime) const {
    return m_date_time->operator>=(*(xtime.m_date_time));
  }
  /**
   * Operator <=
   *
   * Compares two times and returns true if the current time is <= than the
   * argument time
   *
   * @param xtime  time to compare to
   * @return true if <, false otherwise
   */
  bool operator<=(const DateTime& xtime) const {
    return m_date_time->operator<=(*(xtime.m_date_time));
  }
  /**
   * Operator ==
   *
   * Compares two times and returns true if the current time is == to the
   * argument time
   *
   * @param xtime  time to compare to
   * @return true if ==, false otherwise
   */
  bool operator==(const DateTime& xtime) const {
    return m_date_time->operator==(*(xtime.m_date_time));
  }
  /**
   * Operator !=
   *
   * Compares two times and returns true if the current time is != than the
   * argument time
   *
   * @param xtime  time to compare to
   * @return true if !=, false otherwise
   */
  bool operator!=(const DateTime& xtime) const {
    return m_date_time->operator!=(*(xtime.m_date_time));
  }

  /**
   * Returns a simple string representation of the time object
   *
   * The fomat of a simple string: YYYY-mmm-DD HH:MM:SS.fffffffff
   * where mmm 3 char month name. Fractional seconds only included if non-zero
   *
   * Ex: 2002-Jan-01 10:00:01.123456789
   *
   * @return The string representation
   */
  std::wstring to_simple_string() const {
    return m_date_time->to_simple_string();
  }
  std::wstring toString() const { return m_date_time->to_simple_string(); }
  std::wstring to_iso_string() const { return m_date_time->to_iso_string(); }
  __int64 to_epoch_time() const { return m_date_time->to_epoch_time(); }

  /**
   * accesor methods
   */

  /**
   * Gets the date component of the time
   */
  const Date date() const { return Date(m_date_time->date()); }

  /**
   * gets the "time of day" component of the time
   */
  const TimeDuration time_of_day() const {
    return TimeDuration(m_date_time->time_of_day());
  }
  const TimeDuration timeOfDay() const {
    return TimeDuration(m_date_time->time_of_day());
  }

  /**
   * assignment operator
   */
  const DateTime& operator=(const DateTime& time) {
    if (this != &time) m_date_time = DateTimeAbstr::make(*(time.m_date_time));
    return *this;
  }

  /**
   * Indicatas whether the current DateTime is a not_a_date_time
   *
   * A DateTime object constructed using the default constructor will be set to
   * not_a_date_time
   *
   * @return true if it is not a valid DateTime
   */
  bool is_not_a_date_time() const { return m_date_time->is_not_a_date_time(); }
  bool isNotADateTime() const { return m_date_time->is_not_a_date_time(); }
  /**
   * Indicates whether the curerent DateTime is one of positive or negative
   * infinity
   *
   * @return true if it is a positive or negative infinity
   */
  bool is_infinity() const { return m_date_time->is_infinity(); }
  bool isInfinity() const { return m_date_time->is_infinity(); }
  /**
   * Indicates whether the current DateTime is a positive infinity
   *
   * @return true if positive infinity
   */
  bool is_pos_infinity() const { return m_date_time->is_pos_infinity(); }
  bool isPosInfinity() const { return m_date_time->is_pos_infinity(); }
  /**
   * Indicates whether the current DateTime is a negative infinity
   *
   * @return true if negative infinity
   */
  bool is_neg_infinity() const { return m_date_time->is_neg_infinity(); }
  bool isNegInfinity() const { return m_date_time->is_neg_infinity(); }
  /**
   * Indicates whether the current DateTime is either an infinity or not a valid
   * value
   *
   * @return true if infinity (positive or negative) or not a valid value
   */
  bool is_special() const { return m_date_time->is_special(); }
  bool isSpecial() const { return m_date_time->is_special(); }
  /**
   * Subtracts a DateTime from the current DateTime and returns the
   * resulting time duration
   *
   * @param time   The DateTime to substract from the current DateTime
   *
   * @return The resulting TimeDuration
   */
  TimeDuration operator-(const DateTime& time) const {
    return m_date_time->operator-(*(time.m_date_time));
  }
  /**
   * Adds a DateDuration to the current DateTime and returns the resulting
   * DateTime
   *
   * @param dd     The DateDuration to be added to the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator+(const DateDuration& dd) const {
    return m_date_time->operator+(*(dd.m_duration));
  }
  /**
   * Adds a DateDuration to the current DateTime, assigns the result to the
   * current DateTime, and returns the resulting DateTime
   *
   * @param dd     The DateDuration to be added to the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator+=(const DateDuration& dd) {
    return m_date_time->operator+=(*(dd.m_duration));
  }
  /**
   * Subtracts a DateDuration from the current DateTime and returns the
   * resulting DateTime
   *
   * @param dd     The DateDuration to be subtracted from the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator-(const DateDuration& dd) const {
    return m_date_time->operator-(*(dd.m_duration));
  }
  /**
   * Subtracts a DateDuration from the current DateTime, assigns the result to
   * the current DateTime, and returns the resulting DateTime
   *
   * @param dd     The DateDuration to be subtracted from the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator-=(const DateDuration& dd) {
    return m_date_time->operator-=(*(dd.m_duration));
  }
  /**
   * Adds a TimeDuration to the current DateTime and returns the resulting
   * DateTime
   *
   * @param td     The TimeDuration to be added to the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator+(const TimeDuration& td) const {
    return m_date_time->operator+(*(td.m_duration));
  }
  /**
   * Adds a TimeDuration to the current DateTime, assigns the result to the
   * current DateTime, and returns the resulting DateTime
   *
   * @param td     The TimeDuration to be added to the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator+=(const TimeDuration& td) {
    return m_date_time->operator+=(*(td.m_duration));
  }
  /**
   * Subtracts a TimeDuration from the current DateTime and returns the
   * resulting DateTime
   *
   * @param td     The TimeDuration to be subtracted from the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator-(const TimeDuration& td) const {
    return m_date_time->operator-(*(td.m_duration));
  }
  /**
   * Subtracts a TimeDuration from the current DateTime, assigns the result to
   * the current DateTime, and returns the resulting DateTime
   *
   * @param td     The TimeDuration  to be subtracted from the current DateTime
   *
   * @return The resulting DateTime
   */
  DateTime operator-=(const TimeDuration& td) {
    return m_date_time->operator-=(*(td.m_duration));
  }
};

class PosInfinityDateTime : public DateTime {
 public:
  PosInfinityDateTime() : DateTime(DateTimeAbstr::makePosInfinity()) {}
};

class NegInfinityDateTime : public DateTime {
 public:
  NegInfinityDateTime() : DateTime(DateTimeAbstr::makeNegInfinity()) {}
};

class MinDateTime : public DateTime {
 public:
  MinDateTime() : DateTime(DateTimeAbstr::makeMinDateTime()) {}
};

class MaxDateTime : public DateTime {
 public:
  MaxDateTime() : DateTime(DateTimeAbstr::makeMaxDateTime()) {}
};

class NotADateTime : public DateTime {
 public:
  NotADateTime() : DateTime(DateTimeAbstr::makeNotADateTime()) {}
};

class DateTimeFromIsoString : public DateTime {
 public:
  DateTimeFromIsoString(const std::wstring& iso_string)
      : DateTime(DateTimeAbstr::makeFromIsoString(iso_string)) {}
};

class DateTimeFromDelimitedString : public DateTime {
 public:
  DateTimeFromDelimitedString(const std::wstring& delimited_string)
      : DateTime(DateTimeAbstr::makeFromDelimitedString(delimited_string)) {}
};

// YYYYMMDDHHMMSS
class DateTimeFromNonDelimitedString : public DateTime {
 public:
  DateTimeFromNonDelimitedString(const std::wstring& delimited_string)
      : DateTime(DateTimeAbstr::makeFromNonDelimitedString(delimited_string)) {}
};

/**
 * Creates a DateTime containing the local date and time, with second resolution
 * (the fractional part will be 0
 */
class LocalTimeSec : public DateTime {
 public:
  LocalTimeSec() : DateTime(DateTimeAbstr::localTimeSec()) {}
};

/**
 * Creates a DateTime containing the local date and time, with sub-second
 * resolution (the fractional part will be the number of microseconds)
 */
class LocalTimeSubSec : public DateTime {
 public:
  LocalTimeSubSec() : DateTime(DateTimeAbstr::localTimeSubSec()) {}
};

class UniversalTime : public DateTime {
  UniversalTime() : DateTime(DateTimeAbstr::universalTime()) {}
};

using DateTimePtr = std::shared_ptr<DateTime>;

class TimerAbstr {
 public:
  virtual ~TimerAbstr() {}

  virtual void restart() = 0;
  virtual void stop() = 0;
  virtual bool isStopped() const = 0;
  virtual double elapsed() const = 0;

  MISC_API static TimerAbstr* make();
};

/**
 * Implements a timer class
 *
 * Useful for determining the duration of certain events or processes
 *
 * First create a timer object, which also starts the timing, and when desired,
 * call elapsed,
 *
 * @see Time
 * @see Duration
 * @see Date
 */
class Timer {
 private:
  using TimerAbstrPtr = std::shared_ptr<TimerAbstr>;

  TimerAbstrPtr m_timer;

 public:
  /**
   * Default constructor
   *
   * The timer starts at the moment it is created.
   */
  Timer() : m_timer(TimerAbstrPtr(TimerAbstr::make())) {}

  /**
   * Restarts the timer
   */
  void restart() { m_timer->restart(); }
  void stop() { m_timer->stop(); }
  /**
   * Returns the time elapsed since the moment the timer was first created or
   * the last restart.
   *
   * The value returned is a double, representing the number of seconds, with a
   * fractional part representing fractions of a second
   *
   * @return Elapsed time
   */
  double elapsed() const { return m_timer->elapsed(); }
  bool isStopped() const { return m_timer->isStopped(); }
};

/**
 * A pair of DateTime instances
 */
using DateTimePair = std::pair<DateTime, DateTime>;

inline std::wstring timeStamp(bool fracSeconds,
                              TCHAR timeSeparator = _TCHAR(':')) {
  const DateTime time = LocalTimeSec();
  const DateTime timeFrac = LocalTimeSubSec();

  long frac = timeFrac.time_of_day().fractional_seconds();

  std::wostringstream o;

  std::wstring timeStr = time.to_simple_string();

  for (std::wstring::size_type n = 0; n < timeStr.length(); n++) {
    if (timeStr.at(n) == _TCHAR(':'))
      timeStr.replace(n, 1, std::wstring(1, timeSeparator));
  }

  o << timeStr;

  if (fracSeconds) {
    std::wostringstream f;

    f << frac << L"000";

    o << _TCHAR('.') << f.str().substr(0, 3);
  }

  return o.str();
}

//@}
// end Time and date

}  // namespace yloader
