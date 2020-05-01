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

class CmdLineException {
 private:
  const std::wstring m_message;

 public:
  CmdLineException(const std::wstring& message) : m_message(message) {}

  const std::wstring& message() const { return m_message; }
};

namespace sympreproc {

class CmdLine {
 private:
  yloader::StrVector m_symbolsFileNames;
  std::wstring m_outputFileName;
  std::wstring m_applicationPath;
  bool m_nyseSymbols;
  bool m_nasdaqSymbols;
  bool m_amexSymbols;
  bool m_allowDupes;
  bool m_help;
  bool m_transform;
  boost::program_options::options_description m_generic;

 public:
  CmdLine();
  ~CmdLine(void);

  yloader::StrVector symbolsFilNames() const { return m_symbolsFileNames; }
  const std::wstring& outputFileName() const { return m_outputFileName; }
  bool nyseSymbols() const { return m_nyseSymbols; }
  bool nasdaqSymbols() const { return m_nasdaqSymbols; }
  bool amexSymbols() const { return m_amexSymbols; }
  bool allowDuplicates() const { return m_allowDupes; }
  bool transform() const { return m_transform; }
  bool isHelp() const { return m_help; }
  void init();
  void help() const;
};

}  // namespace sympreproc
