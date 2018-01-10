========================================================================
    STATIC LIBRARY : updater Project Overview
========================================================================

AppWizard has created this updater library project for you.

This file contains a summary of what you will find in each of the files that
make up your updater application.


updater.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

updater.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).


/////////////////////////////////////////////////////////////////////////////

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named updater.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
The compiler and linker switches have been modified to support MFC. Using the
MFC ClassWizard with this project requires that you add several files to the
project, including "resource.h", "updater.rc" and a "updater.h" that
includes resource.h. If you add an rc file to a static library, you may
experience difficulties due to the limitation that only one rc file may be
present in a Dll or Exe. This problem may be overcome by including the
library's .rc file into the parent project's .rc file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
