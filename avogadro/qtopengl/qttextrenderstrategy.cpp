/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "qttextrenderstrategy.h"

#include <avogadro/rendering/textproperties.h>

#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QImage>
#include <QTransform>
#include <QtGui/QPainter>
#include <QtGui/QPolygonF>

#include <QtWidgets/QApplication>

#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtCore/QRectF>
#include <QtCore/QSysInfo>

#include <cmath>

using Avogadro::Rendering::TextProperties;

namespace {

inline Qt::Alignment textPropertiesToAlignment(const TextProperties& prop)
{
  // This is initialized with no flags set as we want, no need to initialize.
  Qt::Alignment result;

  switch (prop.hAlign()) {
    default:
    case TextProperties::HLeft:
      result |= Qt::AlignLeft;
      break;
    case TextProperties::HCenter:
      result |= Qt::AlignHCenter;
      break;
    case TextProperties::HRight:
      result |= Qt::AlignRight;
      break;
  }

  switch (prop.vAlign()) {
    default:
    case TextProperties::VTop:
      result |= Qt::AlignTop;
      break;
    case TextProperties::VCenter:
      result |= Qt::AlignVCenter;
      break;
    case TextProperties::VBottom:
      result |= Qt::AlignBottom;
      break;
  }

  return result;
}

inline QFont textPropertiesToQFont(const TextProperties& prop)
{
  QString family;
  switch (prop.fontFamily()) {
    case TextProperties::SansSerif:
#ifndef Q_OS_MAC
      family = "SansSerif";
#else
      family = QApplication::font().family();
#endif
      break;
    case TextProperties::Serif:
#ifndef Q_OS_MAC
      family = "serif";
#else
      family = "Times";
#endif
      break;
    case TextProperties::Mono:
#ifndef Q_OS_MAC
      family = "mono";
#else
      family = "Menlo";
#endif
      break;
    default:
      qWarning() << "Unknown font family id: " << prop.fontFamily()
                 << "Defaulting to SansSerif.";
      family = QApplication::font().family();
  }

  TextProperties::FontStyles style = prop.fontStyles();

  QFont result(family, static_cast<int>(prop.pixelHeight()) / 2 + 1,
               static_cast<bool>(style & TextProperties::Bold) ? QFont::Bold
                                                               : QFont::Normal,
               static_cast<bool>(style & TextProperties::Italic));
  result.setUnderline(static_cast<bool>(style & TextProperties::Underline));

  // Scale the font size to match the specified pixel height:
  QFontMetricsF metrics(result);
  int iterLimiter = 0; // no more than 5 iterations below:
  do {
    result.setPointSizeF(result.pointSizeF() *
                         static_cast<qreal>(prop.pixelHeight()) /
                         metrics.height());

    metrics = QFontMetricsF(result);
  } while (
    std::fabs(metrics.height() - static_cast<qreal>(prop.pixelHeight())) >
      static_cast<qreal>(0.5) &&
    iterLimiter++ < 5);

  return result;
}

} // namespace

