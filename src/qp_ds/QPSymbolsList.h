/**
* qpsymbolslist.cpp
*
* Copyright (C) 2003-2005 Adrian Michel
* All rights reserved.
* 
* Use, modification and distribution is subject to the SimLib License Agreement
*
* For more information, see http://www.amichel.com/simlib/
*/

#pragma once

using namespace simlib;

class QPBarSymbolInfo : public BarSymbolInfo
{
public:
  QPBarSymbolInfo( const t_string& symbol, unsigned long resolution, const t_string& dataSourceName )
    : BarSymbolInfo( symbol, resolution, dataSourceName, makeId( symbol, resolution, dataSourceName ) )
  {
  }

private:
  const t_string makeId( const t_string& symbol, unsigned long resolution, const t_string& dataSourceName) const
  {
    t_ostringstream o;

    o << _TT( "qp bars, " ) << resolution << symbol << dataSourceName;
    return o.str();
  }
};

class QPSymbolsInfoSource : public BarsSymbolsInfoSource
{
private:
  TCHAR cpDataDir[512];   //Quotes Plus Data Directory
	const t_string _dataSourceName;

public:
  QPSymbolsInfoSource( const t_string& dataSourceName, QPSymbolsInfoPredicate& predicate );
  virtual ~QPSymbolsInfoSource();

  virtual const SymbolInfo* makeSymbolInfo( const_iterator& i ) const;

	const t_string& getDataSourceName() const
	{
		return _dataSourceName;
	}

private:
  QPSymbolDescription::Exchange convertExchange( const char* exchange ) throw ( SymbolsInfoException );

};
