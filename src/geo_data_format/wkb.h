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

#ifndef __WKB_H__
#define __WKB_H__

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

// #include <cstdint>

#include "geometry/vector.h"

#include <QByteArray>
#include <QDataStream>
#include <QList>
#include <QString>

/**************************************************************************************************/

class QcWkbGeometryObject;

class QcWkbGeometryType
{
public:
  enum
  {
    Z_MASK = 1000,
    M_MASK = 2000,
    ZM_MASK = 3000,

    // http://svn.osgeo.org/postgis/trunk/doc/ZMSgeoms.txt
    SRID_MASK = 0x20000000,

    Point               = 1,
    LineString          = 2,
    Polygon             = 3,
    MultiPoint          = 4,
    MultiLineString     = 5,
    MultiPolygon        = 6,
    GeometryCollection  = 7,
    CircularString      = 8,
    CompoundCurve       = 9,
    CurvePolygon        = 10,
    MultiCurve          = 11,
    MultiSurface        = 12,
    Curve               = 13,
    Surface             = 14,
    PolyhedralSurface   = 15,
    Tin                 = 16,
    Triangle            = 17,
  };

public:
  QcWkbGeometryType();
  QcWkbGeometryType(int base_type, bool has_z = false, bool has_m = false, bool has_srid = false);
  QcWkbGeometryType(const QcWkbGeometryType & other);
  QcWkbGeometryType(quint32 type);
  QcWkbGeometryType(const QString & type_name);

  QcWkbGeometryType & operator=(const QcWkbGeometryType & other);

  bool operator==(const QcWkbGeometryType & other) const;
  inline bool operator!=(const QcWkbGeometryType & other) const {
    return not(*this == other);
  }

  inline int base_type() const { return m_base_type; }
  inline void set_base_type(int base_type) { m_base_type = base_type; }

  inline int has_z() const { return m_has_z; }
  inline void set_has_z(bool has_z) { m_has_z = has_z; }

  inline int has_m() const { return m_has_m; }
  inline void set_has_m(bool has_m) { m_has_m = has_m; }

  inline bool has_srid() const { return m_has_srid; }
  inline void set_has_srid(int has_srid) { m_has_srid = has_srid; }

  quint32 to_wkb() const;
  QString to_wkt() const;

  QcWkbGeometryObject * new_geometry_object();

private:
  int m_base_type;
  bool m_has_z;
  bool m_has_m;
  bool m_has_srid;
};

/**************************************************************************************************/

enum class QcWkbByteOrder
{
  Xdr = 0, // Big Endian
  Ndr = 1  // Little Endian
};

/**************************************************************************************************/

typedef QList<QcVectorDouble> QcVectorDoubleList;

class QcWktParser;

/**************************************************************************************************/

class QcWkbGeometryObject
{
public:
  QcWkbGeometryObject();

  virtual const QcWkbGeometryType & geometry_type() const = 0;

  static bool read_byte_order(QDataStream & stream);
  static void write_byte_order(QDataStream & stream, bool use_big_endian = true);

  static QcWkbGeometryType read_header(QDataStream & stream);
  void write_header(QDataStream & stream, bool use_big_endian = true, bool use_ewkb = false) const;

  static int read_srid(QDataStream & stream);
  void write_srid(QDataStream & stream);

  template<class T> static T read_point(QDataStream & stream);
  template<class T> static void write_point(QDataStream & stream, const T & point);

  static QString point_to_wkt(const QcVectorDouble & point);
  static QString point_3d_to_wkt(const QcVector3DDouble & point);
  static QString point_4d_to_wkt(const QcVector4DDouble & point);

  static void read_points(QDataStream & stream, QcVectorDoubleList & points);
  static void write_points(QDataStream & stream, const QcVectorDoubleList & points);
  static QString points_to_wkt(const QcVectorDoubleList & points);

  static QcWkbGeometryObject * read_geometry_object(QDataStream & stream);
  virtual void init_from_binary(const QByteArray & stream);
  static QcWkbGeometryObject * init_from_wkt(const QString & stream);
  virtual void set_from_binary(QDataStream & stream) = 0;
  virtual void set_from_wkt(QcWktParser * parser) = 0;

  virtual void to_binary(QDataStream & stream, bool use_big_endian = true) const = 0;
  virtual QString to_wkt() const = 0;
  QByteArray to_wkb(bool use_big_endian = true) const;

  bool has_srid() const { return m_srid != -1; }
  inline int srid() const { return m_srid; }
  inline void set_srid(int srid) { m_srid = srid; }
  QString srid_to_ewkt() const;

private:
  int m_srid;
};

/**************************************************************************************************/

class QcWkbPoint : public QcWkbGeometryObject
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbPoint();
  QcWkbPoint(const QByteArray & bytes);
  QcWkbPoint(double x, double y);
  QcWkbPoint(const QcWkbPoint & other);

  QcWkbPoint & operator=(const QcWkbPoint & other);

  bool operator==(const QcWkbPoint & other) const;

  inline const QcVectorDouble & vector() { return m_point; }

  inline double x() const { return m_point.x(); }
  inline double y() const { return m_point.y(); }

  inline void set_x(double x) { m_point.set_x(x); }
  inline void set_y(double y) { m_point.set_y(y); }

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

private:
  QcVectorDouble m_point;
};

