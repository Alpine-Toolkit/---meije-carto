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

#include <QtTest/QtTest>

/**************************************************************************************************/

#include "geo_data_format/wkb.h"

/***************************************************************************************************/

class TestQcWkb: public QObject
{
  Q_OBJECT

private slots:
  void wkb_tests();
  void wkt_tests();
  void test_wkt(const QString & input, const QString & truth);
};

void
TestQcWkb::test_wkt(const QString & input, const QString & truth)
{
  QcWkbGeometryObject * geometry_object = QcWkbGeometryObject::init_from_wkt(input);
  qInfo() << geometry_object->to_wkt();
  QVERIFY(geometry_object->to_wkt() == truth);
}

void
TestQcWkb::wkb_tests()
{
  QByteArray bytes;

  QcWkbPoint point1(1.12, 2.234);
  bytes = point1.to_wkb();
  QcWkbPoint point2(bytes);
  QVERIFY(point1 == point2);

  QcWkbPointZ pointz1(1.12, 2.234, 3.456);
  bytes = pointz1.to_wkb();
  QcWkbPointZ pointz2(bytes);
  QVERIFY(pointz1 == pointz2);

  QcWkbPointM pointm1(1.12, 2.234, 3.456);
  bytes = pointm1.to_wkb();
  QcWkbPointM pointm2(bytes);
  QVERIFY(pointm1 == pointm2);

  QcWkbPointZM pointzm1(1.12, 2.234, 3.456, 4.567);
  bytes = pointzm1.to_wkb();
  QcWkbPointZM pointzm2(bytes);
  QVERIFY(pointzm1 == pointzm2);
  // qInfo() << pointzm2.to_wkt();
}

void
TestQcWkb::wkt_tests()
{
  test_wkt("POINT(4.1 6.6)",
           "Point(4.1 6.6)");

  test_wkt("POINT   (4.1 6.6)",
           "Point(4.1 6.6)");

  test_wkt("POINT   (4.1   6.6)",
           "Point(4.1 6.6)");

  test_wkt("POINT   (   4.1   6.6   )",
           "Point(4.1 6.6)");

  test_wkt("POINT Z (4.1 6.6 7.7)",
           "Point Z(4.1 6.6 7.7)");

  test_wkt("POINT M (4.1 6.6 7.7)",
           "Point M(4.1 6.6 7.7)");

  test_wkt("POINT ZM (4.1 6.6 7.7 8.8)",
           "Point ZM(4.1 6.6 7.7 8.8)");

  test_wkt("LINESTRING Empty",
           "LineString Empty");

  test_wkt("LINESTRING (30 10,10 30,40 40)",
           "LineString(30 10, 10 30, 40 40)");

  test_wkt("LINESTRING (30 10, 10 30, 40 40)",
           "LineString(30 10, 10 30, 40 40)");

  test_wkt("LINESTRING (30 10,  10 30,   40 40)",
           "LineString(30 10, 10 30, 40 40)");

  test_wkt("LINESTRING (30 10   ,   10 30   ,   40 40)",
           "LineString(30 10, 10 30, 40 40)");

  test_wkt("LINESTRING ((30 10), (10 30), (40 40))",
           "LineString(30 10, 10 30, 40 40)");

  test_wkt("multipoint (30 10,10 30,40 40)",
           "MultiPoint(30 10, 10 30, 40 40)");

  test_wkt("polygon ((30 10,10 30,40 40))",
           "Polygon((30 10, 10 30, 40 40))");

  test_wkt("polygon ((30 10,10 30,40 40), (10 10, 20 20))",
           "Polygon((30 10, 10 30, 40 40), (10 10, 20 20))");

  test_wkt("GEOMETRYCOLLECTION (POINT(4 6),POINT EMPTY, POLYGON  EMPTY, LINESTRING(4 6,7 10))",
           // Fixme: Point Empty
           // "GeometryCollection(Point(4 6), Point EMPTY, Polygon Empty, LineString(4 6, 7 10))");
           "GeometryCollection(Point(4 6), Point(0 0), Polygon Empty, LineString(4 6, 7 10))");
}

/***************************************************************************************************/

QTEST_MAIN(TestQcWkb)
#include "test_wkb.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
