#pragma once

#include <yloader.h>

// default value for various settings
constexpr auto DEFAULT_AUTO_START = false;
constexpr auto DEFAULT_CHECK_FOR_UPDATES_AT_STARTUP = true;
constexpr auto DEFAULT_PERIOD = Period::daily;
constexpr auto DEFAULT_ADJUST = true;
constexpr auto DEFAULT_ALL_AVAILABLE = false;
constexpr auto DEFAULT_UPDATE = false;
constexpr auto DEFAULT_VALIDATE_PRICES = true;
constexpr auto DEFAULT_SORT_BARS_ASCENDING = true;
constexpr auto DEFAULT_DATE_FORMAT = yloader::us;
constexpr auto DEFAULT_ADD_SYMBOL = false;
constexpr auto DEFAULT_COLUMN_NUMBER = 0;
constexpr auto DEFAULT_EXTENSION = L"csv";
constexpr auto DEFAULT_PAD_DATE_FIELDS = false;
constexpr auto DEFAULT_SYMBOL_TIMEOUT = 30;
constexpr auto DEFAULT_CREATE_SUBDIRS = false;
constexpr auto DEFAULT_THREADS = 5;
constexpr auto MAX_THREADS = 20;
constexpr auto DEFAULT_AUTO_SELECT_OUTPUT_PATH = false;
constexpr auto DEFAULT_FIELD_SEPARATOR = L",";
constexpr auto DEFAULT_VOLUME_MULTIPLIER = 1;
constexpr auto DEFAULT_FILE_HEADER = L"";
constexpr auto DEFAULT_SAVE_FILE_ALL = L"";
constexpr auto DEFAULT_DONT_RELOAD_DATA_IN_UPDATE_MODE = false;
constexpr auto DEFAULT_HANDLING_INVALID_DATA = 0;
constexpr auto DEFAULT_VOLUME0_INVALID = 0;
constexpr auto DEFAULT_DATE_SEPARATOR = L"/";
constexpr auto DEFAULT_APPEND_TO_LOG = false;
constexpr auto DEFAULT_LOG_ONLY_ERRORS = true;
constexpr auto DEFAULT_PROGRESS = 100;
constexpr auto DEFAULT_REGEX_FLAGS = 0;
constexpr auto DEFAULT_MATCH_REGEX = L"(.*),(.*)/(.*)/(.*),(.*),(.*),(.*),(.*),(.*)";
constexpr auto DEFAULT_FORMAT_STRING = L"$1,$2/$3/$4,$5,$6,$7,$8,$9";
constexpr auto DEFAULT_ENABLE_REGEX_FORMATTING = false;
constexpr auto DEFAULT_MATCH_REGEX_UNLOCKED = false;
constexpr auto DEFAULT_ERROR_SYMBOLS_LIST = L"";
constexpr auto DEFAULT_APPEND_TO_ERROR_SYMBOLS_LIST = true;
constexpr auto DEFAULT_IGNORE_ERROR_SYMBOLS_LIST = true;
constexpr auto DEFAULT_IGNORE_SYMBOLS_LIST = L"";
constexpr auto DEFAULT_DO_NOT_CHECK_FOR_UPDATE = false;
constexpr auto DEFAULT_FIXED_DECIMALS_COUNT = -1;
constexpr auto DEFAULT_DECIMAL_SEPARATOR = L".";
