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

#include "misc.h"

using namespace std::string_literals;


namespace yloader
{
/**
 * Defines various error codes
 *
 * Each exception has an error code, indicating the nature of the error to the
 * calling application.
 */
enum ErrorCode {
	PLUGIN_ERROR,
	BAR_ERROR
};

/**
 * Base class for all YLoader exceptions.
 *
 * Provides basic functionality that must be supported by all exceptions
 * - error code
 * - message
 */
class YLoaderException {
 private:
  const ErrorCode m_code;
  std::wstring m_message;

 protected:
  /**
   * Sets the exception message
   *
   * The message is received as a string
   *
   * @param str    THe message string
   */
  void setMessage(const std::wstring& str) { m_message = str; }

  /**
   * Sets the exception message
   *
   * The message is received as a std::wostringstream.
   *
   * @param str    The string stream containing the message
   * @see std::ostringstream
   * @see std::wostringstream
   */
  void setMessage(const std::wostringstream& str) { m_message = str.str(); }

 public:
  /**
   * Constructor that takes an error code as only parameter
   *
   * @param code   Error code
   * @return
   * @see ErrorCode
   */
  explicit YLoaderException(ErrorCode code) : m_code(code) {}

  /**
   * Constructor that takes an error code and a pointer to a string as
   * parameters
   *
   * @param code    error code
   * @param message message
   * @return
   * @see ErrorCode
   */
  explicit YLoaderException(ErrorCode code, const TCHAR* message)
      : m_code(code), m_message(message) {}

  /**
   * Consructor that takes an error code and a reference to a std::wstring as
   * parameters
   *
   * @param code    error code
   * @param message message
   * @return
   * @see ErrorCode
   * @see std::wstring
   */
  explicit YLoaderException(ErrorCode code, const std::wstring& message)
      : m_code(code), m_message(message) {}

  virtual ~YLoaderException() {}

  /**
   * Returns the exception message
   *
   * @return message
   */
  const std::wstring& message() const { return m_message; }

  /**
   * Returns the error code of the exception
   *
   * @return Error code
   * @see ErrorCode
   */
  ErrorCode code() const { return m_code; }
};


/**
 * Thrown if a bar is not well formed, i.e. the low is higher than the high or
 * similar
 *
 * @see Bar
 * @see YLoaderException
 */
class BarException : public YLoaderException {
 public:
  /**
   * Constructor taking all the bar info as parameters
   *
   * @param time   The date and time of the bar
   * @param open   Open price
   * @param low    Low price
   * @param high   High price
   * @param close  Close price
   */
  BarException(const std::wstring& message)
      : YLoaderException(BAR_ERROR, message) {}
};


class PluginException : public YLoaderException {
  const std::wstring m_name;

 public:
   PluginException()
     : PluginException(L""s, L""s){}

   PluginException(const std::wstring& name, const std::wstring& message)
      : YLoaderException(PLUGIN_ERROR, message), m_name(name) {}

   const std::wstring& name() const { return m_name; }
};


}  // namespace yloader
