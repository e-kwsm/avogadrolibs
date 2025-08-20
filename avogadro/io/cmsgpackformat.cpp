/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "cmsgpackformat.h"

#include <iostream>

namespace Avogadro::Io {

std::vector<std::string> CMsgPackFormat::fileExtensions() const
{
  return { "cmpk" };
}

std::vector<std::string> CMsgPackFormat::mimeTypes() const
{
  return { "chemical/x-cmpack" };
}

bool CMsgPackFormat::read(std::istream& in, Core::Molecule& molecule)
{
  return CjsonFormat::deserialize(in, molecule, false);
}

bool CMsgPackFormat::write(std::ostream& out, const Core::Molecule& molecule)
{
  return CjsonFormat::serialize(out, molecule, false);
}

} // namespace Avogadro::Io
