/*
Copyright (C) 2020  YLoader.com

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

constexpr auto test =
    L"{\
	\"version\": \"5.0\",\
	\"beta\" : null,\
	\"release_date\" : \"10/7/2016\",\
	\"date\" : \"2016-10-31\",\
	\"description_url\" : \"https://www.yloader.com/5.0/description\",\
	\"download_url\" : \"https://www.yloader.com/5.0/download\"\
	}";

constexpr auto VERSION = "version";
constexpr auto DESCRIPTION = "description";
constexpr auto DESCRIPTION_URL = "description_url";
constexpr auto TITLE = "title";
constexpr auto RELEASE_DATE = "release_date";
constexpr auto DOWNLOAD_URL = "update_url";

bool VersionInfo::operator>(const VersionInfo& otherVersion) {
  assert(version);
  return !otherVersion.getVersion() || version > otherVersion.getVersion();
}

shared_ptr<yloader::DateTime> VersionInfo::getCurrentBuildInfo() {
  const IMAGE_NT_HEADERS* nt_header = (const IMAGE_NT_HEADERS*)((char*)&__ImageBase + __ImageBase.e_lfanew);

  return std::make_shared< yloader::DateTime >(nt_header->FileHeader.TimeDateStamp);
}

VersionInfo::VersionInfo()
    : version(Version::CURRENT), releaseDate(getCurrentBuildInfo()) {}

VersionInfo::VersionInfo(std::shared_ptr<std::string> s) {
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
constexpr auto DEFAULT_YLOADER_UPDATE_HOST_NAME = L"www.yloader.com";
constexpr auto DEFAULT_YLOADER_UPDATE_DIRECTORY = L"/update";

Updater::Updater(UpdaterHandler& handler, const std::wstring& lang, const StringSettable& hostName, const StringSettable& updateDirectory)
    : Thread(L"Updater thread"),
      m_handler(handler),
      m_updateHostName(hostName.getValue(DEFAULT_YLOADER_UPDATE_HOST_NAME)),
      m_updateDirectory(updateDirectory.getValue(DEFAULT_YLOADER_UPDATE_DIRECTORY)) {}

constexpr auto UNKNOWN_ERROR = L"Unknown error";

void Updater::run(yloader::ThreadBase::ThreadContext* context) {
  try {
    std::scoped_lock lock(m_mutex);

    m_handler.checkingForUpdate();

    CurlHTTPRequest request(m_updateHostName);

    std::wstring object = m_updateDirectory + L"?" + VERSION_QUERY_ARGS;

    std::wstring jsonType(L"Content-type: plain/text");

    std::shared_ptr<std::string> r = request.get(object, jsonType);

    m_updateInfo = std::make_shared< VersionInfo >(r);

    if (m_updateInfo && *m_updateInfo > VersionInfo::CURRENT_VERSION_INFO) {
      m_handler.updateAvailable();
    }
    else {
      m_handler.noUpdateAvailable();
    }
  }
  catch (InternetException e) {
    m_lastError = e.message();
    m_handler.updateError(m_lastError);
  }
  catch (std::exception e) {
    m_lastError = yloader::s2ws(e.what());
    m_handler.updateError(m_lastError);
  }
  catch (...) {
    m_lastError = UNKNOWN_ERROR;
    m_handler.updateError(m_lastError);
  }
}

Updater::~Updater() {}

bool Updater::hasUpdate() {
  std::scoped_lock lock(m_mutex);
  return m_updateInfo && *m_updateInfo > VersionInfo();
}
