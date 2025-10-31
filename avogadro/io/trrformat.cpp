/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "trrformat.h"
#include "struct.h"

#include <avogadro/core/elements.h>
#include <avogadro/core/molecule.h>
#include <avogadro/core/unitcell.h>
#include <avogadro/core/utilities.h>
#include <avogadro/core/vector.h>

#include <istream>
#include <ostream>
#include <string>

using std::map;
using std::pair;
using std::string;
using std::to_string;

namespace Avogadro::Io {

using Core::Array;
using Core::Atom;
using Core::Molecule;
using Core::UnitCell;

constexpr int GROMACS_MAGIC = 1993;
constexpr int DIM = 3;
constexpr float NM_TO_ANGSTROM = 10.0;
string TRRVERSION = "GMX_trn_file";
std::array<string, 15> HEADITEMS = { "ir_size",  "e_size",    "box_size",
                                     "vir_size", "pres_size", "top_size",
                                     "sym_size", "x_size",    "v_size",
                                     "f_size",   "natoms",    "step",
                                     "nre",      "time",      "lambda" };

int swapInteger(int inp)
{
  return (((inp << 24) & 0xff000000) | ((inp << 8) & 0x00ff0000) |
          ((inp >> 8) & 0x0000ff00) | ((inp >> 24) & 0x000000ff));
}

char swapEndian(char endian)
{
  if (endian == '>')
    return '<';
  else
    return '>';
}

/* Checks whether the data stored in the binary file is of float or double type
 */
int isDouble(map<string, int>& header)
{
  int SIZE_DOUBLE = struct_calcsize("d");
  int size = 0;
  std::array<string, 4> headerKeys = { "box_size", "x_size", "v_size",
                                       "f_size" };

  for (auto& headerKey : headerKeys) {
    if (header[headerKey] != 0) {
      if (headerKey == "box_size") {
        size = (int)(header[headerKey] / DIM * DIM);
        break;
      } else {
        size = (int)(header[headerKey] / (header["natoms"] * DIM));
        break;
      }
    }
  }
  return size == SIZE_DOUBLE;
}

bool TrrFormat::read(std::istream& inStream, Core::Molecule& mol)
{
  bool doubleStatus;
  char endian = '>';
  std::array<char, BUFSIZ> buff;
  std::array<char, BUFSIZ> fmt;
  string raw;
  raw.resize(1000);
  int magic, natoms, slen0, slen1;
  std::array<int, 13> headval;
  string subs;
  std::array<string, 3> keyCheck = { "box_size", "vir_size", "pres_size" },
                        keyCheck2 = { "x_size", "v_size", "f_size" };
  map<string, int> header;

  // Determining size of file
  inStream.seekg(0, inStream.end);
  int fileLen = inStream.tellg();
  inStream.seekg(0, inStream.beg);

  // Binary file must start with 1993
  snprintf(fmt.data(), sizeof(fmt), "%c1i", endian);
  inStream.read(buff.data(), struct_calcsize(fmt.data()));
  struct_unpack(buff.data(), fmt.data(), &magic);
  if (magic != GROMACS_MAGIC) {
    // Endian conversion
    magic = swapInteger(magic);
    endian = swapEndian(endian);
    if (magic != GROMACS_MAGIC) {
      appendError("Frame does not start with magic number 1993.");
      return false;
    }
  }

  snprintf(fmt.data(), sizeof(fmt), "%c2i", endian);
  inStream.read(buff.data(), struct_calcsize(fmt.data()));
  struct_unpack(buff.data(), fmt.data(), &slen0, &slen1);

  // Reading trajectory version string
  snprintf(fmt.data(), sizeof(fmt), "%c%ds", endian, slen0 - 1);
  inStream.read(buff.data(), struct_calcsize(fmt.data()));
  struct_unpack(buff.data(), fmt.data(), raw.data());
  subs = string(raw.data()).substr(0, 12);
  if (subs != TRRVERSION) {
    appendError("Gromacs version string mismatch.");
    return false;
  }

  // "ir_size", "e_size", "box_size", "vir_size", "pres_size",
  // "top_size", "sym_size", "x_size", "v_size", "f_size",
  // "natoms", "step", "nre"
  snprintf(fmt.data(), sizeof(fmt), "%c13i", endian);
  inStream.read(buff.data(), struct_calcsize(fmt.data()));
  struct_unpack(buff.data(), fmt.data(), &headval[0], &headval[1], &headval[2],
                &headval[3], &headval[4], &headval[5], &headval[6], &headval[7],
                &headval[8], &headval[9], &headval[10], &headval[11],
                &headval[12]);
  for (int i = 0; i < 13; ++i) {
    header.insert(pair<string, int>(HEADITEMS[i], headval[i]));
  }

  // Reading timestep and lambda
  doubleStatus = isDouble(header);
  if (doubleStatus) {
    double header0, header1;
    snprintf(fmt.data(), sizeof(fmt.data()), "%c2d", endian);
    inStream.read(buff.data(), struct_calcsize(fmt.data()));
    struct_unpack(buff.data(), fmt.data(), &header0, &header1);
    header.insert(pair<string, int>("time", header0));
    header.insert(pair<string, int>("lambda", header1));
  } else {
    float header0, header1;
    snprintf(fmt.data(), sizeof(fmt), "%c2f", endian);
    inStream.read(buff.data(), struct_calcsize(fmt.data()));
    struct_unpack(buff.data(), fmt.data(), &header0, &header1);
    header.insert(pair<string, int>("time", header0));
    header.insert(pair<string, int>("lambda", header1));
  }

  // Reading matrices corresponding to "box_size", "vir_size", "pres_size"
  for (auto& _kid : keyCheck) {
    if (header[_kid] != 0) {
      if (doubleStatus) {
        snprintf(fmt.data(), sizeof(fmt.data()), "%c%dd", endian, DIM * DIM);
        double mat[DIM][DIM];
        inStream.read(buff.data(), struct_calcsize(fmt.data()));
        struct_unpack(buff.data(), fmt.data(), &mat[0][0], &mat[0][1],
                      &mat[0][2], &mat[1][0], &mat[1][1], &mat[1][2],
                      &mat[2][0], &mat[2][1], &mat[2][2]);
        if (_kid == "box_size") {
          auto* uc = new UnitCell(
            Vector3(mat[0][0] * NM_TO_ANGSTROM, mat[0][1] * NM_TO_ANGSTROM,
                    mat[0][2] * NM_TO_ANGSTROM),
            Vector3(mat[1][0] * NM_TO_ANGSTROM, mat[1][1] * NM_TO_ANGSTROM,
                    mat[1][2] * NM_TO_ANGSTROM),
            Vector3(mat[2][0] * NM_TO_ANGSTROM, mat[2][1] * NM_TO_ANGSTROM,
                    mat[2][2] * NM_TO_ANGSTROM));
          if (!uc->isRegular()) {
            appendError("lattice vectors are not linear independent");
            delete uc;
            return false;
          }
          mol.setUnitCell(uc);
        }
      } else {
        snprintf(fmt.data(), sizeof(fmt.data()), "%c%df", endian, DIM * DIM);
        float mat[DIM][DIM];
        inStream.read(buff.data(), struct_calcsize(fmt.data()));
        struct_unpack(buff.data(), fmt.data(), &mat[0][0], &mat[0][1],
                      &mat[0][2], &mat[1][0], &mat[1][1], &mat[1][2],
                      &mat[2][0], &mat[2][1], &mat[2][2]);
        if (_kid == "box_size") {
          auto* uc = new UnitCell(
            Vector3(mat[0][0] * NM_TO_ANGSTROM, mat[0][1] * NM_TO_ANGSTROM,
                    mat[0][2] * NM_TO_ANGSTROM),
            Vector3(mat[1][0] * NM_TO_ANGSTROM, mat[1][1] * NM_TO_ANGSTROM,
                    mat[1][2] * NM_TO_ANGSTROM),
            Vector3(mat[2][0] * NM_TO_ANGSTROM, mat[2][1] * NM_TO_ANGSTROM,
                    mat[2][2] * NM_TO_ANGSTROM));
          if (!uc->isRegular()) {
            appendError("lattice vectors are not linear independent");
            return false;
          }
          mol.setUnitCell(uc);
        }
      }
    }
  }

  using AtomTypeMap = map<string, unsigned char>;
  AtomTypeMap atomTypes;
  unsigned char customElementCounter = CustomElementMin;

  // Reading the coordinates of positions, velocities and forces
  for (auto& _kid : keyCheck2) {
    natoms = header["natoms"];
    std::array<double, DIM> coordsDouble;
    std::array<float, DIM> coordsFloat;
    for (int i = 0; i < natoms; ++i) {
      if (header[_kid] != 0) {
        coordsDouble.fill(0);
        coordsFloat.fill(0);
        if (doubleStatus) {
          snprintf(fmt.data(), sizeof(fmt.data()), "%c%dd", endian, DIM);
          inStream.read(buff.data(), struct_calcsize(fmt.data()));
          struct_unpack(buff.data(), fmt.data(), &coordsDouble[0],
                        &coordsDouble[1], &coordsDouble[2]);
        } else {
          snprintf(fmt.data(), sizeof(fmt.data()), "%c%df", endian, DIM);
          inStream.read(buff.data(), struct_calcsize(fmt.data()));
          struct_unpack(buff.data(), fmt.data(), &coordsFloat[0],
                        &coordsFloat[1], &coordsFloat[2]);
        }

        if (_kid == "x_size") {
          // If parsed coordinates are fractional, the corresponding unscaling
          // is done. Else the positions are assigned as parsed.
          Vector3 pos(
            coordsDouble[0] * NM_TO_ANGSTROM + coordsFloat[0] * NM_TO_ANGSTROM,
            coordsDouble[1] * NM_TO_ANGSTROM + coordsFloat[1] * NM_TO_ANGSTROM,
            coordsDouble[2] * NM_TO_ANGSTROM + coordsFloat[2] * NM_TO_ANGSTROM);

          AtomTypeMap::const_iterator it;
          // if (it == atomTypes.end()) {
          atomTypes.insert(
            std::make_pair(to_string(i), customElementCounter++));
          it = atomTypes.find(to_string(i));
          // if (customElementCounter > CustomElementMax) {
          //   appendError("Custom element type limit exceeded.");
          //   return false;
          // }
          Atom newAtom = mol.addAtom(it->second);
          newAtom.setPosition3d(pos);
        }
      }
    }

    // Set the custom element map if needed
    if (!atomTypes.empty()) {
      Molecule::CustomElementMap elementMap;
      for (const auto& atomType : atomTypes) {
        elementMap.insert(
          std::make_pair(atomType.second, "Atom " + atomType.first));
      }
      mol.setCustomElementMap(elementMap);
    }
  }
  mol.setCoordinate3d(mol.atomPositions3d(), 0);

  // Do we have an animation?
  // EOF check
  int coordSet = 1;
  while (static_cast<int>(inStream.tellg()) != fileLen) {
    // Binary header must start with 1993
    snprintf(fmt.data(), sizeof(fmt.data()), "%c1i", endian);
    inStream.read(buff.data(), struct_calcsize(fmt.data()));
    struct_unpack(buff.data(), fmt.data(), &magic);
    if (magic != GROMACS_MAGIC) {
      // Endian conversion
      magic = swapInteger(magic);
      endian = swapEndian(endian);
      if (magic != GROMACS_MAGIC) {
        appendError("Frame does not start with magic number 1993.");
        return false;
      }
    }

    snprintf(fmt.data(), sizeof(fmt.data()), "%c2i", endian);
    inStream.read(buff.data(), struct_calcsize(fmt.data()));
    struct_unpack(buff.data(), fmt.data(), &slen0, &slen1);

    // Reading trajectory version string
    snprintf(fmt.data(), sizeof(fmt.data()), "%c%ds", endian, slen0 - 1);
    inStream.read(buff.data(), struct_calcsize(fmt.data()));
    struct_unpack(buff.data(), fmt.data(), raw.data());
    subs = string(raw.data()).substr(0, 12);
    if (subs != TRRVERSION) {
      appendError("Gromacs version string mismatch.");
      return false;
    }

    // "ir_size", "e_size", "box_size", "vir_size", "pres_size",
    // "top_size", "sym_size", "x_size", "v_size", "f_size",
    // "natoms", "step", "nre"
    snprintf(fmt.data(), sizeof(fmt.data()), "%c13i", endian);
    inStream.read(buff.data(), struct_calcsize(fmt.data()));
    struct_unpack(buff.data(), fmt.data(), &headval[0], &headval[1],
                  &headval[2], &headval[3], &headval[4], &headval[5],
                  &headval[6], &headval[7], &headval[8], &headval[9],
                  &headval[10], &headval[11], &headval[12]);
    for (int i = 0; i < 13; ++i) {
      header.insert(pair<string, int>(HEADITEMS[i], headval[i]));
    }

    // Reading timestep and lambda
    doubleStatus = isDouble(header);
    if (doubleStatus) {
      double header0, header1;
      snprintf(fmt.data(), sizeof(fmt.data()), "%c2d", endian);
      inStream.read(buff.data(), struct_calcsize(fmt.data()));
      struct_unpack(buff.data(), fmt.data(), &header0, &header1);
      header.insert(pair<string, int>("time", header0));
      header.insert(pair<string, int>("lambda", header1));
    } else {
      float header0, header1;
      snprintf(fmt.data(), sizeof(fmt.data()), "%c2f", endian);
      inStream.read(buff.data(), struct_calcsize(fmt.data()));
      struct_unpack(buff.data(), fmt.data(), &header0, &header1);
      header.insert(pair<string, int>("time", header0));
      header.insert(pair<string, int>("lambda", header1));
    }

    // Reading matrices corresponding to "box_size", "vir_size", "pres_size"
    for (auto& _kid : keyCheck) {
      if (header[_kid] != 0) {
        natoms = header["natoms"];
        if (doubleStatus) {
          snprintf(fmt.data(), sizeof(fmt.data()), "%c%dd", endian, DIM * DIM);
          double mat[DIM][DIM];
          inStream.read(buff.data(), struct_calcsize(fmt.data()));
          struct_unpack(buff.data(), fmt.data(), &mat[0][0], &mat[0][1],
                        &mat[0][2], &mat[1][0], &mat[1][1], &mat[1][2],
                        &mat[2][0], &mat[2][1], &mat[2][2]);
          if (_kid == "box_size") {
            auto* uc = new UnitCell(
              Vector3(mat[0][0] * NM_TO_ANGSTROM, mat[0][1] * NM_TO_ANGSTROM,
                      mat[0][2] * NM_TO_ANGSTROM),
              Vector3(mat[1][0] * NM_TO_ANGSTROM, mat[1][1] * NM_TO_ANGSTROM,
                      mat[1][2] * NM_TO_ANGSTROM),
              Vector3(mat[2][0] * NM_TO_ANGSTROM, mat[2][1] * NM_TO_ANGSTROM,
                      mat[2][2] * NM_TO_ANGSTROM));
            if (!uc->isRegular()) {
              appendError("lattice vectors are not linear independent");
              return false;
            }
            mol.setUnitCell(uc);
          }
        } else {
          snprintf(fmt.data(), sizeof(fmt.data()), "%c%df", endian, DIM * DIM);
          float mat[DIM][DIM];
          inStream.read(buff.data(), struct_calcsize(fmt.data()));
          struct_unpack(buff.data(), fmt.data(), &mat[0][0], &mat[0][1],
                        &mat[0][2], &mat[1][0], &mat[1][1], &mat[1][2],
                        &mat[2][0], &mat[2][1], &mat[2][2]);
          if (_kid == "box_size") {
            auto* uc = new UnitCell(
              Vector3(mat[0][0] * NM_TO_ANGSTROM, mat[0][1] * NM_TO_ANGSTROM,
                      mat[0][2] * NM_TO_ANGSTROM),
              Vector3(mat[1][0] * NM_TO_ANGSTROM, mat[1][1] * NM_TO_ANGSTROM,
                      mat[1][2] * NM_TO_ANGSTROM),
              Vector3(mat[2][0] * NM_TO_ANGSTROM, mat[2][1] * NM_TO_ANGSTROM,
                      mat[2][2] * NM_TO_ANGSTROM));
            if (!uc->isRegular()) {
              appendError("lattice vectors are not linear independent");
              return false;
            }
            mol.setUnitCell(uc);
          }
        }
      }
    }

    natoms = header["natoms"];
    Array<Vector3> positions;
    positions.reserve(natoms);

    // Reading the coordinates of positions, velocities and forces
    for (auto& _kid : keyCheck2) {
      std::array<double, DIM> coordsDouble;
      std::array<float, DIM> coordsFloat;
      for (int i = 0; i < natoms; ++i) {
        if (header[_kid] != 0) {
          coordsDouble.fill(0);
          coordsFloat.fill(0);
          if (doubleStatus) {
            snprintf(fmt.data(), sizeof(fmt.data()), "%c%dd", endian, DIM);
            inStream.read(buff.data(), struct_calcsize(fmt.data()));
            struct_unpack(buff.data(), fmt.data(), &coordsDouble[0],
                          &coordsDouble[1], &coordsDouble[2]);
          } else {
            snprintf(fmt.data(), sizeof(fmt.data()), "%c%df", endian, DIM);
            inStream.read(buff.data(), struct_calcsize(fmt.data()));
            struct_unpack(buff.data(), fmt.data(), &coordsFloat[0],
                          &coordsFloat[1], &coordsFloat[2]);
          }

          if (_kid == "x_size") {
            // If parsed coordinates are fractional, the corresponding unscaling
            // is done. Else the positions are assigned as parsed.
            Vector3 pos(coordsDouble[0] * NM_TO_ANGSTROM +
                          coordsFloat[0] * NM_TO_ANGSTROM,
                        coordsDouble[1] * NM_TO_ANGSTROM +
                          coordsFloat[1] * NM_TO_ANGSTROM,
                        coordsDouble[2] * NM_TO_ANGSTROM +
                          coordsFloat[2] * NM_TO_ANGSTROM);
            positions.push_back(pos);
          }
        }
      }
    }
    mol.setCoordinate3d(positions, coordSet++);
    positions.clear();
  }
  return true;
}

bool TrrFormat::write(std::ostream&, const Core::Molecule&)
{
  return false;
}

std::vector<std::string> TrrFormat::fileExtensions() const
{
  std::vector<std::string> ext;
  ext.emplace_back("trr");
  return ext;
}

std::vector<std::string> TrrFormat::mimeTypes() const
{
  std::vector<std::string> mime;
  mime.emplace_back("application/octet-stream");
  return mime;
}

} // namespace Avogadro::Io
