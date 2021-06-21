/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2013 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "fileformat.h"

#include <fstream>
#include <locale>
#include <sstream>

namespace Avogadro {
namespace Io {

using std::ifstream;
using std::locale;
using std::ofstream;

FileFormat::FileFormat() : m_mode(None)
{
}

FileFormat::~FileFormat()
{
}

bool FileFormat::open(const std::string& fileName_, Operation mode_)
{
  close();
  m_fileName = fileName_;
  m_mode = mode_;
  if (!m_fileName.empty()) {
    // Imbue the standard C locale.
    locale cLocale("C");
    if (m_mode & Read) {
      m_in = std::unique_ptr<std::istream>(
        new ifstream(m_fileName.c_str(), std::ifstream::binary));
      if (m_in->is_open()) {
        m_in->imbue(cLocale);
        return true;
      } else {
        appendError("Error opening file: " + fileName_);
        return false;
      }
    } else if (m_mode & Write) {
      m_out = std::unique_ptr<std::ostream>(
        new ofstream(m_fileName.c_str(), std::ofstream::binary));
      if (m_out->is_open()) {
        m_out->imbue(cLocale);
        return true;
      } else {
        appendError("Error opening file: " + fileName_);
        return false;
      }
    }
  }
  return false;
}

void FileFormat::close()
{
  m_in.reset();
  m_out.reset();
  m_mode = None;
}

bool FileFormat::readMolecule(Core::Molecule& molecule)
{
  if (!m_in)
    return false;
  return read(*m_in, molecule);
}

bool FileFormat::writeMolecule(const Core::Molecule& molecule)
{
  if (!m_out)
    return false;
  return write(*m_out, molecule);
}

bool FileFormat::readFile(const std::string& fileName_,
                          Core::Molecule& molecule)
{
  bool result = open(fileName_, Read);
  if (!result)
    return false;

  result = readMolecule(molecule);
  close();
  return result;
}

bool FileFormat::writeFile(const std::string& fileName_,
                           const Core::Molecule& molecule)
{
  bool result = open(fileName_, Write);
  if (!result)
    return false;

  result = writeMolecule(molecule);
  close();
  return result;
}

bool FileFormat::readString(const std::string& string, Core::Molecule& molecule)
{
  std::istringstream stream(string, std::istringstream::in);
  // Imbue the standard C locale.
  locale cLocale("C");
  stream.imbue(cLocale);
  return read(stream, molecule);
}

bool FileFormat::writeString(std::string& string,
                             const Core::Molecule& molecule)
{
  std::ostringstream stream(string, std::ostringstream::out);
  // Imbue the standard C locale.
  locale cLocale("C");
  stream.imbue(cLocale);
  bool result = write(stream, molecule);
  string = stream.str();
  return result;
}

void FileFormat::clear()
{
  m_fileName.clear();
  m_error.clear();
}

void FileFormat::appendError(const std::string& errorString, bool newLine)
{
  m_error += errorString;
  if (newLine)
    m_error += "\n";
}

} // namespace Io
} // namespace Avogadro
