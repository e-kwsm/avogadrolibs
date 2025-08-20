/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "sdfformat.h"

namespace Avogadro::Io {

SdfFormat::SdfFormat() : MdlFormat()
{
  m_writeProperties = true;
}

SdfFormat::~SdfFormat() {}

bool SdfFormat::read(std::istream& in, Core::Molecule& mol)
{
  return MdlFormat::read(in, mol);
}

bool SdfFormat::write(std::ostream& out, const Core::Molecule& mol)
{
  return MdlFormat::write(out, mol);
}

std::set<std::string> SdfFormat::fileExtensions() const
{
  std::set<std::string> ext;
  ext.insert("sdf");
  ext.insert("sd3");
  return ext;
}

std::set<std::string> SdfFormat::mimeTypes() const
{
  std::set<std::string> mime;
  mime.insert("chemical/x-mdl-molfile");
  return mime;
}

} // namespace Avogadro::Io
