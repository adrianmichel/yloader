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

#pragma once

class VersionInfo {
 private:
  yloader::Version version;
  yloader::StringPtr description;
  yloader::StringPtr descriptionUrl;
  yloader::StringPtr title;
  std::shared_ptr<yloader::DateTime> releaseDate;
  yloader::StringPtr downloadUrl;

 public:
  // builds update info for current version
  VersionInfo();
  // builds UpdateInfo from json string
  VersionInfo(std::shared_ptr<std::string> s);

  bool operator>(const VersionInfo& versionInfo);

  static std::shared_ptr<yloader::DateTime> getCurrentBuildInfo();

  std::shared_ptr<yloader::DateTime> getReleaseDate() const {
    return releaseDate;
  }

  yloader::StringPtr getDescription() const { return description; }

  yloader::Version getVersion() const { return version; }

  yloader::StringPtr getDownloadUrl() const { return downloadUrl; }

  bool hasDescription() const { return description && !description->empty(); }

  static const VersionInfo CURRENT_VERSION_INFO;
};

class UpdaterHandler {
 public:
  virtual ~UpdaterHandler() {}

  virtual void checkingForUpdate() = 0;
  virtual void updateAvailable() = 0;
  virtual void noUpdateAvailable() = 0;
  virtual void updateError(const std::wstring& error) = 0;
};

class Updater : public yloader::Thread {
 private:
  std::shared_ptr<VersionInfo> m_updateInfo;
  std::wstring m_lastError;
  mutable std::mutex m_mutex;
  UpdaterHandler& m_handler;
  std::wstring m_updateHostName;
  std::wstring m_updateDirectory;

 public:
  Updater(UpdaterHandler& handler, const std::wstring& lang, const StringSettable& hostName, const StringSettable& updateDirectory);
  virtual ~Updater();

  std::shared_ptr<VersionInfo> getUpdateInfo() {
    std::scoped_lock lock(m_mutex);
    return m_updateInfo;
  }

  bool hasUpdate();

  yloader::Version getVersion() const {
    std::scoped_lock lock(m_mutex);
    return m_updateInfo ? m_updateInfo->getVersion() : yloader::Version();
  }

  void run(ThreadContext* context) override;
};
