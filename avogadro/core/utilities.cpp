/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "utilities.h"

#include <fast_float/fast_float.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <system_error>

namespace Avogadro::Core {

namespace {

std::vector<std::string> split(const std::string& string, char delimiter,
                               bool skipEmpty)
{
  std::vector<std::string> elements;
  std::stringstream stringStream(string);
  std::string item;
  while (std::getline(stringStream, item, delimiter)) {
    if (skipEmpty && item.empty())
      continue;
    elements.push_back(item);
  }
  return elements;
}

bool contains(const std::string& input, const std::string& search,
              bool caseSensitive)
{
  if (caseSensitive) {
    return input.find(search) != std::string::npos;
  } else {
    std::string inputLower = input;
    std::string searchLower = search;
    std::transform(inputLower.begin(), inputLower.end(), inputLower.begin(),
                   ::tolower);
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(),
                   ::tolower);
    return inputLower.find(searchLower) != std::string::npos;
  }
}

bool startsWith(const std::string& input, const std::string& search)
{
  return input.size() >= search.size() &&
         input.compare(0, search.size(), search) == 0;
}

bool endsWith(std::string const& input, std::string const& ending)
{
  if (ending.size() > input.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), input.rbegin());
}

std::string trimmed(const std::string& input)
{
  size_t start = input.find_first_not_of(" \n\r\t");
  size_t end = input.find_last_not_of(" \n\r\t");
  if (start == std::string::npos && end == std::string::npos)
    return "";
  return input.substr(start, end - start + 1);
}

constexpr fast_float::chars_format kGeneral =
  fast_float::chars_format::general | fast_float::chars_format::no_infnan |
  fast_float::chars_format::allow_leading_plus |
  fast_float::chars_format::skip_white_space;

constexpr fast_float::chars_format kFortran =
  fast_float::chars_format::fortran | fast_float::chars_format::no_infnan |
  fast_float::chars_format::allow_leading_plus |
  fast_float::chars_format::skip_white_space;

// Nothing is read from the C locale here: fast_float looks only at the
// characters themselves. "nan" and "inf" are refused because no file format
// Avogadro reads uses them as data, and a non-finite value poisons every
// later sum.
template <typename T>
const char* parseFloating(const char* first, const char* last, T& value)
{
  if (first == nullptr || last == nullptr || first >= last)
    return nullptr;

  T parsed = 0;
  fast_float::from_chars_result result =
    fast_float::from_chars(first, last, parsed, kGeneral);
  if (result.ec != std::errc() && result.ec != std::errc::result_out_of_range)
    return nullptr;

  // Fortran writes a double precision exponent with a 'D' ("1.0D-03"). Try
  // again in fortran mode when the number stopped at one, but keep that result
  // only if it really consumed the exponent. Fortran mode cannot be used
  // outright: it also accepts an exponent with no letter at all, so "1-5" would
  // read as 1e-5 and "1.5+2" as 150, misreading things such as a range of
  // residue numbers.
  if (result.ptr != last && (*result.ptr == 'D' || *result.ptr == 'd')) {
    T fortranParsed = 0;
    const fast_float::from_chars_result fortran =
      fast_float::from_chars(first, last, fortranParsed, kFortran);
    if ((fortran.ec == std::errc() ||
         fortran.ec == std::errc::result_out_of_range) &&
        fortran.ptr > result.ptr + 1) {
      parsed = fortranParsed;
      result = fortran;
    }
  }

  if (result.ec == std::errc::result_out_of_range) {
    // fast_float has already stored a signed zero for underflow and an
    // infinity for overflow. Keep the zero, and clamp the infinity so that
    // later arithmetic cannot see it.
    if (std::isinf(parsed))
      parsed = std::signbit(parsed) ? std::numeric_limits<T>::lowest()
                                    : std::numeric_limits<T>::max();
  }

  value = parsed;
  return result.ptr;
}

// Decode a T (int32_t, float or double) stored in the given byte order,
// independent of the host's.
// Going through the integer bits keeps -0.0, denormals, inf and NaN exact.
template <typename T>
T unpackValue(const char* data, ByteOrder byteOrder)
{
  using Bits = std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>;
  Bits bits = 0;
  for (std::size_t i = 0; i < sizeof(Bits); ++i) {
    const std::size_t shift =
      byteOrder == ByteOrder::BigEndian ? sizeof(Bits) - 1 - i : i;
    bits |= static_cast<Bits>(static_cast<unsigned char>(data[i]))
            << (8 * shift);
  }
  T value;
  std::memcpy(&value, &bits, sizeof(T));
  return value;
}

} // namespace

const char* parseDouble(const char* first, const char* last, double& value)
{
  return parseFloating(first, last, value);
}

const char* parseFloat(const char* first, const char* last, float& value)
{
  return parseFloating(first, last, value);
}

int32_t unpackInt32(const char* data, ByteOrder byteOrder)
{
  return unpackValue<int32_t>(data, byteOrder);
}

float unpackFloat(const char* data, ByteOrder byteOrder)
{
  return unpackValue<float>(data, byteOrder);
}

double unpackDouble(const char* data, ByteOrder byteOrder)
{
  return unpackValue<double>(data, byteOrder);
}

} // namespace Avogadro::Core
