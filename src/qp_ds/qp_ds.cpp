/**
* qp_ds.cpp
*
* Copyright (C) 2003-2005 Adrian Michel
* All rights reserved.
* 
* Use, modification and distribution is subject to the SimLib License Agreement
*
* For more information, see http://www.amichel.com/simlib/
*/

#include "stdafx.h"
#include <qp_ds.h>
#include "QPSymbolsList.h"

//#pragma comment( lib, "..\\..\\external\\QpDevKit22\\bin\\qpdll.lib" )


const t_string qpDataSourceName( _TT( "QP data source" ) );

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
  switch ( ul_reason_for_call )
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}

class QPDataSource : public DataSource
{
public:
  virtual ~QPDataSource()
  {
  }

private:
  mutable Mutex _mutex;
  TCHAR cpDataDir[512];   //Quotes Plus Data Directory


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

public:
  Bars* makeBars( const t_string& symbol, const Stock_Record* sr, int size, int count, const Range* range ) const throw ( BarException )
  {
    // put in auto_ptr in case of an exception
    //TODO: determine type/resolution, not hardcoded.
    std::auto_ptr< Bars > bars( simlib::createBars( name(), symbol, Bars::Type::stock, 3600*24, range ) );
    if ( range == 0 )
    {
      for ( int n = size - count; n < size; n++ )
      {
        Date date( sr[ n ].YY, sr[ n ].MM, sr[ n ].DD );
        DateTime pt( date );
        bars->add( Bar( pt, sr[ n ].open, sr[ n ].hi, sr[ n ].lo, sr[ n ].cl, sr[ n ].vol ) );
      }
    }
    else
    {
      for ( int n = size - count; n < size; n++ )
      {
        Date date( sr[ n ].YY, sr[ n ].MM, sr[ n ].DD );
        DateTime pt( date );
        Bar bar( pt,  sr[ n ].open, sr[ n ].hi, sr[ n ].lo, sr[ n ].cl, sr[ n ].vol );
        if ( *range > bar )
          continue;
        else if ( *range < bar )
          break;
        else
          bars->add( bar );
      }
    }

    // no exceptions, so get the pointer, and release (so won't get deleted)
    return bars.release();
  }