namespace Avogadro::QtOpenGL {

QtTextRenderStrategy::QtTextRenderStrategy() {}

QtTextRenderStrategy::~QtTextRenderStrategy() {}

Rendering::TextRenderStrategy* QtTextRenderStrategy::newInstance() const
{
  return new QtTextRenderStrategy;
}

void QtTextRenderStrategy::boundingBox(const std::string& string,
                                       const Rendering::TextProperties& tprop,
                                       int bbox[]) const
{
  QRect rect;
  const QFont font(textPropertiesToQFont(tprop));
  const Qt::Alignment align(textPropertiesToAlignment(tprop));
  rect = QFontMetrics(font).boundingRect(rect, align,
                                         QString::fromStdString(string));
  // Rotate if needed
  if (tprop.rotationDegreesCW() != 0.f) {
    // Build transformation
    QTransform transform;
    transform.rotate(static_cast<qreal>(-tprop.rotationDegreesCW()));

    // Transform a floating point representation of the bbox
    QRectF tmpRect(rect);
    tmpRect = transform.mapRect(tmpRect);

    // Update the bbox, rounding values to give the largest containing bbox
    rect.setLeft(static_cast<int>(std::floor(tmpRect.left())));
    rect.setRight(static_cast<int>(std::floor(tmpRect.right())));
    rect.setTop(static_cast<int>(std::ceil(tmpRect.top())));
    rect.setBottom(static_cast<int>(std::floor(tmpRect.bottom())));
  }

  bbox[0] = rect.left();
  bbox[1] = rect.right();
  bbox[2] = rect.top();
  bbox[3] = rect.bottom();
}

void QtTextRenderStrategy::render(const std::string& string,
                                  const Rendering::TextProperties& tprop,
                                  unsigned char* buffer,
                                  const Vector2i& dims) const
{
  // Convert inputs to Qt
  QString str = QString::fromStdString(string);
  Qt::Alignment flags = textPropertiesToAlignment(tprop);
  QFont font = textPropertiesToQFont(tprop);

  // Construct our target image to use the input buffer and clear it.
  QImage target(buffer, dims[0], dims[1], QImage::Format_ARGB32_Premultiplied);
  target.fill(qRgba(0, 0, 0, 0));

  // Painter setup
  QPainter painter(&target);
  painter.setFont(font);

  // Get a tight bbox for the text without rotations:
  QRectF textRect(painter.boundingRect(QRectF(), flags, str));

  // Undo any alignment translations of the bbox.
  textRect.translate(-textRect.topLeft());

  // Apply rotation, if any
  auto rot(static_cast<qreal>(tprop.rotationDegreesCW()));
  if (rot != 0.f) {
    // Rotate the painter:
    painter.rotate(static_cast<qreal>(rot));

    // Convert the bbox into a polygon and rotate it
    QPolygonF textCorners(textRect);
    textCorners = painter.transform().map(textCorners);

    // Find the new origin in the rotated space:
    QPointF newOrigin(-textCorners.boundingRect().topLeft());

    // Rotate the point back (drawText will reapply the rotation)
    newOrigin = painter.transform().inverted().map(newOrigin);

    // Account for the rotation in the bbox:
    textRect.translate(newOrigin);
  }

  // Draw the text
  painter.setPen(
    QColor(tprop.red(), tprop.green(), tprop.blue(), tprop.alpha()));
  painter.drawText(textRect, flags, str);
  painter.end();

  // Convert the buffer from ARGB --> RGBA for openGL by default
  if (!m_preserveArgb)
    argbToRgba(buffer, dims[0] * dims[1]);
}

namespace {

template <int ByteOrder>
inline void argbToRgbaWorker(quint32 in, quint32& out)
{
  Q_UNUSED(in)
  Q_UNUSED(out)
  qWarning("QtTextRenderStrategy::argbToRgba: Invalid byte order.");
}

template <>
inline void argbToRgbaWorker<QSysInfo::BigEndian>(quint32 in, quint32& out)
{
  out = ((in >> 24) & 0xff) | (in << 8);
}

template <>
inline void argbToRgbaWorker<QSysInfo::LittleEndian>(quint32 in, quint32& out)
{
  out = ((in << 16) & 0xff0000) | ((in >> 16) & 0xff) | (in & 0xff00ff00);
}

} // namespace

void QtTextRenderStrategy::argbToRgba(unsigned char* buffer, size_t pixels)
{
  // Adapted from QGLWidget::convertToGLFormat to perform the conversion
  // in-place.
  // input:  0xAARRGGBB
  // output: 0xRRGGBBAA (big endian)
  // output: 0xAABBGGRR (little endian)

  auto* cur = reinterpret_cast<quint32*>(buffer);
  quint32* end = &cur[pixels];

  while (cur < end) {
    // Ignore empty pixels
    if (*cur)
      argbToRgbaWorker<QSysInfo::ByteOrder>(*cur, *cur);
    cur++;
  }
}

} // namespace Avogadro::QtOpenGL
