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


#include "stdafx.h"
#include <usrdll22.h>
#include <qp_ds.h>
#include "QPSymbolsList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QPSymbolsInfoSource::QPSymbolsInfoSource( const t_string& dataSourceName, QPSymbolsInfoPredicate& predicate )
: BarsSymbolsInfoSource( SymbolInfo::Type::BARS ), _dataSourceName( dataSourceName )
{
  HKEY hKey;          //Handle to Quotes Plus Setup Key
  DWORD dwType, dwSize;   //Registry value type and size
  //**********
  // init phase

  //----------Get the data directory from the registry------
  if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, _TT("Software\\Quotes Plus\\Setup" ), 0, KEY_ALL_ACCESS, &hKey ) != ERROR_SUCCESS )
    throw SymbolsInfoException( _TT( "Could not open registry key \"Software\\Quotes Plus\\Setup\"" ) );
  if ( RegQueryValueEx( hKey, _TT( "DataDir" ), 0, &dwType, 0, &dwSize ) != ERROR_SUCCESS )
  {
    RegCloseKey( hKey );
    throw SymbolsInfoException( _TT( "Could not get registry value \"Software\\Quotes Plus\\Setup\\DataDir\"" ) );
  }
  if ( RegQueryValueEx( hKey, _TT( "DataDir" ), 0, &dwType, (LPBYTE)cpDataDir, &dwSize ) != ERROR_SUCCESS )
  {
    RegCloseKey( hKey );
    throw SymbolsInfoException( _TT( "Could not get registry value \"Software\\Quotes Plus\\Setup\\DataDir\"" ) );
  }

  RegCloseKey( hKey );

  //Initialize Quotes Plus Dll
  if ( R2_InitReadStock( cpDataDir, TRUE, NULL, FALSE ) != 0 )
    throw SymbolsInfoException( _TT( "Could not initialize Quotes Plus Dll" ) );

  //------------Open the master file----------
  if ( R2_Open_Files( MASTER_FILE, cpDataDir ) != 0 )
  {
    R2_Done();
    throw SymbolsInfoException( _TT( "Could not open Quotes Plus data files" ) );
  }

  //******************
  // getting the symbols

  char cpSymbol[8];     //Ticker Symbol
  Master_Rec rec;


  int iError = R2_QP_ReadMaster( QP_TOP, QP_SYMBOL, "", &rec );
  while ( iError == 0 )
  {

    memcpy( cpSymbol, rec.symbol, sizeof( rec.symbol ) );
    cpSymbol[sizeof( rec.symbol )] = 0; //NULL Terminate

    // QP returns fixed size symbols (7 chars or so), with blancs at the end,
    // so have to remove the spaces at the end, or the symbol will not be recognized
    // later on
    t_string sx( cpSymbol );
    t_string::size_type s = sx.find_last_not_of( _TT( " " ) );

    t_string symbol( sx.substr( 0, ++s ) );

    // TODO: there is no way to see if a symbol is active or not directly. The solution suggested
    // by Gary Lyben at QP was to read a few bars and see if there is data. Return true for now,
    // so all symbols are considered to be active, but the ones that are not, will not get data
    QPSymbolDescription qpsd( symbol, convertExchange( rec.exchange ), true );
    // if the predicate allows us, process the new symbol
    if( predicate.onSymbol( qpsd ) )
    {

        // add the new symbol to the symbols list
      push_back( symbol );
      //*********************Read in zacks data***********************
    }
    iError = R2_QP_ReadMaster( QP_NEXT, QP_SYMBOL, "", &rec );
  }

  //******************
  // un-init phase
  R2_Close_Files( MASTER_FILE );
  R2_Done();
}

QPSymbolsInfoSource::~QPSymbolsInfoSource()
{
}

const SymbolInfo* QPSymbolsInfoSource::makeSymbolInfo( const_iterator& i ) const
{
  return new QPBarSymbolInfo( *i, defResolution(), getDataSourceName() ); 
}


QPSymbolDescription::Exchange QPSymbolsInfoSource::convertExchange( const char* exchange ) throw ( SymbolsInfoException )
{

  /*here are the different codes in Master_Rec.exchange (a 2 characters code)
  - "00", Index
  - "01" , NY Stock Exchange
  - "02" , American Stock Exchange
  - "60" , NASDAQ
  - "65" , OTC Bulletin Board
  - "40" , Mutual Fund
  */
  if ( strnicmp( exchange , "00" , 2 ) == 0 ) return QPSymbolDescription::Exchange::index;
  else if ( strnicmp( exchange , "01" , 2 ) == 0 ) return QPSymbolDescription::Exchange::nyse;
  else if ( strnicmp( exchange , "02" , 2 ) == 0 ) return QPSymbolDescription::Exchange::amex;
  else if ( strnicmp( exchange , "60" , 2 ) == 0 ) return QPSymbolDescription::Exchange::nasdaq;
  else if ( strnicmp( exchange , "65" , 2 ) == 0 ) return QPSymbolDescription::Exchange::otc;
  else if ( strnicmp( exchange , "40" , 2 ) == 0 ) return QPSymbolDescription::Exchange::mutual_fund;
  else
    return QPSymbolDescription::Exchange::other;
}