/**************************************************************************************************/

// Fixme: duplicated code, how to improve ?

class QcWkbPointZ : public QcWkbGeometryObject
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbPointZ();
  QcWkbPointZ(const QByteArray & bytes);
  QcWkbPointZ(double x, double y, double z);
  QcWkbPointZ(const QcWkbPointZ & other);

  QcWkbPointZ & operator=(const QcWkbPointZ & other);

  bool operator==(const QcWkbPointZ & other) const;

  inline const QcVector3DDouble & vector() { return m_point; }

  inline double x() const { return m_point.x(); }
  inline double y() const { return m_point.y(); }
  inline double z() const { return m_point.z(); }

  inline void set_x(double x) { m_point.set_x(x); }
  inline void set_y(double y) { m_point.set_y(y); }
  inline void set_z(double z) { m_point.set_z(z); }

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

private:
  QcVector3DDouble m_point;
};

/**************************************************************************************************/

class QcWkbPointM : public QcWkbGeometryObject
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbPointM();
  QcWkbPointM(const QByteArray & bytes);
  QcWkbPointM(double x, double y, double m);
  QcWkbPointM(const QcWkbPointM & other);

  QcWkbPointM & operator=(const QcWkbPointM & other);

  bool operator==(const QcWkbPointM & other) const;

  inline const QcVector3DDouble & vector() { return m_point; }

  inline double x() const { return m_point.x(); }
  inline double y() const { return m_point.y(); }
  inline double m() const { return m_point.z(); }

  inline void set_x(double x) { m_point.set_x(x); }
  inline void set_y(double y) { m_point.set_y(y); }
  inline void set_m(double m) { m_point.set_z(m); }

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

private:
  QcVector3DDouble m_point;
};

/**************************************************************************************************/

class QcWkbPointZM : public QcWkbGeometryObject
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbPointZM();
  QcWkbPointZM(const QByteArray & bytes);
  QcWkbPointZM(double x, double y, double z, double m);
  QcWkbPointZM(const QcWkbPointZM & other);

  QcWkbPointZM & operator=(const QcWkbPointZM & other);

  bool operator==(const QcWkbPointZM & other) const;

  inline const QcVector3DDouble & vector() { return m_point; }

  inline double x() const { return m_point.x(); }
  inline double y() const { return m_point.y(); }
  inline double z() const { return m_point.z(); }
  inline double m() const { return m_point.t(); }

  inline void set_x(double x) { m_point.set_x(x); }
  inline void set_y(double y) { m_point.set_y(y); }
  inline void set_z(double z) { m_point.set_z(z); }
  inline void set_m(double m) { m_point.set_t(m); }

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

private:
  QcVector4DDouble m_point;
};

/**************************************************************************************************/

class QcWkbPointList : public QcWkbGeometryObject
{
public:
  QcWkbPointList();
  QcWkbPointList(const QcWkbPointList & other);

  QcWkbPointList & operator=(const QcWkbPointList & other);

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

  inline const QcVectorDoubleList & points() { return m_points; }

private:
  QcVectorDoubleList m_points;
};

/**************************************************************************************************/

class QcWkbLineString : public QcWkbPointList
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbLineString();
  QcWkbLineString(const QByteArray & bytes);
  QcWkbLineString(const QcVectorDoubleList & points);
  QcWkbLineString(const QcWkbLineString & other);

  QcWkbLineString & operator=(const QcWkbLineString & other);
};

/**************************************************************************************************/

class QcWkbPolygon : public QcWkbGeometryObject
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbPolygon();
  QcWkbPolygon(const QByteArray & bytes);
  QcWkbPolygon(const QcWkbPolygon & other);

  QcWkbPolygon & operator=(const QcWkbPolygon & other);

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

  inline const QList<QcVectorDoubleList> & rings() const { return m_rings; }
  void add_ring(const QcVectorDoubleList & ring);

private:
  QList<QcVectorDoubleList> m_rings;
};

/**************************************************************************************************/

class QcWkbMultiPoint : public QcWkbPointList
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbMultiPoint();
  QcWkbMultiPoint(const QByteArray & bytes);
  QcWkbMultiPoint(const QcVectorDoubleList & points);
  QcWkbMultiPoint(const QcWkbMultiPoint & other);

  QcWkbMultiPoint & operator=(const QcWkbMultiPoint & other);
};

/**************************************************************************************************/

class QcWkbGeometryCollection : public QcWkbGeometryObject
{
private:
  static const QcWkbGeometryType m_type;

public:
  inline const QcWkbGeometryType & geometry_type() const { return m_type; }

public:
  QcWkbGeometryCollection();
  QcWkbGeometryCollection(const QByteArray & bytes);
  QcWkbGeometryCollection(const QcWkbGeometryCollection & other);

  QcWkbGeometryCollection & operator=(const QcWkbGeometryCollection & other);

  void set_from_binary(QDataStream & stream);
  void set_from_wkt(QcWktParser * parser);
  void to_binary(QDataStream & stream, bool use_big_endian = true) const;
  QString to_wkt() const;

  inline const QList<const QcWkbGeometryObject *> geometries() const { return m_geometries; };
  void add_geometry(const QcWkbGeometryObject & geometry_object);

private:
  QList<const QcWkbGeometryObject *> m_geometries;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WKB_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
