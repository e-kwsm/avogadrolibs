/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "unitcell.h"

#include <cmath>
#include <sstream>

namespace Avogadro::Core {

std::string UnitCell::errorCellParameters(const std::string& name) const
{
  Eigen::IOFormat v3_fmt{
    Eigen::StreamPrecision, 0, " ", ", ", "", "", "[", "]"
  };
  std::stringstream ss;
  ss << name
     << ": lattice vectors does not span "
        "(zero length or linearly dependent)\n"
     << "a=" << aVector().format(v3_fmt) << "\n"
     << "b=" << bVector().format(v3_fmt) << "\n"
     << "c=" << cVector().format(v3_fmt) << "\n";
  return ss.str();
}

UnitCell::UnitCell(const Vector3& a_, const Vector3& b_,
                   const Vector3& c_) noexcept(false)
{
  m_cellMatrix.col(0) = a_;
  m_cellMatrix.col(1) = b_;
  m_cellMatrix.col(2) = c_;
  if (volume() == 0.0) {
    throw std::invalid_argument{ errorCellParameters(__FUNCTION__) };
  }
  computeFractionalMatrix();
}

UnitCell::UnitCell(const Matrix3& cellMatrix_) : m_cellMatrix(cellMatrix_)
{
  if (volume() == 0.0) {
    throw std::invalid_argument{ errorCellParameters(__FUNCTION__) };
  }
  computeFractionalMatrix();
}

void UnitCell::setAVector(const Vector3& v) noexcept(false)
{
  m_cellMatrix.col(0) = v;
  if (volume() == 0.0) {
    throw std::invalid_argument{ errorCellParameters(__FUNCTION__) };
  }
  computeFractionalMatrix();
}

void UnitCell::setBVector(const Vector3& v) noexcept(false)
{
  m_cellMatrix.col(1) = v;
  if (volume() == 0.0) {
    throw std::invalid_argument{ errorCellParameters(__FUNCTION__) };
  }
  computeFractionalMatrix();
}

void UnitCell::setCVector(const Vector3& v) noexcept(false)
{
  m_cellMatrix.col(2) = v;
  if (volume() == 0.0) {
    throw std::invalid_argument{ errorCellParameters(__FUNCTION__) };
  }
  computeFractionalMatrix();
}

void UnitCell::setCellParameters(Real a_, Real b_, Real c_, Real al, Real be,
                                 Real ga) noexcept(false)
{
  if (a_ <= 0.0 || b_ <= 0.0 || c_ <= 0.0) {
    std::stringstream ss;
    ss << __FUNCTION__ << ": length is not positive\n"
       << "a=" << a_ << ", b=" << b_ << ", c=" << c_ << "\n";
    throw std::invalid_argument{ ss.str() };
  }
  if (al <= 0.0 || al >= M_PI || be <= 0.0 || be >= M_PI || ga <= 0.0 ||
      ga >= M_PI) {
    std::stringstream ss;
    ss << __FUNCTION__ << ": angle is wrong\n"
       << "alpha=" << al << ", beta=" << be << ", gamma=" << ga << "\n";
    throw std::invalid_argument{ ss.str() };
  }

  // Convert parameters to matrix. See "Appendix 2: Coordinate Systems and
  // Transformations" of the PDB guide (ref v2.2, 4/23/13,
  // http://www.bmsc.washington.edu/CrystaLinks/man/pdb/guide2.2_frame.html)
  const Real cosAlpha = std::cos(al);
  const Real cosBeta = std::cos(be);
  const Real cosGamma = std::cos(ga);
  const Real sinGamma = std::sin(ga);

  m_cellMatrix(0, 0) = a_;
  m_cellMatrix(1, 0) = static_cast<Real>(0.0);
  m_cellMatrix(2, 0) = static_cast<Real>(0.0);

  m_cellMatrix(0, 1) = b_ * cosGamma;
  m_cellMatrix(1, 1) = b_ * sinGamma;
  m_cellMatrix(2, 1) = static_cast<Real>(0.0);

  m_cellMatrix(0, 2) = c_ * cosBeta;
  m_cellMatrix(1, 2) = c_ * (cosAlpha - cosBeta * cosGamma) / sinGamma;
  m_cellMatrix(2, 2) =
    (c_ / sinGamma) *
    std::sqrt(
      static_cast<Real>(1.0) -
      ((cosAlpha * cosAlpha) + (cosBeta * cosBeta) + (cosGamma * cosGamma)) +
      (static_cast<Real>(2.0) * cosAlpha * cosBeta * cosGamma));
  computeFractionalMatrix();
}

Real UnitCell::signedAngleRadians(const Vector3& v1, const Vector3& v2,
                                  const Vector3& axis)
{
  const Vector3 crossProduct(v1.cross(v2));
  const Real crossProductNorm(crossProduct.norm());
  const Real dotProduct(v1.dot(v2));
  const Real signDet(crossProduct.dot(axis));
  const Real angle(std::atan2(crossProductNorm, dotProduct));
  return signDet > 0.f ? angle : -angle;
}

} // namespace Avogadro::Core