  QPDataSource( const t_string& name ) : DataSource( qpDataSourceName )
  {
    HKEY hKey;          //Handle to Quotes Plus Setup Key

    DWORD dwType, dwSize;   //Registry value type and size

    //----------Get the data directory from the registry------
    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, _TT("Software\\Quotes Plus\\Setup" ), 0, KEY_ALL_ACCESS, &hKey ) != ERROR_SUCCESS )
			throw DataSourceException( DATA_SOURCE_ERROR, _TT( "Could not open registry key \"Software\\Quotes Plus\\Setup\"" ), DataSource::name() );
    if ( RegQueryValueEx( hKey, _TT( "DataDir" ), 0, &dwType, 0, &dwSize ) != ERROR_SUCCESS )
    {
      RegCloseKey( hKey );
      throw DataSourceException( DATA_SOURCE_ERROR, _TT( "Could not get registry value \"Software\\Quotes Plus\\Setup\\DataDir\"" ), DataSource::name() );
    }
    if ( RegQueryValueEx( hKey, _TT( "DataDir" ), 0, &dwType, (LPBYTE)cpDataDir, &dwSize ) != ERROR_SUCCESS )
    {
      RegCloseKey( hKey );
      throw DataSourceException( DATA_SOURCE_ERROR, _TT( "Could not get registry value \"Software\\Quotes Plus\\Setup\\DataDir\"" ), DataSource::name() );
    }

    RegCloseKey( hKey );
  }

  void QPDataSource::init() const throw (DataSourceException)
  {
    Lock lock( _mutex );

    //Initialize Quotes Plus Dll
    if ( R2_InitReadStock( cpDataDir, TRUE, NULL, FALSE ) != 0 )
      throw DataSourceException( DATA_SOURCE_ERROR, _TT( "Could not initialize Quotes Plus Dll" ), name() );

    //------------Open Master Files and Zacks Data Files----------
    if ( R2_Open_Files( ALL_DATA_FILES, cpDataDir ) != 0 )
    {
      R2_Done();
      throw DataSourceException( DATA_SOURCE_ERROR, _TT( "Could not open Quotes Plus data files" ), name() );
    }
/*  if ( R2_Open_Files( MASTER_FILE, cpDataDir ) != 0 )
  {
    R2_Done();
    return false;
  }
*/  /*
  if ( R2_Open_Files( ZACKS_DATA, cpDataDir ) != 0 )
  {
    R2_Close_Files( MASTER_FILE );
    R2_Done();
    return false;
  }
  if ( R2_Open_Files( EPS_RANK_TABLE, cpDataDir ) != 0 )
  {
    R2_Close_Files( ZACKS_DATA );
    R2_Close_Files( MASTER_FILE );
    R2_Done();
    return false;
  }
  if ( R2_Open_Files( NAME_CHG_FILE, cpDataDir ) != 0 )
  {
    R2_Close_Files( EPS_RANK_TABLE );
    R2_Close_Files( ZACKS_DATA );
    R2_Close_Files( MASTER_FILE );
    R2_Done();
    return false;
  }
  if ( R2_Open_Files( ALL_DATA_FILES, cpDataDir ) != 0 )
  {
    R2_Close_Files( NAME_CHG_FILE );
    R2_Close_Files( EPS_RANK_TABLE );
    R2_Close_Files( ZACKS_DATA );
    R2_Close_Files( MASTER_FILE );
    R2_Done();
    return false;
  }
  */

  }

  void uninit() const
  {
    Lock lock( _mutex );
    R2_Close_Files( ALL_DATA_FILES );
//  R2_Close_Files( MASTER_FILE );

    R2_Done();
  }

  DataX* getData( const SymbolInfo *si, const Range* range ) const throw (DataSourceException)
  {
    try
    {
      Lock lock( _mutex );
      init();
      int iCount;
      // get at most ~10k bars
      Stock_Record data[10000];
      int recsToRead = 9000;
      {
        TCHAR symbol[ 20 ];
        _tcscpy( symbol, si -> symbol().c_str() );
        _tcsupr( symbol );
        if ( false )
        {
          t_ostringstream o;
          o << _TT( "Before load data for : " ) << symbol << std::endl;
          tsprint( o, std::cout );
        }
        iCount = R2_QP_LoadSymbol( symbol,
                                   data,     // Pointer to Price data record(s)
                                   recsToRead,   // Number of records/days to get(NOTE:lpStockRec must point to a valid memory block that can store max_rec * sizeof( Stock_Record) bytes of data,
                                   FALSE,    // Load Unadjusted data,
                                   TRUE,     // Do not  return days that are holidays,
                                   0       // Pointer to Master_Rec
                                 );
        if ( false )
        {
          t_ostringstream   o;
          o << _TT( "After load data: " ) << iCount << std::endl;
          tsprint( o, std::cout );
        }
      }

      uninit();
			// TODO: for the moment no stamp for QP - add real stamp
      return new DataX( makeBars( si -> symbol(), data, 9000, iCount, range ), _TT( "" ) );
    }
    catch( const BarException& e )
    {
      throw DataSourceException( DATA_SOURCE_ERROR, _TT( "Data error on symbol: " ) + si->symbol() + e.message(), name() );
    }
  }

	virtual bool isConsistent( const t_string& stamp, const SymbolInfo* si, const Range* range = 0 ) const throw (DataSourceException)
	{
		return true;
	}

};

QP_DS_API DataSource* simlib::createQPDataSource()
{
  return new QPDataSource( qpDataSourceName );
}

QP_DS_API SymbolsInfoSource* simlib::createQPSymbolsInfo( const t_string& dataSourceName, QPSymbolsInfoPredicate& predicate )
{
  return new QPSymbolsInfoSource( dataSourceName, predicate );
}