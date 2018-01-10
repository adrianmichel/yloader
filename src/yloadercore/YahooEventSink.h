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

#pragma once

#define TAB _T( "\t" )
#define DOUBLE_TAB TAB TAB
#define TRIPLE_TAB DOUBLE_TAB TAB
#define QUAD_TAB TRIPLE_TAB TAB

enum EventType {
  event_info,
  event_warning,
  event_error,
  event_fatal_error,
  event_ignored_symbol
};

class YahooEvent {
 private:
  const std::wstring _symbol;
  const std::wstring _message;
  const EventType _type;

 public:
  YahooEvent(const std::wstring& symbol, const std::wstring& message,
             EventType type)
      : _symbol(symbol), _message(message), _type(type) {}

  const std::wstring& message() const { return _message; }
  const std::wstring& symbol() const { return _symbol; }
  EventType type() const { return _type; }
  bool isInfoOrWarning() const {
    return _type == event_info || _type == event_warning;
  }

  const std::wstring toString() const {
    std::wostringstream o;

    switch (_type) {
      case event_info:
        //      o << _T( "info" );
        o << TAB;
        break;
      case event_error:
        o << _T( "error:" ) << TAB;
        break;
      case event_fatal_error:
        o << _T( "fatal error:" );
        break;
      case event_warning:
        o << _T( "warning: " );
        break;
      case event_ignored_symbol:
        o << _T( "ignored" );
        break;
      default:
        assert(false);
        break;
    }

    o << TAB << _symbol << TAB << _message;

    return o.str();
  }
};

class YahooEventSink {
 public:
  virtual ~YahooEventSink() {}
  virtual void event(const YahooEvent& event) = 0;
};

class YahooEventDelegator : public YahooEventSink {
  std::set<YahooEventSink*> _yesSet;

 public:
  virtual void event(const YahooEvent& event) {
    for (std::set<YahooEventSink*>::iterator i = _yesSet.begin();
         i != _yesSet.end(); i++) {
      YahooEventSink* yes = *i;
      assert(yes != 0);
      yes->event(event);
    }
  }

  void addSink(YahooEventSink* sink) {
    assert(sink != 0);
    _yesSet.insert(sink);
  }
};

typedef yloader::ManagedPtr<YahooEventSink> YahooEventSinkPtr;
typedef yloader::ManagedPtr<YahooEventDelegator> YahooEventDelegatorPtr;

class Statistics : public YahooEventSink {
 private:
  unsigned int _totalCount;
  unsigned int _totalProcessedCount;
  unsigned int _processedOkCount;
  unsigned int _processedWithErrorsCount;
  unsigned int _ignoredCount;
  yloader::Timer _duration;  // duration in seconds
  mutable yloader::Mutex _mx;

 public:
  Statistics()
      : _totalCount(0),
        _processedOkCount(0),
        _processedWithErrorsCount(0),
        _totalProcessedCount(0),
        _ignoredCount(0) {}

  void setTotalCount(unsigned int totalCount) { _totalCount = totalCount; }
  virtual void event(const YahooEvent& event) {
    OutputDebugString(_T( "statistics event\n" ));
    yloader::Lock lock(_mx);

    switch (event.type()) {
      case event_error:
        _processedWithErrorsCount++;
        _totalProcessedCount++;
        break;
      case event_fatal_error:
        _totalProcessedCount++;
        break;
      case event_warning:
        //      _processWithWarningsCount++;
        break;
      case event_info:
        _totalProcessedCount++;
        _processedOkCount++;
        break;
      case event_ignored_symbol:
        _totalProcessedCount++;
        _ignoredCount++;
        break;
      default:
        assert(false);
        break;
    }
  }

  unsigned int totalCount() const {
    yloader::Lock lock(_mx);
    return _totalCount;
  }
  unsigned int totalProcessedCount() const {
    yloader::Lock lock(_mx);
    return _totalProcessedCount;
  }
  unsigned int processedOkCount() const {
    yloader::Lock lock(_mx);
    return _processedOkCount;
  }
  unsigned int processedWithErrorsCount() const {
    yloader::Lock lock(_mx);
    return _processedWithErrorsCount;
  }
  unsigned int ignoredCount() const {
    yloader::Lock lock(_mx);
    return _ignoredCount;
  }
  yloader::TimeDuration duration() const {
    yloader::Lock lock(_mx);
    return yloader::Seconds(_duration.elapsed());
  }

  const std::wstring totalCountAsString() const {
    std::wostringstream o;
    o << _totalCount;
    return o.str();
  }

  std::wstring pct(unsigned int value) const {
    std::wostringstream o;
    double pct = 0;

    if (_totalCount != 0) {
      pct = ((double)value / (double)_totalCount) * 100.0;
    }

    o << std::fixed << std::setprecision(1) << (pct < 10 ? _T( " " ) : _T( "" ))
      << pct << _T( "%" );
    return o.str();
  }
  const std::wstring totalProcessedCountAsString() const {
    std::wostringstream o;
    o << _totalProcessedCount;
    return o.str();
  }
  const std::wstring processedOkCountAsString() const {
    std::wostringstream o;
    o << _processedOkCount;
    return o.str();
  }
  const std::wstring processedWithErrorsCountAsString() const {
    std::wostringstream o;
    o << _processedWithErrorsCount;
    return o.str();
  }
  const std::wstring ignoredAsString() const {
    std::wostringstream o;
    o << _ignoredCount;
    return o.str();
  }

  const std::wstring durationAsString() const {
    return yloader::Seconds(_duration.elapsed()).toString();
  }

  const std::wstring totalProcessedCountPctAsString() const {
    std::wostringstream o;
    o << pct(_totalProcessedCount);
    return o.str();
  }
  const std::wstring processedOkCountPctAsString() const {
    std::wostringstream o;
    o << pct(_processedOkCount);
    return o.str();
  }
  const std::wstring processedWithErrorsCountPctAsString() const {
    std::wostringstream o;
    o << pct(_processedWithErrorsCount);
    return o.str();
  }
  const std::wstring ignoredPctAsString() const {
    std::wostringstream o;
    o << pct(_ignoredCount);
    return o.str();
  }

  /*
  void setDuration( double duration )
  {
    _duration = duration;
  }
*/

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

    o << _T( "Total symbols:\t\t\t" ) << totalCount() << std::endl;
    o << _T( "Total symbols processed:\t" ) << totalProcessedCount() << " - "
      << pct(totalProcessedCount(), totalCount()) << std::endl;
    o << _T( "Processed OK:\t\t\t" ) << processedOkCount() << " - "
      << pct(processedOkCount(), totalCount()) << std::endl;
    o << _T( "Errors:\t\t\t\t" ) << processedWithErrorsCount() << " - "
      << pct(processedWithErrorsCount(), totalCount()) << std::endl;
    o << _T( "Ignored:\t\t\t" ) << ignoredCount() << " - "
      << pct(ignoredCount(), totalCount()) << std::endl;
    o << _T( "Duration:\t\t\t" ) << durationAsString() << std::endl;

    return o.str();
  }

  std::wstring toGraphicString() const {
    double p = PCT(totalProcessedCount(), totalCount());

    std::wstring t;

    for (int n = 1; n <= 50; ++n) t += n <= p / 2 ? 0xDB : 0XB0;

    t << " (" << pct(totalProcessedCount(), totalCount()) << ")";
    return t;
  }

  void start(unsigned int total = 0) {
    _totalCount = total;
    _totalProcessedCount = 0;
    _processedOkCount = 0;
    _processedWithErrorsCount = 0;
    _ignoredCount = 0;
    _duration.restart();
  }
};
