/*
Copyright(C) 2017  YLoader.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <datetime.h>
#include <time.h>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

namespace yloader {
// base 64 code taken from http://www.adp-gmbh.ch/cpp/common/base64.html
const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

inline std::string base64_encode(unsigned char const* bytes_to_encode,
                                 unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++) ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];

    while ((i++ < 3)) ret += '=';
  }

  return ret;
}

inline std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++) ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

// http://en.wikipedia.org/wiki/XXTEA
//#include <stdint.h>
#define DELTA 0x9e3779b9
#define MX \
  ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z));

#define SIZE_SIZE sizeof(uint32_t)
#define STRLEN_CHARS(str) (str.length() * sizeof(std::string::value_type))

class Crypt {
 private:
  uint32_t m_key[4];
  const bool m_randomize;
  const bool m_base64;

 private:
  /*
v is the n word data vector
k is the 4 word key
n is negative for decoding
if n is zero result is 1 and no coding or decoding takes place, otherwise the
result is zero assumes 32 bit ‘long’ and same endian coding and decoding
*/
  void btea(uint32_t* v, int n, uint32_t const k[4]) {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    if (n > 1) /* Coding Part */
    {
      rounds = 6 + 52 / n;
      sum = 0;
      z = v[n - 1];
      do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p = 0; p < n - 1; p++) y = v[p + 1], z = v[p] += MX;
        y = v[0];
        z = v[n - 1] += MX;
      } while (--rounds);
    } else if (n < -1) /* Decoding Part */
    {
      n = -n;
      rounds = 6 + 52 / n;
      sum = rounds * DELTA;
      y = v[0];
      do {
        e = (sum >> 2) & 3;
        for (p = n - 1; p > 0; p--) z = v[p - 1], y = v[p] -= MX;
        z = v[n - 1];
        y = v[0] -= MX;
      } while ((sum -= DELTA) != 0);
    }
  }

 public:
  // with randomize, the encrypted string will be different each time
  Crypt(const uint32_t key[4], bool randomize = true, bool base64 = false)
      : m_randomize(randomize), m_base64(base64) {
    memcpy(m_key, key, sizeof(uint32_t) * 4);
  }

  boost::shared_ptr<std::string> en(const std::string& str) {
    if (!str.empty()) {
      std::string ts(ws2s(timeStamp(true)));

      static bool b = false;

      if (!b) {
        srand(time(0));
        b = true;
      }

      unsigned int k = rand() % 32;

      while (k-- > 0) {
        assert(k < 32);
        ts += (unsigned char)rand();
      }

      int n = SIZE_SIZE + (STRLEN_CHARS(str) / sizeof(uint32_t)) + 1;

      if (m_randomize) n += STRLEN_CHARS(ts) / sizeof(uint32_t) + 1;

      boost::shared_array<uint32_t> output(new uint32_t[n]);

      ((uint32_t*)output.get())[0] = (uint32_t)(str.length());

      memcpy(((char*)output.get()) + SIZE_SIZE, str.data(), STRLEN_CHARS(str));

      if (m_randomize)
        memcpy(((char*)output.get()) + SIZE_SIZE + STRLEN_CHARS(str), ts.data(),
               STRLEN_CHARS(ts));

      btea(output.get(), n, m_key);

      char* p((char*)output.get());

      if (m_base64)
        return boost::shared_ptr<std::string>(new std::string(
            base64_encode((unsigned char*)p, n * sizeof(uint32_t))));
      else
        return boost::shared_ptr<std::string>(
            new std::string(p, n * sizeof(uint32_t)));

    } else
      return boost::shared_ptr<std::string>();
  }

  // an encrypted string will always be align to the size of a unit32_t
  boost::shared_ptr<std::string> de(const std::string& s) {
    std::string str;
    if (m_base64)
      str = base64_decode(s);
    else
      str = s;

    // make sure the string is not empty and is aligned
    if (!str.empty() && str.length() % sizeof(uint32_t) == 0) {
      // number of unit32_t characters
      int n(STRLEN_CHARS(str) / sizeof(uint32_t));
      boost::shared_array<uint32_t> input(new uint32_t[n]);

      memcpy(input.get(), str.data(), STRLEN_CHARS(str));

      btea(input.get(), -n, m_key);

      uint32_t size = *((uint32_t*)input.get());

      // make sure the size is for a string lower or equal in length to the
      // whole string - the size of the initial unit32 char
      if (size <= (n * sizeof(uint32_t) - 1)) {
        char* p((char*)input.get());

        return boost::shared_ptr<std::string>(
            new std::string(p + sizeof(unsigned int), size));
      } else
        // the string has been tampered with most likely, so return an empty
        // string
        return boost::shared_ptr<std::string>();

    } else
      return boost::shared_ptr<std::string>();
  }
};
}  // namespace yloader
