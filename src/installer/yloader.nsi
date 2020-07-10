;NSIS Modern User Interface
;Header Bitmap Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General
  !define KEY "Software\YLoader"
  !define YLOADER_INSTALLER_NAME_BASE y6_

  !if ${Platform} == win32
	!define TYPE 32
	!define INSTALLDIR $PROGRAMFILES32\YLoader
  !else
	!define TYPE 64
	!define INSTALLDIR $PROGRAMFILES64\YLoader
  !endif

  BrandingText "YLoader"
  OutFile "${InstallerFile}"

  ;Default installation folder
  InstallDir ${INSTALLDIR}

  ;Name and file
  Name "YLoader Quotes Downloader ${TYPE} bit"
  ;OutFile "YLoader.exe"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU ${KEY} ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "license.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_RUN "$INSTDIR\yloader.exe"
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH


;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR"

  File ${INDIR}\yloader.exe
  File ${INDIR}\yloadercl.exe
  File ${INDIR}\symbols.exe
  File ${INDIR}\misc.dll
  File ${INDIR}\miscwin.dll
  File ${INDIR}\yahooplugin.ypi
  File ${INDIR}\zlib1.dll
  File ${INDIR}\libcurl.dll
  File license.txt

  ;Store installation folder
  WriteRegStr HKCU ${KEY} "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."


;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete $INSTDIR\yloader.exe
  Delete $INSTDIR\yloadercl.exe
  Delete $INSTDIR\symbols.exe
  Delete $INSTDIR\misc.dll
  Delete $INSTDIR\miscwin.dll
  Delete $INSTDIR\yahooplugin.ypi
  Delete ${INDIR}\zlib1.dll
  Delete ${INDIR}\libcurl.dll
  Delete $INSTDIR\license.txt


  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU ${KEY}

SectionEnd
