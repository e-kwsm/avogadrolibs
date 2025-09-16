/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "utilities.h"
#include <algorithm>

namespace Avogadro::Core {

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

} // namespace Avogadro::Core
