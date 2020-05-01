// ArmadilloStuff.h: interface for ArmAccess.dll.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>   // string functions
#include <boost/format.hpp>
#include <strings.h>
#include "buildtimestamp.h"

#include <log.h>



// #define's for secured sections/nanomites

#define SECUREBEGIN   __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0x00
#define SECUREEND     __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0xFF
#define SECUREBEGIN_A __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0x01
#define SECUREEND_A   __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0xFF
#define SECUREBEGIN_B __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0x02
#define SECUREEND_B   __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0xFF
#define SECUREBEGIN_C __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0x03
#define SECUREEND_C   __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD6 __asm _emit 0xFF
#define NANOBEGIN     __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x01
#define NANOEND       __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x00

// These typedefs show the compiler the formats of the functions  

typedef bool (__stdcall *CheckCodeFn)(const char *name, const char *code);  

typedef bool (__stdcall *VerifyKeyFn)(const char *name, const char *code);  

typedef bool (__stdcall *InstallKeyFn)(const char *name, const char *code);  

typedef bool (__stdcall *InstallKeyLaterFn)(const char *name, const char *code);  

typedef bool (__stdcall *UninstallKeyFn)(void);  

typedef bool (__stdcall *SetDefaultKeyFn)(void);  

typedef bool (__stdcall *UpdateEnvironmentFn)(void);  

typedef bool (__stdcall *IncrementCounterFn)(void);  

typedef int  (__stdcall *CopiesRunningFn)(void);  

typedef bool (__stdcall *ChangeHardwareLockFn)(void);  

typedef DWORD (__stdcall *GetShellProcessIDFn)(void);  

typedef bool (__stdcall *FixClockFn)(const char *fixclockkey);  

typedef DWORD (__stdcall *RawFingerprintInfoFn)(DWORD item);  

typedef bool (__stdcall *SetUserStringFn)(int which, const char *string);  

typedef DWORD (__stdcall *GetUserStringFn)(int which, char *buffer, DWORD bufferlength);  

typedef bool (__stdcall *WriteHardwareChangeLogFn)(const char *filename);  

typedef bool (__stdcall *ConnectedToServerFn)(void);  

typedef bool (__stdcall *CallBuyNowURLFn)(HWND parent);  

typedef bool (__stdcall *ExpireCurrentKeyFn)(void);

typedef bool (__stdcall *ShowEnterKeyDialogFn)( HWND parent );

// the armadillo namespace
namespace armadillo
{

// Wrapper for an armadillo DLL function
// It handles library loading/freeing
class CArmadilloFunction
{
  HINSTANCE      m_armadillo_library;
  FARPROC         m_function;
public:

