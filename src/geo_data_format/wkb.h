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

/*
struct Point {
  double x;
  double y;
};

struct LinearRing {
  uint32_t number_of_points;
  Point points[number_of_points];
};

struct WkbPoint {
  char byte_order;
  uint32_t wkb_type; // 1
  Point point;
};

struct WkbLineString {
  uint8_t byte_order;
  uint32_t wkb_type; // 2
  uint32_t number_of_points;
  Point points[number_of_points];
};

struct WkbPolygon {
  uint8_t byte_order;
  uint32_t wkb_type; // 3
  uint32_t number_of_rings;
  LinearRing rings[number_of_rings];
};

struct WkbMultiPoint {
  uint8_t byte_order;
  uint32_t wkb_type; // 4
  uint32_t number_of_points;
  WkbPoint WkbPoints[number_of_points];
};

struct WkbMultiLineString {
  uint8_t byte_order;
  uint32_t wkb_type; // 5
  uint32_t number_of_line_strings;
  WkbLineString WkbLineStrings[number_of_line_strings];
};

struct wkbMultiPolygon {
  uint8_t byte_order;
  uint32_t wkb_type; // 6
  uint32_t number_of_polygons;
  WkbPolygon wkbPolygons[number_of_polygons];
};

WkbGeometry {
  union {
    WkbPoint point;
    WkbLineString line_string;
    WkbPolygon polygon;
    WkbGeometryCollection collection;
    WkbMultiPoint multi_point;
    WkbMultiLineString multi_line_string;
    WkbMultiPolygon multi_polygon;
  }
};

struct WkbGeometryCollection {
  uint8_t byte_order;
  uint32_t wkb_type; // 7
  uint32_t number_of_geometries;
  WkbGeometry geometries[number_of_geometries]
};
*/

/**************************************************************************************************/

enum class QcWkbGeometryType
{
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

  PointZ              = 1001,
  LineStringZ         = 1002,
  PolygonZ            = 1003,
  MultiPointZ         = 1004,
  MultiLineStringZ    = 1005,
  MultiPolygonZ       = 1006,
  GeometryCollectionZ = 1007,
  CircularStringZ     = 1008,
  CompoundCurveZ      = 1009,
  CurvePolygonZ       = 1010,
  MultiCurveZ         = 1011,
  MultiSurfaceZ       = 1012,
  CurveZ              = 1013,
  SurfaceZ            = 1014,
  PolyhedralSurfaceZ  = 1015,
  TinZ                = 1016,
  TriangleZ           = 1017,

  PointM              = 2001,
  LineStringM         = 2002,
  PolygonM            = 2003,
  MultiPointM         = 2004,
  MultiLineStringM    = 2005,
  MultiPolygonM       = 2006,
  GeometryCollectionM = 2007,
  CircularStringM     = 2008,
  CompoundCurveM      = 2009,
  CurvePolygonM       = 2010,
  MultiCurveM         = 2011,
  MultiSurfaceM       = 2012,
  CurveM              = 2013,
  SurfaceM            = 2014,
  PolyhedralSurfaceM  = 2015,
  TinM                = 2016,
  TriangleM           = 2017,

  PointZM              = 3001,
  LineStringZM         = 3002,
  PolygonZM            = 3003,
  MultiPointZM         = 3004,
  MultiLineStringZM    = 3005,
  MultiPolygonZM       = 3006,
  GeometryCollectionZM = 3007,
  CircularStringZM     = 3008,
  CompoundCurveZM      = 3009,
  CurvePolygonZM       = 3010,
  MultiCurveZM         = 3011,
  MultiSurfaceZM       = 3012,
  CurveZM              = 3013,
  SurfaceZM            = 3014,
  PolyhedralSurfaceZM  = 3015,
  TinZM                = 3016,
  TriangleZM           = 3017
};

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

  virtual QcWkbGeometryType geometry_type() const = 0;
  virtual QString geometry_type_name() const = 0;

  static bool read_byte_order(QDataStream & stream);
  static void write_byte_order(QDataStream & stream, bool use_big_endian = true);

  static QcWkbGeometryType read_header(QDataStream & stream);
  void write_header(QDataStream & stream, bool use_big_endian = true) const;

  static QcVectorDouble read_point(QDataStream & stream);
  static void write_point(QDataStream & stream, const QcVectorDouble & point);
  static QString point_to_wkt(const QcVectorDouble & point);

  static void read_points(QDataStream & stream, QcVectorDoubleList & points);
  static void write_points(QDataStream & stream, const QcVectorDoubleList & points);
  static QString points_to_wkt(const QcVectorDoubleList & points);

  static QcWkbGeometryObject * new_geometry_object(QcWkbGeometryType type);
  static QcWkbGeometryObject * read_geometry_object(QDataStream & stream);
  virtual void init_from_binary(const QByteArray & stream);
  static QcWkbGeometryObject * init_from_wkt(const QString & stream);
  virtual void set_from_binary(QDataStream & stream) = 0;
  virtual void set_from_wkt(QcWktParser * parser) = 0;

  virtual void to_binary(QDataStream & stream, bool use_big_endian = true) const = 0;
  virtual QString to_wkt() const = 0;
  QByteArray to_wkb(bool use_big_endian = true) const;
};

/**************************************************************************************************/

class QcWkbPoint : public QcWkbGeometryObject
{
public:
  inline QcWkbGeometryType geometry_type() const { return QcWkbGeometryType::Point; }
  inline QString geometry_type_name() const { return QStringLiteral("Point"); }

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
public:
  inline QcWkbGeometryType geometry_type() const { return QcWkbGeometryType::LineString; }
  inline QString geometry_type_name() const { return QStringLiteral("LineString"); }

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
public:
  inline QcWkbGeometryType geometry_type() const { return QcWkbGeometryType::Polygon; }
  inline QString geometry_type_name() const { return QStringLiteral("Polygon"); }

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
public:
  inline QcWkbGeometryType geometry_type() const { return QcWkbGeometryType::MultiPoint; }
  inline QString geometry_type_name() const { return QStringLiteral("MultiPoint"); }

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
public:
  inline QcWkbGeometryType geometry_type() const { return QcWkbGeometryType::GeometryCollection; }
  inline QString geometry_type_name() const { return QStringLiteral("GeometryCollection"); }

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
