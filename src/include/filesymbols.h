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

///
/// @cond
///
namespace yloader {
///
/// @endcond
///

/**
 * Template class - represents a class (generally a collection) to which we can
 * add elements
 */
template <class T>
class Addable {
 public:
  virtual ~Addable() {}

  virtual void add(const T& t) = 0;
};

class FileSymbolsParserException {
 private:
  const std::wstring m_message;

 public:
  FileSymbolsParserException(const std::wstring& message) : m_message(message) {}

  const std::wstring& message() const { return m_message; }
};

class SymbolTransformer {
 public:
  virtual ~SymbolTransformer() {}

  virtual std::wstring operator()(const std::wstring& symbol) const = 0;
};

using UniqueSymbolsSetBase = std::set<std::wstring>;

class UniqueSymbolsSet : public UniqueSymbolsSetBase {
 private:
  std::mutex m_mx;

 public:
  bool add(const std::wstring& symbol) {
    return __super::insert(yloader::to_lower_case(symbol)).second;
  }

  bool hasSymbol(const std::wstring& symbol) const {
    return __super::find(yloader::to_lower_case(symbol)) != __super::end();
  }

  const UniqueSymbolsSet& operator+=(const UniqueSymbolsSet& symbols) {
    __super::insert(symbols.begin(), symbols.end());

    return *this;
  }
};

using UniqueSymbolsSetPtr = std::shared_ptr<UniqueSymbolsSet>;

StrListPtr getSymbols(const std::wstring& fileName, bool removeDuplcateSymbols = false, const SymbolTransformer* st = 0);
StrListPtr getSymbols( const StrVector& fileNames, bool removeDuplcateSymbols = false, const SymbolTransformer* st = 0,
    const yloader::StrList* extraSymbols = 0);

UniqueSymbolsSetPtr getUniqueSymbols(const StrVector& fileNames);
UniqueSymbolsSetPtr getUniqueSymbols(const std::wstring& file);
UniqueSymbolsSetPtr getUniqueSymbols(const std::wstring file1, const std::wstring& file2);

}  // namespace yloader
