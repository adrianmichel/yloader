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
using namespace std::string_literals;

using namespace yloader;

class Symbols {
 private:
  StrListPtr symbols;

  bool uniqueSymbolsPopulated;
  UniqueSymbolsSetPtr uniqueSymbols;
  const bool removeDuplicateSymbols;
  const SymbolTransformer* st;

  mutable std::mutex mx;

 protected:
  Symbols(bool removeDuplicateSymbols, const SymbolTransformer* st = 0)
      : removeDuplicateSymbols(removeDuplicateSymbols),
        st(st),
        symbols(std::make_shared< StrList >()),
        uniqueSymbols(std::make_shared< UniqueSymbolsSet >()),
        uniqueSymbolsPopulated(false) {}

  void parse(std::wistream& is) {
    std::wstring str;

    do {
      std::getline(is, str);
      if (!str.empty()) {
        parseLine(str);
      }
    } while (!is.eof());
  }


  template <typename Container> void add(Container container) {
    for (const auto& symbol : container ) {
      // add the symbol only if the symbol didn't exist already in the set, and
      // if the _removeDuplicateSymbols flag is set
      if (!removeDuplicateSymbols || uniqueSymbols->add(symbol)) {
        symbols->push_back((st == 0 ? symbol : (*st)(symbol)));
      }
    }
  }

  void parseLine(const std::wstring& str) {
    // TODO: allow for quoted strings in order to support spaces in names.
    if (str.at(0) == L'#' || str.length() > 1 && str.at(0) == L'/' && str.at(1) == L'/') {
      return;
    }

    Tokenizer tokens(str, L",; \t");

    add(tokens);
  }

 public:
  StrListPtr get() { return symbols; }
  UniqueSymbolsSetPtr getUniqueSymbolsSet() {
    if (!removeDuplicateSymbols && !uniqueSymbolsPopulated) {
      for (const auto& symbol : *symbols) {
        uniqueSymbols->add(symbol);
      }

      uniqueSymbolsPopulated = true;
    }
    return uniqueSymbols;
  }
};

/**
 * Loads a lost of symbols from a text file
 *
 * Symbols separators are: ",; \t" and new line
 */
class FileSymbolsList : public Symbols {
 private:
  bool m_removeDuplicateSymbols;

 public:
  /**
   * Constructor that takes as arguments the file name and a flag indicating
   * whether to eliminate duplicate symbols
   *
   * @param fileName file name
   * @param removeDuplicateSymbols
   *                 Indicates whether to eliminate duplicate symbols
   *
   * @exception FileSymbolsParserException
   */
  FileSymbolsList(const std::wstring& fileName, bool removeDuplicateSymbols, const SymbolTransformer* st = 0)
      : Symbols(removeDuplicateSymbols, st) {
    (*this)(fileName);
  }

  /**
   * Constructor that takes as argument a vector of filenames, and flag
   * indicating whether to remove duplicate symbols
   *
   * @param fileNames vector of file names
   * @param removeDuplicateSymbols
   *                  eliminate duplicate symbols if true
   *
   * @exception FileSymbolsParserException
   */
  FileSymbolsList(const StrVector& fileNames, bool removeDuplicateSymbols, const SymbolTransformer* st = 0, const yloader::StrList* extraSymbols = 0)
      : Symbols(removeDuplicateSymbols, st) {
    for (const auto& fileName : fileNames) {
      (*this)(fileName);
    }

    if (extraSymbols != 0) __super::add(*extraSymbols);
  }

  bool operator()(const std::wstring& fileName) {
    if (!fileName.empty()) {
      std::wifstream file(fileName.c_str());
      if (!file) {
        throw FileSymbolsParserException(L"Could not open symbols file \""s + fileName + L"\"");
      }
      __super::parse(file);
      return true;
    }
    return false;
  }
};

StrListPtr yloader::getSymbols(const std::wstring& fileName, bool removeDuplicateSymbols, const SymbolTransformer* st) {
  // the reason I'm using a deleter function is so the deletion happens in the
  // same module that created the list, or memory errors will occur.
  return FileSymbolsList(fileName, removeDuplicateSymbols, st).get();
}

StrListPtr yloader::getSymbols(const StrVector& fileNames, bool removeDuplicateSymbols, const SymbolTransformer* st, const yloader::StrList* extraSymbols) {
  // the reason I'm using a deleter function is so the deletion happens in the
  // same module that created the list, or memory errors will occur.
  return FileSymbolsList(fileNames, removeDuplicateSymbols, st, extraSymbols).get();
  ;
}

UniqueSymbolsSetPtr yloader::getUniqueSymbols(const StrVector& fileNames) {
  return FileSymbolsList(fileNames, true).getUniqueSymbolsSet();
}

UniqueSymbolsSetPtr yloader::getUniqueSymbols(const std::wstring& file) {
  StrVector files;

  files.push_back(file);

  return getUniqueSymbols(files);
}

UniqueSymbolsSetPtr yloader::getUniqueSymbols(
    const std::wstring file1,
    const std::wstring& file2) {
  StrVector files;

  files.push_back(file1);
  files.push_back(file2);

  return yloader::getUniqueSymbols(files);
}