  CArmadilloFunction( const TCHAR *proc_name )
     : m_armadillo_library(NULL), m_function(NULL)
  {
     m_armadillo_library = ::LoadLibrary( _T( "ArmAccess.DLL" ) );
     if ( ! m_armadillo_library )
        return;

     m_function = ::GetProcAddress( m_armadillo_library, tradery::ws2s( proc_name ) );
  }
  ~CArmadilloFunction()
  {
     if ( m_armadillo_library ) ::FreeLibrary( m_armadillo_library );
  }
  FARPROC get_function() { return m_function; }
  bool is_loaded() { return m_function != NULL; }

};


// =============
// Here are the wrappers for the functions exported by the ArmAccess.dll
// NOTE: Descriptions in comments lifted verbatim from Armadillo Help files
// =============


// Function name   : install_key_later
// Description       : This function, like VerifyKey (below), checks whether a key 
//      is good and returns non-zero (and sets environment variables) if it is. 
//      It doesn't install the key until that copy of the program exits though.  
// Return type      : inline bool 
// Argument         : const std::string &name
// Argument         : const std::string &key
inline bool install_key_later(const std::string &name, const std::string &key)
{
  CArmadilloFunction arm_lib( _T("InstallKeyLater" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  InstallKeyLaterFn arm_fun = (InstallKeyLaterFn)arm_lib.get_function();

  return arm_fun( name.c_str(), key.c_str() );

}

// Function name   : verify_key
// Description       : This function is similar to CheckCode/InstallKey. It checks 
//      a name/key pair, returning non-zero if it's valid and could be installed. 
//      However, unlike CheckCode/InstallKey, it does not install the key; the 
//      original key remains installed. After calling VerifyKey, the environment 
//      variables are set as if the key were installed; to recover the environment 
//      variables for the original key, call UpdateEnvironment.
// Return type      : inline bool 
// Argument         : const std::string &name
// Argument         : const std::string &key
inline bool verify_key(const std::string &name, const std::string &key)
{
  CArmadilloFunction arm_lib( _T("VerifyKey" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  VerifyKeyFn arm_fun = (VerifyKeyFn)arm_lib.get_function();

  return arm_fun( name.c_str(), key.c_str() );

}

// Function name   : check_code
// Description       : This is identical to InstallKey. You can use either name, 
//      but we recommend InstallKey, this one is only here for historical purposes.
// Return type      : inline bool 
// Argument         : const std::string &name
// Argument         : const std::string &key
inline bool check_code(const std::string &name, const std::string &key)
{
  CArmadilloFunction arm_lib( _T("CheckCode" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  CheckCodeFn arm_fun = (CheckCodeFn)arm_lib.get_function();

  return arm_fun( name.c_str(), key.c_str() );

}

// Function name   : install_key
// Description       : Checks a name/key pair, and (if valid and not already expired)
//      stores it as the new key. Returns non-zero if the code is valid. Note: an 
//      expired key is still valid! You must check the EXPIRED environment variable, 
//      or the value returned by the GetLastError() function, to see if it's 
//      expired or not.
// Return type      : inline bool 
// Argument         : const std::string &name
// Argument         : const std::string &key
inline bool install_key(const std::string &name, const std::string &key)
{
  CArmadilloFunction arm_lib( _T("InstallKey" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  InstallKeyFn arm_fun = (InstallKeyFn)arm_lib.get_function();

  return arm_fun( name.c_str(), key.c_str() );

}

// Function name   : update_environment
// Description       : You can call this function to have Armadillo update the 
//      environment strings it sets.
// Return type      : inline bool 
inline bool update_environment()
{
  CArmadilloFunction arm_lib( _T("UpdateEnvironment" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  UpdateEnvironmentFn arm_fun = (UpdateEnvironmentFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : uninstall_key
// Description       : Securely uninstalls a hardware-locked key, and changes the 
//      hardware "fingerprint" so that it can't be reinstalled. It also gives you 
//      an uninstall code (in the otherwise-undocumented environment variable 
//      UNINSTALLCODE), so that you can manually verify that the key has been 
//      uninstalled.
// Return type      : inline bool 
inline bool uninstall_key()
{
  CArmadilloFunction arm_lib( _T("UninstallKey" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  UninstallKeyFn arm_fun = (UninstallKeyFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : set_default_key
// Description       : Calling this function will force the Armadillo shell to 
//      revert to the default certificate (if any) for your program. Note: if your 
//      ogram is already running on the default certificate, calling this function 
//      will replace the existing key, effectively resetting the number of 
//      days/uses the user has to test your program!
// Return type      : inline bool 
inline bool set_default_key()
{
  CArmadilloFunction arm_lib( _T("SetDefaultKey" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  SetDefaultKeyFn arm_fun = (SetDefaultKeyFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : increment_counter
// Description       : Calling this function will force the Armadillo shell to 
//      revert to the default certificate (if any) for your program. Note: if your 
//      program is already running on the default certificate, calling this 
//      function will replace the existing key, effectively resetting the number 
//      of days/uses the user has to test your program!
// Return type      : inline bool 
inline bool increment_counter()
{
  CArmadilloFunction arm_lib( _T("IncrementCounter" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  IncrementCounterFn arm_fun = (IncrementCounterFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : copies_running
// Description       : Mainly used with network licenses, this function returns the 
//      number of copies running on the network at the time it is called (including
//      the calling copy). Returns zero if there is an error. Note that this 
//      function only reports the number of copies that are using the same key as 
//      the one that makes the request.
// Return type      : inline int 
inline int copies_running()
{
  CArmadilloFunction arm_lib( _T("CopiesRunning" ) );

  if ( arm_lib.is_loaded( ) == false ) return 0;

  CopiesRunningFn arm_fun = (CopiesRunningFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : change_hardware_lock
// Description       : Used only with hardware-locked keys, this function randomly 
//      changes the hardware "fingerprint" for a machine, just as if the TRANSFER 
//      command were used.
// Return type      : inline bool 
inline bool change_hardware_lock()
{
  CArmadilloFunction arm_lib( _T("ChangeHardwareLock" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  ChangeHardwareLockFn arm_fun = (ChangeHardwareLockFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : get_shell_process_id
// Description       : Returns the Process ID of the Armadillo shell around your 
//      program. Please note that, as of Armadillo version 2.50, this is redundant;
//      the process ID of the shell is identical to the process ID of your program.
// Return type      : inline DWORD 
inline DWORD get_shell_process_id()
{
  CArmadilloFunction arm_lib( _T("GetShellProcessID" ) );

  if ( arm_lib.is_loaded( ) == false ) return 0;

  GetShellProcessIDFn arm_fun = (GetShellProcessIDFn)arm_lib.get_function();

  return arm_fun();

}

// Function name   : fix_clock
// Description       : Accepts a FixClock key, checks it for validity, and (if it's 
//      okay) fixes the system clock. Only useful if you've told Armadillo not to 
//      report clock problems, and your program is handling them itself.
// Return type      : inline bool 
// Argument         : const std::string &fixclockkey
inline bool fix_clock( const std::string &fixclockkey)
{
  CArmadilloFunction arm_lib( _T("FixClock" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  FixClockFn arm_fun = (FixClockFn)arm_lib.get_function();

  return arm_fun( fixclockkey.c_str() );

}

// Function name   : raw_fingerprint_info
// Description       : If you're using the Professional Edition, this function 
//      will return the current raw information for any one of the items that 
//      Armadillo uses to create the hardware fingerprint. Returns zero when a 
//      specified item is unavailable. Valid parameters are:
//         0 (CPU)
//         1 (BIOS)
//         2 (NAME)
//         3 (DOS)
//         4 (GEO)
//         5 (SMART)
//         6 (MAC)
//         7 (MEM)
//         31 (RND)
// Return type      : inline DWORD 
// Argument         : DWORD item
inline DWORD raw_fingerprint_info( DWORD item )
{
  CArmadilloFunction arm_lib( _T("RawFingerprintInfo" ) );

  if ( arm_lib.is_loaded( ) == false ) return 0;

  RawFingerprintInfoFn arm_fun = (RawFingerprintInfoFn)arm_lib.get_function();

  return arm_fun( item );
}


// Function name   : set_user_string
// Description       : In the Professional Edition, you can set up to eight "user 
//      strings" (0..7) of up to 255 characters each (plus the terminating null 
//      character), to be stored with Armadillo's own key information for your 
//      program. These strings persist between runs, until your program removes 
//      them, or you use Armadillo to clear the key information, or the system is 
//      reformatted and the OS is reinstalled. Please note that the null character 
//      (ASCII character zero) is not allowed in these strings except at the end, 
//      otherwise they can contain anything.
// Return type      : inline bool 
// Argument         :  int which
// Argument         : const std::string &user_string
inline bool set_user_string( int which, const std::string &user_string )
{
  CArmadilloFunction arm_lib( _T("SetUserString" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  SetUserStringFn arm_fun = (SetUserStringFn)arm_lib.get_function();

  return arm_fun( which, user_string.c_str() );
}

// Function name   : get_user_string
// Description       : These functions return one of the eight strings set by the 
//      SetUserString function. The VB version returns a BSTR (standard VB string 
//      format); the other one returns a C-style string.
// Return type      : inline std::string
// Argument         :  int which
inline std::string get_user_string( int which )
{
  CArmadilloFunction arm_lib( _T("GetUserString") );

  if ( arm_lib.is_loaded( ) == false ) return std::string();

  GetUserStringFn arm_fun = (GetUserStringFn)arm_lib.get_function();

  char buff[256];
  if ( ! arm_fun( which, buff, 256 ) )
     return std::string();

  return std::string( buff );
}

// Function name   : write_hardware_change_log
// Description       : Writes the hardware change-log to a filename that you 
//      specify. The hardware change log is kept for all programs protected with 
//      Armadillo 3.05 or later; you can find more information about it here.
// Return type      : inline std::string 
// Argument         : const std::string &file_name
inline bool write_hardware_change_log( const std::string &file_name )
{
  CArmadilloFunction arm_lib( _T("WriteHardwareChangeLog" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  WriteHardwareChangeLogFn arm_fun = (WriteHardwareChangeLogFn)arm_lib.get_function();

  return arm_fun( file_name.c_str() );
}

// Function name   : connected_to_server
// Description       : If your program is using client/server Network Licensing, 
//      this function will let you check to see whether the current copy is still 
//      connected to the server copy or not. Returns non-zero if the current copy 
//      is connected to the server, or zero if it is not.  
// Return type      : inline bool 
inline bool connected_to_server(  )
{
  CArmadilloFunction arm_lib( _T("ConnectedToServer" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  ConnectedToServerFn arm_fun = (ConnectedToServerFn)arm_lib.get_function();

  return arm_fun(  );
}

inline bool expire_current_key()
{
  CArmadilloFunction arm_lib( _T("ExpireCurrentKey" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  ExpireCurrentKeyFn arm_fun = (ExpireCurrentKeyFn)arm_lib.get_function();

  return arm_fun(  );
}

// Function name   : call_buy_now_url
// Description       : Calling this function brings up the Website/BuyNow URL that
//      you've set in the current certificate (if any), or (if your program is 
//      tagged for use with the Digial River Network) the purchase URL for your 
//      program. Returns non-zero on success, or zero if there is a failure or no 
//      Website/BuyNow URL is provided in the current certificate.  
// Return type      : inline bool 
// Argument         : HWND parent
inline bool call_buy_now_url( HWND parent  )
{
  CArmadilloFunction arm_lib( _T("CallBuyNowURL" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  CallBuyNowURLFn arm_fun = (CallBuyNowURLFn)arm_lib.get_function();

  return arm_fun( parent );
}

// Function name   : show_enter_key_dialog
// Description       : Calling this function brings up the Website/BuyNow URL that
//      you've set in the current certificate (if any), or (if your program is 
//      tagged for use with the Digial River Network) the purchase URL for your 
//      program. Returns non-zero on success, or zero if there is a failure or no 
//      Website/BuyNow URL is provided in the current certificate.  
// Return type      : inline bool 
// Argument         : HWND parent
inline bool show_enter_key_dialog( HWND parent  )
{
  CArmadilloFunction arm_lib( _T("ShowEnterKeyDialog" ) );

  if ( arm_lib.is_loaded( ) == false ) return false;

  ShowEnterKeyDialogFn arm_fun = (ShowEnterKeyDialogFn)arm_lib.get_function();

  return arm_fun( parent );
}


// Function name   : get_environment_var
// Description       : C++ wrapper for Win32 API routine GetEnvironmentVariable
// Return type      : inline std::string 
// Argument         : const std::string &val
inline std::string get_environment_var( const std::string &val )
{
  char buff[256] = "";
  ::GetEnvironmentVariable( val.c_str(), buff, 256 );

  return std::string( buff );
}

// Function name   : get_error_string
// Description       : gets last error set by ArmAccess.dll
// Return type      : inline std::string 
inline std::string get_arm_error_string( DWORD err = ::GetLastError() )
{
  switch ( err )
  {
  case 0: 
     return std::string( "NO_ERROR\nThe function returned successfully" );

  case 10: 
     return std::string( "ERROR_BAD_ENVIRONMENT\nCan't establish communications with the Armadillo shell. This usually means that you haven't protected the program, or the one-second communications delay on startup: return string(Armadillo 2.50 and 2.51 only, eliminated in 2.52 beta-3) has not yet elapsed. Only returned by the actual file, not the virtual one.)  " ); 

  case  12: 
     return std::string( "ERROR_INVALID_ACCESS\nYou've tried to access a function that is not available in your edition of Armadillo. (For example, the SetUserString function is not available in the Basic Edition of Armadillo.)  " ); 

  case  13: 
     return std::string( "ERROR_INVALID_DATA\nThere was an error in the communications path. You might be able to get it to work if you try the call a second time. If you receive this error again, it's probably unrecoverable. Only returned by the actual file, not the virtual one.  " ); 

  case  53: 
     return std::string( "ERROR_BAD_NETPATH\nThere is no Website/BuyNow URL in the current certificate, or there was an error while bringing up the URL in the user's browser.  " ); 

  case  109: 
     return std::string( "ERROR_BROKEN_PIPE\nCould not open the communications path. This is probably an unrecoverable error. Only returned by the actual file, not the virtual one.  " ); 

  case  111: 
     return std::string( "ERROR_BUFFER_OVERFLOW\nThe string you specified is too long to be stored.  " ); 

  case  122: 
     return std::string( "ERROR_INSUFFICIENT_BUFFER\nThe buffer you supplied is not large enough to hold all the requested data.  " ); 

  case  170: 
     return std::string( "ERROR_BUSY\nReturned only by the CopiesRunning function, if the network is too busy to get a response. Should almost never be seen in the real world.  " );

  case  1013: 
     return std::string( "ERROR_CANTWRITE\nThere was an error storing the key information. The user may not have write access to the registry key locations.  " ); 

  case  1168: 
     return std::string( "ERROR_NOT_FOUND\nThe FixClock key passed to the function was valid, but Armadillo found nothing to fix. The FixClock key is NOT invalidated. When this error is returned, the value returned by the function indicates that it was successful.  " ); 

  case  1327: 
     return std::string( "ERROR_ACCOUNT_RESTRICTION\nThe certificate has \"Limit Key Validity\" set, and this key is either past the date it is allowed to be installed, or Armadillo has detected a problem with the system clock.  " ); 

  case  1331: 
     return std::string( "ERROR_ACCOUNT_DISABLED\nThe key is an upgrade key, and the user does not have an existing upgradable key installed.  " ); 

  case  1341: 
     return std::string( "ERROR_SERVER_DISABLED\nReturned by ConnectedToServer() if the current certificate does not use client/server Network Licensing.  " ); 

  case  1352: 
     return std::string( "ERROR_INVALID_SERVER_STATE\nReturned by ConnectedToServer() if the server has been lost (for any reason, including the server copy being shut down or network problems) and the current certificate uses client/server Network Licensing.  " ); 

  case  1395: 
     return std::string( "ERROR_LICENSE_QUOTA_EXCEEDED\nThe key was successfully installed, but there are now more computers using this key than it is licensed for. Please see the discussion on the Limit Copies option for more information.  " );

  case  1413: 
     return std::string( "ERROR_INVALID_INDEX\nThe index you specified is out of bounds.  " ); 

  case  1603: 
     return std::string( "ERROR_INSTALL_FAILURE\nThe FixClock key passed to the function was valid, but Armadillo could not fix the clock for some reason. The key is NOT invalidated. The user may need to reboot the system and try the FixClock operation again. This should never be seen in practice.  " ); 

  case  1793: 
     return std::string( "ERROR_ACCOUNT_EXPIRED\nThe name/key you specified was valid at one time, but is now expired. It should not be considered a usable key. If this is a FixClock key, the user may not have set the date to the current one; the key is NOT invalidated.  " ); 

  case  1804: 
     return std::string( "ERROR_INVALID_DATATYPE\nThe certificate in use is not expire-by-uses (IncrementCounter), or the key installed is not a hardware locked key (UninstallKey).  " ); 

  case  1909: 
     return std::string( "ERROR_ACCOUNT_LOCKED_OUT\nThis key is listed in the Stolen Codes Database for the program.  " ); 

  case  2202: 
     return std::string( "ERROR_BAD_USERNAME\nThe name/key you specified is not valid for this program.  " ); 

  }   
  return std::string( "Unknown error!" );
}
};

#if !defined( _NDEBUG )
	inline tradery::t_string GetEnvVar( const tradery::t_string& name )
	{
		char x[ 4000 ];
		x[ 0 ] = 0;
		GetEnvironmentVariable( name.c_str(), x, 99 );

//		::MessageBox( 0, name.c_str(), x, 0 );

		return x;
	}

#define GET_ENV_VAR( x ) GetEnvVar( x ).c_str()
#endif


#define MAX_DAYS 30

inline bool isUnlimitedCertificate()
{
  char name[256]=""; 
    // Check the ALTUSERNAME; it should always be set if the program is protected *
  return GetEnvironmentVariable("UNLIMITED_CERTIFICATE", name, 255) != 0;
}

inline bool isTimeLimitedCertificate()
{
  char name[256]=""; 

  return GetEnvironmentVariable( "TIME_LIMITED_CERTIFICATE", name, 255 ) != 0;
}  

inline tradery::t_string getKeyString()
{
	char date[ 256 ] = "";

	return GetEnvironmentVariable( "KEYSTRING", date, 255 ) != 0 ? date : tradery::t_string();
}


inline tradery::Date getKeyCustomDate()
{
	return tradery::Date( getKeyString(), tradery::iso, "." );
}

inline tradery::Date getKeyCreationDate()
{
	char date[ 256 ] = "";

	if( GetEnvironmentVariable( "KEYCREATED", date, 255 ) != 0 )
		return tradery::Date( date, tradery::iso, "." );
	else
		return tradery::Date();
}

enum SubscriptionType
{
	unknown_subscription,
	annual_subscription,
	lifetime_subscription
};

inline SubscriptionType getSubscriptionType()
{
	char extraInfo[256]=""; 

	if( GetEnvironmentVariable( "EXTRAINFO", extraInfo, 255 ) != 0 )
	{
		std::istringstream is( extraInfo );

		unsigned int i;

		is >> i;

		return (i & 1) != 0 ? lifetime_subscription : annual_subscription;
	}
	else
		return unknown_subscription;
}

inline bool isDefaultCertificate()
{
  char name[256]=""; 

  return GetEnvironmentVariable( "DEFAULT_CERTIFICATE", name, 255 ) != 0;
}

inline tradery::t_string getSubscriptionTypeAsString()
{
  switch( getSubscriptionType() )
  {
  case annual_subscription:
    return _T( "yearly");
  case lifetime_subscription:
    return _T( "lifetime" );
  case unknown_subscription:
  default:
    return "unknown";
  }
}

#define ONE_YEAR_AS_DAYS 365

inline tradery::Date subscriptionExpirationDate()
{
	// for def cert, the expiration date is virtually +infinity
  if( !isUnlimitedCertificate() )
    return tradery::PosInfinityDate();

  SubscriptionType subscriptionType( getSubscriptionType() );

  switch( subscriptionType )
  {
  case unknown_subscription:
    return tradery::NegInfinityDate();
  case annual_subscription:
    {
		tradery::Date date;
		try
		{
			date = getKeyCustomDate();
		}
		catch( const tradery::DateException& ) 
		{
    		date = getKeyCreationDate();

		}
		return date + tradery::DateDuration( ONE_YEAR_AS_DAYS );
    }
  case lifetime_subscription:
    return tradery::PosInfinityDate();
  default:
    // unknown subscription
    return tradery::NegInfinityDate();
  }
}

inline tradery::t_string getSubscriptionExpirationMessage( int n = 0 )
{
  return ( boost::format( "Your %1% Yloader service subscription expired on %2%.\n\n" 
						  "Please renew your subscription at http://www.yloader.com and enter the new registration key in the next dialog, or install an older version from http://www.yloader.com/releasehistory.php." ) % 
           getSubscriptionTypeAsString() % subscriptionExpirationDate().to_simple_string() ).str();
}

class SubscriptionExpiredException

{
private:
	tradery::t_string m_message;

public:
	SubscriptionExpiredException( const tradery::t_string& message )
	: m_message( message )
	{
	}

	tradery::t_string message() const { return m_message; }
};

// this checks the expiration date against the timestamp of the current version
#define IS_SUBSCRIPTION_EXPIRED( n ) ds( TIMESTAMP_PW(n), TIMESTAMP(n) ) > subscriptionExpirationDate() 

void subscriptionExpiredMessage( int n );

#if defined( _DEBUG )
#define CHECK_SUBSCRIPTION( n, k )
#else
#define CHECK_SUBSCRIPTION( n, k ) 															    					 \
{   																						    					 \
	if( IS_SUBSCRIPTION_EXPIRED( n ) )  														    				 \
	{ 																						    					 \
																													 \
		if( k == 0 )                                                                                				 \
			subscriptionExpiredMessage( n ); 														    			 \
		else                                                                                        				 \
		{                                                                                           				 \
/*			::MessageBox( 0, ( boost::format( "Subscription error: %1%" ) % n ).str().c_str(), "Error", MB_OK );*/	 \
																													 \
			LOG( log_error, _T( "Invalid address error: 0x0" ) << n << "000ffff" ); 								 \
			__asm                                                                                   				 \
			{                                                                                       				 \
			call eax                                                                            					 \
			__asm _emit 0x4b __asm _emit 0x99 __asm _emit 0x32                                  					 \
			}                                                                                       				 \
		}                                                                                           				 \
																					    							 \
		if( !armadillo::show_enter_key_dialog( 0 ) )                                                				 \
			throw SubscriptionExpiredException( "Subscription expired - downloading failed" );	    				 \
	} 																						    					 \
}   																						    					 \

#endif

void expiredCertMessage();

inline bool isExpiredCertificate()
{
  char name[256]=""; 

  return GetEnvironmentVariable( "EXPIRED", name, 255 ) != 0;
}  

class NotRegisteredException
{
private:
	tradery::t_string m_message;

public:
	NotRegisteredException( const tradery::t_string& message )
	: m_message( message )
	{
	}

	tradery::t_string message() const { return m_message; }
};


#if defined( _DEBUG )
#define CHECK_EXPIRED_CERT
#else
#define CHECK_EXPIRED_CERT checkExpiredCert()

inline void checkExpiredCert( HWND hWnd = 0 )
{
	if( isExpiredCertificate() )
	{
		::MessageBox( hWnd, _T( "You need to register YLoader to continue.\n\n"
								"Please go to http://www.yloader.com and purchase a\n"
								"lifetime or yearly subscription, then enter your\n"
								"new registration key in the next dialog" ), "YLoader registration", MB_OK );
		if( !armadillo::show_enter_key_dialog( hWnd ) )
			throw NotRegisteredException( "Program not registered - downloading failed" );
	}
}
#endif

inline bool clockBack()
{
  char name[256]=""; 

  bool b = GetEnvironmentVariable( "CLOCKBACK", name, 255 ) != 0;  
//  AfxMessageBox( ( tradery::t_string( "clock: " ) + name ).c_str() );
  return b;
}

inline bool clockForward()
{
  char name[256]=""; 

  return GetEnvironmentVariable( "CLOCKFORWARD", name, 255 ) != 0;
}

inline tradery::t_string getStdHardwareFingerprint()
{
  char name[256]=""; 

  if( GetEnvironmentVariable( "FINGERPRINT", name, 255 ) != 0 )
	  return name;
  else
	  return tradery::t_string();
}


inline tradery::t_string getUserKey()
{
  char name[256]=""; 

  if( GetEnvironmentVariable( "USERKEY", name, 255 ) != 0 )
	  return name;
  else
	  return tradery::t_string();
}

inline tradery::t_string uninstallCode()
{
  char name[256]=""; 

  if( GetEnvironmentVariable( "UNINSTALLCODE", name, 255 ) != 0 )
	  return name;
  else
	  return tradery::t_string();
}

inline unsigned int daysLeft()
{
  if( isExpiredCertificate() )
    return 0;
  else
  {
    TCHAR name[256]=""; 

    GetEnvironmentVariable( "DAYSLEFT", name, 255 );

    tradery::t_istringstream i( name );

    unsigned int days;

    i >> days;

    return days;
  }
}

inline const tradery::t_string getUserName()
{
  TCHAR name[256]=""; 

  if( GetEnvironmentVariable( "ALTUSERNAME", name, 255 ) )
    return name;
  else
    return _TT( "" );
}

inline bool enableExtraFeatures()
{
	bool b = isUnlimitedCertificate() ? true : daysLeft() > 0;
	return b;
}


inline tradery::t_string getRegistrationInfoString()
{
	// Check the ALTUSERNAME; it should always be set if the program is protected *
	tradery::t_string regStr;

	subscriptionExpirationDate();

	IS_SUBSCRIPTION_EXPIRED( 2 );

	if( isUnlimitedCertificate() )
	{
		// this is the unlimited certificate
		regStr += tradery::t_string( "Hardware fingerprint: " ) + getStdHardwareFingerprint();
		regStr += tradery::t_string( "\r\nRegistered to: " ) + getUserName().c_str();
		tradery::t_string userKey( getUserKey() );

		if( !userKey.empty() ) 
		{
			regStr += tradery::t_string( "\r\nKey: " ) + userKey;
		}

		switch( getSubscriptionType() ) 
		{
		case annual_subscription:
			regStr += ( IS_SUBSCRIPTION_EXPIRED( 2 ) || tradery::LocalTimeSec().date() > subscriptionExpirationDate() ) ? "\r\nSubscription: expired on " : "\r\nSubscription: expires on: ";
			regStr += subscriptionExpirationDate().to_simple_string();
			break;
		case lifetime_subscription:
			regStr += "\r\nSubscription: Lifetime ";
			break;
		default:
			break;
		}
	}
	else if( isDefaultCertificate() || isTimeLimitedCertificate() )
	{

		if( isExpiredCertificate() )
			regStr += _TT( "Not registered" );
		else
		{
			regStr += ( boost::format( "%1% trial days left" ) % daysLeft() ).str();
		}
	}

	return regStr + "\r\n";
}

