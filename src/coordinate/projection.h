// -*- mode: c++ -*-

/***************************************************************************************************
**
** $QTCARTO_BEGIN_LICENSE:GPL3$
**
** Copyright (C) 2016 Fabrice Salvaire
** Contact: http://www.fabrice-salvaire.fr
**
** This file is part of the QtCarto library.
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
** $QTCARTO_END_LICENSE$
**
***************************************************************************************************/

/**************************************************************************************************/

#ifndef __PROJECTION_H__
#define __PROJECTION_H__

/**************************************************************************************************/

#include "qtcarto_global.h"
#include "enum_flag.h"
#include "geometry/vector.h"
#include "math/interval.h"

// #include <cstdint>

#include <QMap>
#include <QString>
#include <QSharedPointer>

#ifdef WITH_PROJ4
#include "proj_api.h"
#endif

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QDebug;
class QDataStream;

class QcGeoCoordinate;

/**************************************************************************************************/

#ifdef WITH_PROJ4
class QC_EXPORT QcProjection4
{
 public:
  QcProjection4(const QString & definition, projCtx context = nullptr);
  ~QcProjection4();

  void transform(const QcProjection4 & proj2, double & x, double & y) const;
  void transform(const QcProjection4 & proj2, double & x, double & y, double & z) const;

  bool is_latlong() const;

 private:
  QString m_definition;
  projPJ m_projection;
};
#endif

/**************************************************************************************************/

class QC_EXPORT QcProjection
{
 public:
  enum class PreserveBit : unsigned int { // std::uint8_t
    PreserveAll           = 0xFFF,
    PreserveDirection     = 0x1, // azimuthal or zenithal
    PreserveShape         = 0x2, // conformal or orthomorphic
    PreserveArea          = 0x4, // equal-area or equiareal or equivalent or authalic
    PreserveDistance      = 0x8, // equidistant
    PreserveShortestRoute = 0xA
  };

 public:
  static const QcProjection * by_srid(const QString & srid);

 private:
  static void init();
  static void register_projection(QcProjection * projection);
  static QMap<QString, QcProjection *> m_instances; // QSharedPointer<>

 public:
  QcProjection();
  QcProjection(const QString & srid, const QcInterval2DDouble & extent, PreserveBit preserve_bit);
  QcProjection(const QcProjection & other);

  QcProjection & operator=(const QcProjection & other);

  bool operator==(const QcProjection & other) const;
  inline bool operator!=(const QcProjection & other) const {
    return !operator==(other);
  }

  inline const QString & srid() const { return m_srid; }

  inline const QcInterval2DDouble & extent() const { return m_extent; }
  inline const QcIntervalDouble & x_extent() const { return m_extent.x(); }
  inline const QcIntervalDouble & y_extent() const { return m_extent.y(); }
  inline bool is_valid_x(double x) const { return x_extent().contains(x); }
  inline bool is_valid_y(double y) const { return y_extent().contains(y); }
  inline bool is_valid_xy(double x, double y) const { return m_extent.contains(x, y); }

  inline PreserveBit preserve_bit() const { return m_preserve_bit; }
  inline bool preserve_direction() const { return test_preserve_bit(PreserveBit::PreserveDirection); }
  inline bool preserve_shape() const { return test_preserve_bit(PreserveBit::PreserveShape); }
  inline bool preserve_area() const { return test_preserve_bit(PreserveBit::PreserveArea); }
  inline bool preserve_distance() const { return test_preserve_bit(PreserveBit::PreserveDistance); }
  inline bool preserve_shortest_route() const { return test_preserve_bit(PreserveBit::PreserveShortestRoute); }

  QcGeoCoordinate coordinate(double x, double y) const;

 private:
  bool test_preserve_bit(PreserveBit preserve_bit) const;

 private:
  QString m_srid;
  QcInterval2DDouble m_extent;
  PreserveBit m_preserve_bit;

#ifdef WITH_PROJ4
 public:
  virtual QString proj4_definition() const;
  inline QcProjection4 & projection4() const { return *m_projection4; }

 private:
  QSharedPointer<QcProjection4> m_projection4; // Fixme: several instance
#endif
};

ENUM_FLAGS(QcProjection::PreserveBit, unsigned int)

inline bool QcProjection::test_preserve_bit(PreserveBit preserve_bit) const {
  return test_bit(m_preserve_bit, preserve_bit);
}

/**************************************************************************************************/

class QC_EXPORT QcGeoCoordinateTrait
{
 public:
  virtual const QcProjection & projection() const = 0;

 public:
  QcGeoCoordinateTrait();
  QcGeoCoordinateTrait(const QcGeoCoordinateTrait & other);

  QcGeoCoordinateTrait & operator=(const QcGeoCoordinateTrait & other);

  bool operator==(const QcGeoCoordinateTrait & other) const;
  inline bool operator!=(const QcGeoCoordinateTrait & other) const {
    return !operator==(other);
  }

  bool is_valid() const;

  inline double x() const { return m_x; }
  inline void set_x(double value) { m_x = value; } // Fixme: check validity

  inline double y() const { return m_y; }
  inline void set_y(double value) { m_y = value; }

#ifdef WITH_PROJ4
  void transform(QcGeoCoordinateTrait & other) const;
#endif

  inline QcVectorDouble vector() const {
    return QcVectorDouble(m_x, m_y);
  }

 private:
  // double[2] coordinates
  double m_x;
  double m_y;
};

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug, const QcGeoCoordinateTrait &);
#endif

#ifndef QT_NO_DATASTREAM
QC_EXPORT QDataStream &operator<<(QDataStream & stream, const QcGeoCoordinateTrait & coordinate);
QC_EXPORT QDataStream &operator>>(QDataStream & stream, QcGeoCoordinateTrait & coordinate);
#endif

/**************************************************************************************************/

template <typename Projection>
class QC_EXPORT QcGeoCoordinateTemplate : public QcGeoCoordinateTrait
{
 public:
  static const Projection cls_projection; // class
  inline const QcProjection & projection() const { return cls_projection; }; // instance

 public:
  QcGeoCoordinateTemplate() : QcGeoCoordinateTrait() {}
  QcGeoCoordinateTemplate(double x, double y);
};

/**************************************************************************************************/

class QC_EXPORT QcGeoCoordinate : public QcGeoCoordinateTrait
{
 public:
  QcGeoCoordinate() : QcGeoCoordinateTrait() {}
  QcGeoCoordinate(const QcProjection * projection, double x, double y);

  inline const QcProjection & projection() const { return *m_projection; }; // instance

 private:
  const QcProjection * m_projection; // class
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#ifndef QC_MANUAL_INSTANTIATION
#include "projection.hxx"
#endif

/**************************************************************************************************/

#endif /* __PROJECTION_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
