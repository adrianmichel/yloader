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

constexpr auto TAB = L"\t";
constexpr auto DOUBLE_TAB = L"\t\t";
constexpr auto TRIPLE_TAB = L"\t\t\t";
constexpr auto QUAD_TAB = L"\t\t\t\t";

enum EventType {
  event_info,
  event_warning,
  event_error,
  event_fatal_error,
  event_ignored_symbol
};

class YahooEvent {
 private:
  const std::wstring m_symbol;
  const std::wstring m_message;
  const EventType t_type;

 public:
  YahooEvent(const std::wstring& symbol, const std::wstring& message,
             EventType type)
      : m_symbol(symbol), m_message(message), t_type(type) {}

  const std::wstring& message() const { return m_message; }
  const std::wstring& symbol() const { return m_symbol; }
  EventType type() const { return t_type; }
  bool isInfoOrWarning() const {
    return t_type == event_info || t_type == event_warning;
  }

  const std::wstring toString() const {
    std::wostringstream o;

    switch (t_type) {
      case event_info:
        //      o << _T( "info" );
        o << TAB;
        break;
      case event_error:
        o << L"error:" << TAB;
        break;
      case event_fatal_error:
        o << L"fatal error:";
        break;
      case event_warning:
        o << L"warning: ";
        break;
      case event_ignored_symbol:
        o << L"ignored";
        break;
      default:
        assert(false);
        break;
    }

    o << TAB << m_symbol << TAB << m_message;

    return o.str();
  }
};

class YahooEventSink {
 public:
  virtual ~YahooEventSink() {}
  virtual void event(const YahooEvent& event) = 0;
};

class YahooEventDelegator : public YahooEventSink {
  std::set<YahooEventSink*> m_yesSet;

 public:
  virtual void event(const YahooEvent& event) {
    for (std::set<YahooEventSink*>::iterator i = m_yesSet.begin();
         i != m_yesSet.end(); i++) {
      YahooEventSink* yes = *i;
      assert(yes != 0);
      yes->event(event);
    }
  }

  void addSink(YahooEventSink* sink) {
    assert(sink != 0);
    m_yesSet.insert(sink);
  }
};

using YahooEventSinkPtr = std::shared_ptr<YahooEventSink>;
using YahooEventDelegatorPtr = std::shared_ptr<YahooEventDelegator>;

class Statistics : public YahooEventSink {
 private:
  size_t m_totalCount;
  unsigned int m_totalProcessedCount;
  unsigned int m_processedOkCount;
  unsigned int m_processedWithErrorsCount;
  unsigned int m_ignoredCount;
  yloader::Timer m_duration;  // duration in seconds
  mutable std::mutex m_mx;

 public:
  Statistics()
      : m_totalCount(0),
        m_processedOkCount(0),
        m_processedWithErrorsCount(0),
        m_totalProcessedCount(0),
        m_ignoredCount(0) {}

  void setTotalCount(unsigned int totalCount) { m_totalCount = totalCount; }
  void event(const YahooEvent& event) override {
    std::scoped_lock lock(m_mx);

    switch (event.type()) {
      case event_error:
        m_processedWithErrorsCount++;
        m_totalProcessedCount++;
        break;
      case event_fatal_error:
        m_totalProcessedCount++;
        break;
      case event_warning:
        //      _processWithWarningsCount++;
        break;
      case event_info:
        m_totalProcessedCount++;
        m_processedOkCount++;
        break;
      case event_ignored_symbol:
        m_totalProcessedCount++;
        m_ignoredCount++;
        break;
      default:
        assert(false);
        break;
    }
  }

  size_t totalCount() const {
    std::scoped_lock lock(m_mx);
    return m_totalCount;
  }
  unsigned int totalProcessedCount() const {
    std::scoped_lock lock(m_mx);
    return m_totalProcessedCount;
  }
  unsigned int processedOkCount() const {
    std::scoped_lock lock(m_mx);
    return m_processedOkCount;
  }
  unsigned int processedWithErrorsCount() const {
    std::scoped_lock lock(m_mx);
    return m_processedWithErrorsCount;
  }
  unsigned int ignoredCount() const {
    std::scoped_lock lock(m_mx);
    return m_ignoredCount;
  }
  yloader::TimeDuration duration() const {
    std::scoped_lock lock(m_mx);
    return yloader::Seconds(m_duration.elapsed());
  }

  const std::wstring totalCountAsString() const {
    std::wostringstream o;
    o << m_totalCount;
    return o.str();
  }

  std::wstring pct(unsigned int value) const {
    std::wostringstream o;
    double pct = 0;

    if (m_totalCount != 0) {
      pct = ((double)value / (double)m_totalCount) * 100.0;
    }

    o << std::fixed << std::setprecision(1) << (pct < 10 ? L" " : L"") << pct << L"%";
    return o.str();
  }
  const std::wstring totalProcessedCountAsString() const {
    return std::to_wstring(m_totalProcessedCount);
  }
  const std::wstring processedOkCountAsString() const {
    return std::to_wstring(m_processedOkCount);
  }
  const std::wstring processedWithErrorsCountAsString() const {
    return std::to_wstring(m_processedWithErrorsCount);
  }
  const std::wstring ignoredAsString() const {
    return std::to_wstring(m_ignoredCount);
  }

  const std::wstring durationAsString() const {
    return yloader::Seconds(m_duration.elapsed()).toString();
  }

  const std::wstring totalProcessedCountPctAsString() const {
    return pct(m_totalProcessedCount);
  }
  const std::wstring processedOkCountPctAsString() const {
    return std::to_wstring(m_processedOkCount);
  }
  const std::wstring processedWithErrorsCountPctAsString() const {
    return std::to_wstring(m_processedWithErrorsCount);
  }
  const std::wstring ignoredPctAsString() const {
    return pct(m_ignoredCount);
  }

  double PCT(double v1, double v2) const {
    return (v2 != 0 ? v1 / v2 * 100 : 0);
  }

  std::wstring pct(double v1, double v2) const {
    double p = PCT(v1, v2);

    std::wostringstream x;

    x << std::fixed << std::setprecision(2) << p << " %";

    return x.str();
  }

  std::wstring toString() const {
    std::wostringstream o;

    o << L"Total symbols:\t\t\t" << totalCount() << std::endl;
    o << L"Total symbols processed:\t" << totalProcessedCount() << " - " << pct(totalProcessedCount(), totalCount()) << std::endl;
    o << L"Processed OK:\t\t\t" << processedOkCount() << " - " << pct(processedOkCount(), totalCount()) << std::endl;
    o << L"Errors:\t\t\t\t" << processedWithErrorsCount() << " - " << pct(processedWithErrorsCount(), totalCount()) << std::endl;
    o << L"Ignored:\t\t\t" << ignoredCount() << " - " << pct(ignoredCount(), totalCount()) << std::endl;
    o << L"Duration:\t\t\t" << durationAsString() << std::endl;

    return o.str();
  }

  std::wstring toGraphicString() const {
    double p = PCT(totalProcessedCount(), totalCount());

    std::wstring t;

    for (int n = 1; n <= 50; ++n) {
      t += n <= p / 2 ? 0xDB : 0XB0;
    }

    t += L" (" + pct(totalProcessedCount(), totalCount()) + L")";
    return t;
  }

  void start(size_t total = 0) {
    m_totalCount = total;
    m_totalProcessedCount = 0;
    m_processedOkCount = 0;
    m_processedWithErrorsCount = 0;
    m_ignoredCount = 0;
    m_duration.restart();
  }
};
