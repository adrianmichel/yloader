# YLoader

YLoader is a free stock quotes downloader for Windows.

You can find the complete product documentation and download the most recent stable binaries from http://www.yloader.com.

**Email info@yloader.com with any questions or feedback**

## License
YLoader is free open-source software distributed under the Terms and Conditions of the GNU General Public License (GPL) version 3 or (at your option) any later version.

For using YLoader, no restrictions apply. You can further redistribute and/or modify this software under the terms of the GPL.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

For details, have a look at the full text of the GPL in the LICENSE file or at http://www.gnu.org/licenses/

## 3rd party components
### Libraries
YLoader uses several 3rd party components, as source code or libraries that need to be downloaded, installed and built separately
The source code components are already part of the YLoader project, however the libraries and applications need to be downloaded and installed (and libraries built as well) prior to building YLoader, which is done by the bootstrap.bat script. Here is the list of all 3rd party components used:

Source code 
* **CSettingsDlg**, a settings dialog class: https://www.codeproject.com/articles/1831/csettingsdialog-a-dialog-customizing-your-project
* **CReportCtrl**, a List control wrapper class: https://www.codeproject.com/Articles/4014/CReportCtrl-An-extremely-convenient-version-of-rep
* **CDropEdit**, a drag & drop enabled CEdit wrapper class: https://www.codeproject.com/Articles/1899/CDropEdit

Libraries
* **boost**, a set of portable C++ libraries: http://www.boost.org
* **libcurl**, a multiprotocol file transfer library: https://curl.haxx.se/libcurl/
* **JSON for modern C++**, a JSON C++ library: https://github.com/nlohmann/json
* **OpenSSL**, a TLS and SSL toolkit: https://www.openssl.org

### Applications 
Applications used in the course of YLoader development:
* **Visual Studio 2022 Community Edition**
* **Git Command Line Client for Linux**
* **Doxygen** (optional), documentation generator: http://wwww.doxygen.org.
* **NSIS** - Nullsoft Scriptable Install System, a Windows installer generator: http://nsis.sourceforge.net (already included in the external directory).
* **vcpkg** - Library manager: https://github.com/Microsoft/vcpkg
* **fciv** - utility for calculating the MD5 and SHA1 hashes, which can be downloaded from https://www.microsoft.com (already included in the external directory).

## Building YLoader from sources
Follow the steps below to successfully build YLoader binaries and optionally the installer(s). This process will generate quite a bit of data, so be sure to have enough available storage on the build drive. 

Note that this build will generate binaries compatible with Windows 7 or later. **They will not run on Windows XP**.

1. Install Microsoft Visual Studio 2022 Community Edition and make sure to select C++ and all sub-options required to build Windows applications written in C++ using MFC. 
4. Install the newest Git client.
6. Open a Windows console and clone the YLoader repository to the desired directory:<pre><code>git clone https://github.com/adrianmichel/yloader.git [yloader_dir] </code></pre>
8. cd to the new YLoader directory directory and run <pre><code>bootstrap.bat</code></pre>This will download and build third party packages that YLoader depends on.
10. Once this build has completed successfully, open Visual Studio, open the *File* menu, *Open*, *Project/Solution*, and open the YLoader solution located at:<pre><code>[yloader_dir]\src\yloader.sln</code></pre>
12. On the *Build* toolbar located under the Visual Studio main menu select the *Solution Configuration* (Debug/Release) and *Solution Platform* (win32/x64) that you would like to build.
11. In the Solution Explorer pane, expand the *yloader* folder, right click on the *yloader* project under that folder and select *Set as Startup Project*.
12. Right click again on the *yloader* project and click on *Build*.
13. The generated binaries will be under <pre><code>[yloader_dir]\[Release/Debug]\[win32/x64]</pre></code>
14. If you have installed NSIS and built the Release version of either win32 and/or x64, the YLoader installer(s) will be created under<pre><code>[yloader_dir]\src\bin\installer</code></pre>
15. If you have installed the fciv.exe utility, an additional text file with the MD5 and SHA1 hashes will generated for each of 32 and 64 bit under the installer directory.
