/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_CORE_VARIANT_INLINE_H
#define AVOGADRO_CORE_VARIANT_INLINE_H

#include "variant.h"

#include <iostream>
#include <sstream>

namespace Avogadro::Core {

inline Variant::Variant() : m_type(Null) {}

inline Variant::Variant(double x, double y, double z) : m_type(Vector)
{
  Vector3* v = new Vector3(x, y, z);
  m_value = v;
}

template <typename T>
inline Variant::Variant(T v) : m_type(Null)
{
  setValue(v);
}

template <>
inline Variant::Variant(char v) : m_type(Int)
{
  m_value = static_cast<unsigned char>(v);
}

template <>
inline Variant::Variant(const char* v) : m_type(String)
{
  m_value = new std::string(v);
}

template <>
inline Variant::Variant(const MatrixXf& v) : m_type(Matrix)
{
  MatrixX* m = new MatrixX(v.rows(), v.cols());
  *m = v.cast<double>();
  m_value = m;
}

template <>
inline Variant::Variant(const Vector3& v) : m_type(Vector)
{
  Vector3* _v = new Vector3(v);
  m_value = _v;
}

template <>
inline Variant::Variant(const Vector3f& v) : m_type(Vector)
{
  Vector3* _v = new Vector3(v.x(), v.y(), v.z());
  m_value = _v;
}

template <>
inline Variant::Variant(const std::vector<double>& v) : m_type(Matrix)
{
  MatrixX* m = new MatrixX(v.size(), 1);
  for (size_t i = 0; i < v.size(); ++i)
    m->coeffRef(i, 0) = v[i];
  m_value = m;
}

inline Variant::Variant(const Variant& variant) : m_type(variant.type())
{
  if (m_type == String)
    m_value = new std::string(variant.toString());
  else if (m_type == Matrix)
    m_value = new MatrixX(*std::get<MatrixX*>(variant.m_value));
  else if (m_type == Vector)
    m_value = new Vector3(*std::get<Vector3*>(variant.m_value));
  else if (m_type != Null)
    m_value = variant.m_value;
}

inline Variant::~Variant()
{
  clear();
}

inline Variant::Type Variant::type() const
{
  return m_type;
}

inline bool Variant::isNull() const
{
  return m_type == Null;
}

inline bool Variant::setValue(double x, double y, double z)
{
  clear();

  m_type = Vector;
  m_value = new Vector3(x, y, z);

  return true;
}

inline bool Variant::setValue(const std::vector<double>& v)
{
  clear();

  auto* tmp = new MatrixX(v.size(), 1);
  for (size_t i = 0; i < v.size(); ++i)
    tmp->coeffRef(i, 0) = v[i];
  m_type = Matrix;
  m_value = tmp;

  return true;
}

template <typename T>
inline bool Variant::setValue(T v)
{
  AVO_UNUSED(v);

#ifndef NDEBUG
#if defined(_MSC_VER)
  std::cerr << " Variant::setValue() not implemented for " << __FUNCSIG__
            << std::endl;
#else
  std::cerr << " Variant::setValue() not implemented for "
            << __PRETTY_FUNCTION__ << std::endl;
#endif
#endif

  clear();

  return false;
}

template <>
inline bool Variant::setValue(bool v)
{
  clear();

  m_type = Bool;
  m_value = v;

  return true;
}

template <>
inline bool Variant::setValue(char v)
{
  clear();

  m_type = Int;
  m_value = static_cast<unsigned char>(v);

  return true;
}

template <>
inline bool Variant::setValue(short v)
{
  clear();

  m_type = Int;
  m_value = v;

  return true;
}

template <>
inline bool Variant::setValue(int v)
{
  clear();

  m_type = Int;
  m_value = v;

  return true;
}

template <>
inline bool Variant::setValue(long v)
{
  clear();

  m_type = Long;
  m_value = v;

  return true;
}

template <>
inline bool Variant::setValue(float v)
{
  clear();

  m_type = Float;
  m_value = v;

  return true;
}

template <>
inline bool Variant::setValue(double v)
{
  clear();

  m_type = Double;
  m_value = v;

  return true;
}

template <>
inline bool Variant::setValue(std::string string)
{
  clear();

  m_type = String;
  m_value = new std::string(string);

  return true;
}

template <>
inline bool Variant::setValue(const char* string)
{
  return setValue(std::string(string));
}

template <>
inline bool Variant::setValue(void* pointer)
{
  clear();

  m_type = Pointer;
  m_value = pointer;

  return true;
}

template <>
inline bool Variant::setValue(MatrixX matrix)
{
  clear();

  m_type = Matrix;
  m_value = new MatrixX(matrix);

  return true;
}

template <>
inline bool Variant::setValue(Vector3 vector)
{
  clear();

  m_type = Vector;
  m_value = new Vector3(vector);

  return true;
}

template <>
inline bool Variant::setValue(Vector3f vector)
{
  clear();

  m_type = Vector;
  m_value = new Vector3(vector.x(), vector.y(), vector.z());

  return true;
}

template <typename T>
inline T Variant::value() const
{
  return 0;
}

template <>
inline bool Variant::value() const
{
  if (m_type == Bool)
    return std::get<bool>(m_value);
  else if (m_type == Int)
    return std::get<int>(m_value) != 0;

  return false;
}

template <>
inline char Variant::value() const
{
  if (m_type == Int)
    return static_cast<char>(std::get<int>(m_value));
  else if (m_type == String && !std::get<std::string*>(m_value)->empty())
    return std::get<std::string*>(m_value)->at(0);

  return '\0';
}

template <>
inline short Variant::value() const
{
  if (m_type == Int)
    return static_cast<short>(std::get<int>(m_value));
  else if (m_type == String)
    return lexical_cast<short>(*std::get<std::string*>(m_value));

  return 0;
}

template <>
inline int Variant::value() const
{
  if (m_type == Int)
    return std::get<int>(m_value);
  else if (m_type == Bool)
    return static_cast<int>(std::get<bool>(m_value));
  else if (m_type == Float)
    return static_cast<int>(std::get<float>(m_value));
  else if (m_type == Double)
    return static_cast<int>(std::get<double>(m_value));
  else if (m_type == String)
    return lexical_cast<int>(*std::get<std::string*>(m_value));

  return 0;
}

template <>
inline long Variant::value() const
{
  if (m_type == Long)
    return std::get<long>(m_value);
  else if (m_type == Int)
    return static_cast<long>(std::get<int>(m_value));
  else if (m_type == String)
    return lexical_cast<long>(*std::get<std::string*>(m_value));

  return 0;
}

template <>
inline float Variant::value() const
{
  if (m_type == Float)
    return std::get<float>(m_value);
  else if (m_type == Double)
    return static_cast<float>(std::get<double>(m_value));
  else if (m_type == Int)
    return static_cast<float>(std::get<int>(m_value));
  else if (m_type == String)
    return lexical_cast<float>(*std::get<std::string*>(m_value));

  return 0;
}

template <>
inline double Variant::value() const
{
  if (m_type == Double)
    return std::get<double>(m_value);
  else if (m_type == Float)
    return static_cast<double>(std::get<float>(m_value));
  else if (m_type == Int)
    return static_cast<double>(std::get<int>(m_value));
  else if (m_type == String)
    return lexical_cast<double>(*std::get<std::string*>(m_value));

  return 0;
}

template <>
inline void* Variant::value() const
{
  if (m_type == Pointer)
    return std::get<void*>(m_value);

  return nullptr;
}

template <>
inline std::string Variant::value() const
{
  if (m_type == String)
    return *std::get<std::string*>(m_value);

  std::stringstream string;

  if (m_type == Int)
    string << std::get<int>(m_value);
  else if (m_type == Float)
    string << std::get<float>(m_value);
  else if (m_type == Double)
    string << std::get<double>(m_value);

  return string.str();
}

template <>
inline MatrixX Variant::value() const
{
  if (m_type == Matrix)
    return *std::get<MatrixX*>(m_value);

  return MatrixX();
}

template <>
inline const MatrixX& Variant::value() const
{
  if (m_type == Matrix)
    return *std::get<MatrixX*>(m_value);

  // Use a static null matrix for the reference.
  static MatrixX nullMatrix(0, 0);
  return nullMatrix;
}

template <>
inline Vector3 Variant::value() const
{
  if (m_type == Vector)
    return *std::get<Vector3*>(m_value);

  return Vector3();
}

template <>
inline const Vector3& Variant::value() const
{
  if (m_type == Vector)
    return *std::get<Vector3*>(m_value);

  static Vector3 nullVector(0, 0, 0);
  return nullVector;
}

template <>
inline std::vector<double> Variant::value() const
{
  if (m_type == Matrix && std::get<MatrixX*>(m_value)->cols() == 1) {
    std::vector<double> list(std::get<MatrixX*>(m_value)->rows());
    for (int i = 0; i < std::get<MatrixX*>(m_value)->rows(); ++i)
      list[i] = std::get<MatrixX*>(m_value)->coeff(i, 0);
    return list;
  }

  return std::vector<double>();
}

inline void Variant::clear()
{
  if (m_type == String) {
    delete std::get<std::string*>(m_value);
    std::get<std::string*>(m_value) = nullptr;
  } else if (m_type == Matrix) {
    delete std::get<MatrixX*>(m_value);
    std::get<MatrixX*>(m_value) = nullptr;
  } else if (m_type == Vector) {
    delete std::get<Vector3*>(m_value);
    std::get<Vector3*>(m_value) = nullptr;
  }

  m_type = Null;
}

inline bool Variant::toBool() const
{
  return value<bool>();
}

inline char Variant::toChar() const
{
  return value<char>();
}

inline unsigned char Variant::toUChar() const
{
  return value<unsigned char>();
}

inline short Variant::toShort() const
{
  return value<short>();
}

inline unsigned short Variant::toUShort() const
{
  return value<unsigned short>();
}

inline int Variant::toInt() const
{
  return value<int>();
}

inline unsigned int Variant::toUInt() const
{
  return value<unsigned int>();
}

inline long Variant::toLong() const
{
  return value<long>();
}

inline unsigned long Variant::toULong() const
{
  return value<unsigned long>();
}

inline float Variant::toFloat() const
{
  return value<float>();
}

inline double Variant::toDouble() const
{
  return value<double>();
}

inline Real Variant::toReal() const
{
  return value<Real>();
}

inline void* Variant::toPointer() const
{
  return value<void*>();
}

inline std::string Variant::toString() const
{
  return value<std::string>();
}

inline MatrixX Variant::toMatrix() const
{
  return value<MatrixX>();
}

inline const MatrixX& Variant::toMatrixRef() const
{
  return value<const MatrixX&>();
}

inline Vector3 Variant::toVector3() const
{
  return value<Vector3>();
}

inline std::vector<double> Variant::toList() const
{
  return value<std::vector<double>>();
}

// --- Operators ----------------------------------------------------------- //
inline Variant& Variant::operator=(const Variant& variant)
{
  if (this != &variant) {
    // Clear previous data,
    clear();

    // Set the new type.
    m_type = variant.m_type;

    // Set the new value,
    if (m_type == String)
      m_value = new std::string(variant.toString());
    else if (m_type == Matrix)
      m_value = new MatrixX(*std::get<MatrixX*>(variant.m_value));
    else if (m_type == Vector)
      m_value = new Vector3(*std::get<Vector3*>(variant.m_value));
    else if (m_type != Null)
      m_value = variant.m_value;
  }

  return *this;
}

// --- Internal Methods ---------------------------------------------------- //
template <typename T>
inline T Variant::lexical_cast(const std::string& str)
{
  T value;
  std::istringstream(str) >> value;
  return value;
}

} // namespace Avogadro::Core

#endif // AVOGADRO_CORE_VARIANT_INLINE_H
