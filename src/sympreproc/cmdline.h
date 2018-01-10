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

class CmdLineException {
 private:
  const std::wstring _message;

 public:
  CmdLineException(const std::wstring& message) : _message(message) {}

  const std::wstring& message() const { return _message; }
};

namespace sympreproc {

class CmdLine {
 private:
  yloader::StrVector _symbolsFileNames;
  std::wstring _outputFileName;
  std::wstring _applicationPath;
  bool _nyseSymbols;
  bool _nasdaqSymbols;
  bool _amexSymbols;
  bool _allowDupes;
  bool _help;
  bool _transform;
  boost::program_options::options_description _generic;

 public:
  CmdLine();
  ~CmdLine(void);

  yloader::StrVector symbolsFilNames() const { return _symbolsFileNames; }
  const std::wstring& outputFileName() const { return _outputFileName; }
  bool nyseSymbols() const { return _nyseSymbols; }
  bool nasdaqSymbols() const { return _nasdaqSymbols; }
  bool amexSymbols() const { return _amexSymbols; }
  bool allowDuplicates() const { return _allowDupes; }
  bool transform() const { return _transform; }
  bool isHelp() const { return _help; }
  void init();
  void help() const;
};

}  // namespace sympreproc
