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

#include "stdafx.h"

using namespace yloader;

#include <fstream>
#include "cmdline.h"
#include "httprequest.h"

#include <boost\regex.hpp>

class NasdaqSymbolsList : public yloader::StrList {
 private:
  static const std::wstring nasdaqUrl;
  static const std::wstring nyseUrl;
  static const std::wstring amexUrl;

 public:
  // nasdaq:
  // http://www.nasdaq.com/screening/companies-by-name.aspx?letter=&exchange=amex&render=download
  // nyse:
  // http://www.nasdaq.com/screening/companies-by-name.aspx?letter=&exchange=nyse&render=download
  // amex:
  // http://www.nasdaq.com/screening/companies-by-name.aspx?letter=&exchange=amex&render=download

  NasdaqSymbolsList(bool nyseSymbols, bool nasdaqSymbols, bool amexSymbols,
                    bool transform) {
    CurlHTTPRequest req(L"www.nasdaq.com");
    if (nasdaqSymbols) {
      std::cout << "Downloading NASDAQ symbols..." << std::endl;
      parse(req.get(nasdaqUrl), transform);
    }
    if (amexSymbols) {
      std::cout << "Downloading AMEX symbols..." << std::endl;
      parse(req.get(amexUrl), transform);
    }

    if (nyseSymbols) {
      std::cout << "Downloading NYSE symbols..." << std::endl;
      parse(req.get(nyseUrl), transform);
    }
  }

  void parse(const std::shared_ptr<std::string> str, bool transform) {
    if (!str) {
      return;
    }

    boost::regex expression("^\"([^\"]*)\"");

    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    std::string::const_iterator start = str->begin();
    std::string::const_iterator end = str->end();
    unsigned long count = 0;
    while (boost::regex_search(start, end, what, expression, flags)) {
      if (count++ >= 2) {
        std::string symbol(what[1].first, what[1].second);

        if (transform) {
          std::string::size_type n = symbol.find("/");
          if (n != std::wstring::npos) {
            symbol.replace(n, 1, "-");
          }

          n = symbol.find("^");
          if (n != std::wstring::npos) {
            symbol.replace(n, 1, "-P");
          }

          n = symbol.find(":");
          if (n != std::wstring::npos) {
            symbol.replace(n, 1, "-P");
          }
        }

        __super::push_back(s2ws(symbol));
      }
      // update search position:
      start = what[0].second;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
  }
};

const std::wstring NasdaqSymbolsList::nasdaqUrl = L"/screening/companies-by-industry.aspx?exchange=NASDAQ&render=download";
const std::wstring NasdaqSymbolsList::nyseUrl = L"/screening/companies-by-industry.aspx?exchange=NYSE&render=download";
const std::wstring NasdaqSymbolsList::amexUrl = L"/screening/companies-by-industry.aspx?exchange=AMEX&render=download";

class SymbolsFileWriterException {};

class SymbolsFileWriter {
  mutable std::wofstream m_ofs;

 public:
  SymbolsFileWriter(const std::wstring& fileName, StrListPtr symbols)
      : m_ofs(fileName.c_str()) {
    if (symbols) {
      std::wcout << L"Creating output symbols file: " << fileName << std::endl;
      if (!m_ofs){
        throw SymbolsFileWriterException();
      }

      for (const auto& symbol : *symbols) {
        m_ofs << symbol << L" ";
      }
    }
  }
};

int _tmain(int argc, _TCHAR* argv[]) {
  sympreproc::CmdLine cmdLine;

  try {
    printDashedLine(yloader::getCaption(L"Symbols"));
    printLine();

    cmdLine.init();

    if (cmdLine.isHelp()) return 0;

    NasdaqSymbolsList nsl(cmdLine.nyseSymbols(), cmdLine.nasdaqSymbols(),
                          cmdLine.amexSymbols(), cmdLine.transform());
    StrListPtr sym(::getSymbols(cmdLine.symbolsFilNames(), !cmdLine.allowDuplicates(), 0, &nsl));

    if (sym && sym->size() > 0) {
      SymbolsFileWriter sfw(cmdLine.outputFileName(), sym);
      std::cout << "Symbols file generation complete" << std::endl;
    }
    else {
      std::cout << "No input symbols, nothing to do" << std::endl;
    }

    return 0;
  }
  catch (const InternetException&) {
    std::cout << "Symbols downloading error" << std::endl;
  }
  catch (const FileSymbolsParserException&) {
    std::cout << "Input symbols file error" << std::endl;
  }
  catch (const SymbolsFileWriterException&) {
    std::cout << "Output symbols file error" << std::endl;
  }
  catch (const CmdLineException& e) {
    std::wcout << e.message() << std::endl;
    cmdLine.help();
  }
}
