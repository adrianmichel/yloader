# YLoader

YLoader is a free stock quotes downloader for Windows.

You can find the complete product documentation and download the most recent stable binaries from http://www.yloader.com.

**This document is work in progress and quite incomplete. Email info@yloader.com if you need help or have any questions.**

## License
YLoader is free open-source software distributed under the Terms and Conditions of the GNU General Public License (GPL) version 3 or (at your option) any later version.

For using YLoader, no restrictions apply. You can further redistribute and/or modify this software under the terms of the GPL.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

For details, have a look at the full text of the GPL in the LICENSE file or at http://www.gnu.org/licenses/

## 3rd party components
YLoader uses several 3rd party components, as source code, libraries (need to be downloaded, installed and built separately), or applications. The source code components are already part of the YLoader project, however the libraries and applications need to be downloaded and installed (and libraries built as well) prior to building YLoader. Here is the list:

Source code 
* **CSettingsDlg**, a settings dialog class: https://www.codeproject.com/articles/1831/csettingsdialog-a-dialog-customizing-your-project
* **CReportCtrl**, a List control wrapper class: https://www.codeproject.com/Articles/4014/CReportCtrl-An-extremely-convenient-version-of-rep
* **CDropEdit**, a drag & drop enabled CEdit wrapper class: https://www.codeproject.com/Articles/1899/CDropEdit

Libraries
* **boost**, a set of portable C++ libraries: http://www.boost.org
* **libcurl**, a multiprotocol file transfer library: https://curl.haxx.se/libcurl/
* **JSON for modern C++**, a JSON C++ library: https://github.com/nlohmann/json
* **OpenSSL**, a TLS and SSL toolkit: https://www.openssl.org

Applications 
* **Visual Studio 2015 Community Edition**: https://www.visualstudio.com/vs/older-downloads/ with **Service Update 3**: https://www.visualstudio.com/en-us/news/releasenotes/vs2015-update3-vs
* **Git Command Line Client for Windows**: https://git-scm.com/downloads.
* **Doxygen** (optional), documentation generator: http://wwww.doxygen.org.
* **NSIS** - Nullsoft Scriptable Install System (optional),  a Windows installer generator: http://nsis.sourceforge.net.

## Building YLoader from sources
Follow the steps below to successfully build YLoader binaries. This process will generate around 11Gb of data, so be sure to have enough available storage on the build drive. See above for download URLs for different packages.

Note that this build will generate binaries compatible with Windows 7 or later. **They will not run on Windows XP**.

1. Install Microsoft Visual Studio 2015 Community Edition and make sure to select C++ and all sub-options required to build Windows applications written in C++ using MFC. 
2. Install Visual Studio 2015 Service Update 3.
3. Optionally install Doxygen and/or NSIS if you want to be able to build the documentation and/or the YLoader installer.
4. Install the newest Git client.
5. Create the directory where you want to install and build the YLoader source code, for example:<pre><code>c:\yloader_dev</code></pre>
6. Open a Windows console and navigate to that directory:<pre><code>cd c:\yloader_dev</code></pre>
7. Clone YLoader source code from the github repository into the current directory:<pre><code>git clone https://github.com/adrianmichel/yloader.git</code></pre>
8. Navigate to the *src* directory:<pre><code>cd src</code></pre>
9. Run:<pre><code>build</code></pre>This step will download and build all third party packages that YLoader depends on, and may take up to 30 min, depending on your hardware.
10. Once this build has completed successfully, open Visual Studio, open the *File* menu, *Open*, *Project/Solution*, and open the YLoader solution located at:<pre><code>c:\yloader_dev\src\yloader.sln</code></pre>
12. On the *Build* toolbar located under the Visual Studio main menu select the *Solution Configuration* (Debug/Release) and *Solution Platform* (win32/x64) that you would like to build.
11. In the Solution Explorer pane, expand the *yloader* folder, right click on the *yloader* project under that folder and select *Set as Startup Project*.
12. Right click again on the *yloader* project and click on *Build*.
13. The generated binaries will be under <pre><code>c:\yloader_dev\src\bin\[Release/Debug]\[win32/x64]</pre></code>
14. If you have installed NSIS and built the Release version of win32 and/or x64, the YLoader installer(s) will be created under<pre><code>c:\yloader_dev\src\bin\installer</code></pre>

