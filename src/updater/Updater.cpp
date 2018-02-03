/*
Copyright (C) 2017  YLoader.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "Updater.h"

using json = nlohmann::json;
using namespace yloader;
using namespace std;

static const std::wstring test(
    _T("{\
	\"version\": \"5.0\",\
	\"beta\" : null,\
	\"release_date\" : \"10/7/2016\",\
	\"date\" : \"2016-10-31\",\
	\"description_url\" : \"http://www.yloader.com/5.0/description\",\
	\"download_url\" : \"http://www.yloader.com/5.0/download\"\
	}"));

const std::wstring Updater::DEFAULT_YLOADER_UPDATE_HOST_NAME(
    _T("www.yloader.com"));
const std::wstring Updater::DEFAULT_YLOADER_UPDATE_DIRECTORY(_T("/update"));

#define VERSION "version"
#define DESCRIPTION "description"
#define DESCRIPTION_URL "description_url"
#define TITLE "title"
#define RELEASE_DATE "release_date"
#define DOWNLOAD_URL "update_url"

bool VersionInfo::operator>(const VersionInfo& otherVersion) {
  assert(version);
  return !otherVersion.getVersion() || version > otherVersion.getVersion();
}

shared_ptr<yloader::DateTime> VersionInfo::getCurrentBuildInfo() {
  const IMAGE_NT_HEADERS* nt_header =
      (const IMAGE_NT_HEADERS*)((char*)&__ImageBase + __ImageBase.e_lfanew);

  return shared_ptr<yloader::DateTime>(
      new yloader::DateTime(nt_header->FileHeader.TimeDateStamp));
}

VersionInfo::VersionInfo()
    : version(Version::CURRENT), releaseDate(getCurrentBuildInfo()) {}

VersionInfo::VersionInfo(boost::shared_ptr<std::string> s) {
  json j = json::parse(s->c_str());

  if (!j[VERSION].is_null()) {
    version = yloader::Version(s2ws(j[VERSION]));
  }

  if (!j[DESCRIPTION].is_null()) {
    description = StringPtr(new std::wstring(s2ws(j[DESCRIPTION])));
  }

  if (!j[DESCRIPTION_URL].is_null()) {
    descriptionUrl = StringPtr(new std::wstring(s2ws(j[DESCRIPTION_URL])));
  }

  if (!j[TITLE].is_null()) {
    title = StringPtr(new std::wstring(s2ws(j[TITLE])));
  }

  if (!j[RELEASE_DATE].is_null()) {
    releaseDate = shared_ptr<DateTime>(new DateTime(s2ws(j[RELEASE_DATE])));
  }

  if (!j[DOWNLOAD_URL].is_null()) {
    downloadUrl = StringPtr(new std::wstring(s2ws(j[DOWNLOAD_URL])));
  }
}

const VersionInfo VersionInfo::CURRENT_VERSION_INFO = VersionInfo();

Updater::Updater(UpdaterHandler& handler, const std::wstring& lang,
                 const StringSettable& hostName,
                 const StringSettable& updateDirectory)
    : Thread(_T("Updater thread")),
      m_handler(handler),
      m_updateHostName(hostName.getValue(DEFAULT_YLOADER_UPDATE_HOST_NAME)),
      m_updateDirectory(
          updateDirectory.getValue(DEFAULT_YLOADER_UPDATE_DIRECTORY)) {}

#define UNKNOWN_ERROR _T( "Uknown error" )
void Updater::run(yloader::ThreadBase::ThreadContext* context) {
  try {
    yloader::Lock lock(m_mutex);

    m_handler.checkingForUpdate();

    CurlHTTPRequest request(m_updateHostName);

    std::wstring object = m_updateDirectory + _T( "?" ) + VERSION_QUERY_ARGS;

    std::wstring jsonType(_T("Content-type: plain/text"));

    boost::shared_ptr<std::string> r = request.get(object, jsonType);

    m_updateInfo = std::shared_ptr<VersionInfo>(new VersionInfo(r));

    if (m_updateInfo && *m_updateInfo > VersionInfo::CURRENT_VERSION_INFO) {
      m_handler.updateAvailable();
    } else {
      m_handler.noUpdateAvailable();
    }
  } catch (InternetException e) {
    m_lastError = e.message();
    m_handler.updateError(m_lastError);
  } catch (std::exception e) {
    m_lastError = yloader::s2ws(e.what());
    m_handler.updateError(m_lastError);
  } catch (...) {
    m_lastError = UNKNOWN_ERROR;
    m_handler.updateError(m_lastError);
  }
}

Updater::~Updater() {}

bool Updater::hasUpdate() {
  yloader::Lock lock(m_mutex);
  return m_updateInfo && *m_updateInfo > VersionInfo();
}
