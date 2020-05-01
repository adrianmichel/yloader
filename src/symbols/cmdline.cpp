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

namespace po = boost::program_options;

#include ".\cmdline.h"

using namespace sympreproc;

constexpr auto HELP = "help,?";
constexpr auto SYMBOLS_FILE = "symbolsfile";
constexpr auto OUTPUT_FILE = "output,o";
constexpr auto NYSE_SYMBOLS = "nyse,n";
constexpr auto NASDAQ_SYMBOLS = "nasdaq,q";
constexpr auto AMEX_SYMBOLS = "amex,a";
constexpr auto IGNORE_DUPLICATE_SYMBOLS = "allowdupes,d";
constexpr auto DO_NOT_TRANSFORM_SYMBOLS = "notransform,t";

void sympreproc::CmdLine::init() {
  try {
    std::vector<std::wstring> def;
    // Declare a group of options that will be
    // allowed only on command line

    m_generic.add_options()(HELP, "shows this help message")
      (SYMBOLS_FILE, po::value<std::vector<std::string> >()->multitoken(),"input file(s) containing symbols")
      (OUTPUT_FILE, po::value<std::string>(),"specifies the output file that will contain the resulting symbols")
      (NYSE_SYMBOLS, "optional, if present will download NYSE symbols from www.nasdaq.com")
      (NASDAQ_SYMBOLS, "optional, if present will download NASDAQ symbols from www.nasdaq.com")
      (AMEX_SYMBOLS, "optional, if present will download AMEX symbols from www.nasdaq.com")
      (IGNORE_DUPLICATE_SYMBOLS, "optional, if present duplicates symbols will be eliminated from the final symbols list")
      (DO_NOT_TRANSFORM_SYMBOLS, "optionsl, if present will prevent downloaded symbols from being transformed");

    po::positional_options_description p;
    p.add(SYMBOLS_FILE, -1);

    std::vector<std::string> args = yloader::cmdLineSplitter(::GetCommandLine());

    // GetCommandLine also returns the full application path so remove it
    CFileFind fileFinder;

    for (std::vector<std::string>::iterator i = args.begin(); i != args.end(); i++) {
      // remove the arg with the tool name in it if any
      std::wstring::size_type index = yloader::to_lower_case(yloader::s2ws(*i))
              .find(yloader::to_lower_case((LPCTSTR)fileFinder.GetFileTitle()));
      if (index != std::wstring::npos) {
        args.erase(i);
        break;
      }
    }

    for (const auto& arg : args) {
      std::wstring x(yloader::s2ws(arg));
    }

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(m_generic).positional(p).run(), vm);

    if (vm.count("help") > 0) {
      help();
      m_help = true;
    }
    else {
      if (vm.count(SYMBOLS_FILE) > 0) {
        m_symbolsFileNames = vm[SYMBOLS_FILE].as<std::vector<std::wstring> >();
      }
      if (vm.count("output") > 0) {
        m_outputFileName = yloader::s2ws(vm["output"].as<std::string>());
      }
      else {
        throw CmdLineException(L"No output file specified");
      }

      m_nyseSymbols = vm.count("nyse") > 0;
      m_nasdaqSymbols = vm.count("nasdaq") > 0;
      m_amexSymbols = vm.count("amex") > 0;
      m_allowDupes = vm.count("allowdupes") > 0;
      m_transform = !(vm.count("notransform") > 0);
    }
  }
  catch (const std::exception& e) {
    throw CmdLineException(yloader::s2ws(e.what()));
  }
}

sympreproc::CmdLine::CmdLine()
    : m_help(false), m_generic("Command line options"), m_transform(true) {}

void sympreproc::CmdLine::help() const { std::cout << m_generic << std::endl; }

sympreproc::CmdLine::~CmdLine() {}