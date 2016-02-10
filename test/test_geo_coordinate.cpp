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

#include <cmath>
#include <iostream>

#include <QtTest/QtTest>
#include <QtDebug>

/**************************************************************************************************/

#include "map/geo_coordinate.h"

/***************************************************************************************************/

class TestQcGeoCoordinateWGS84: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

void TestQcGeoCoordinateWGS84::constructor()
{
  QcGeoCoordinateWGS84 coordinate0;
  QVERIFY(coordinate0.longitude() == .0);
  QVERIFY(coordinate0.latitude() == .0);

  // echo '2.478917 48.805639' | cs2cs -f "%.2f" +init=epsg:4326 +to +init=epsg:3857
  // 275951.78 6241946.52 0.00
  double longitude = 2.478917;
  double latitude = 48.805639;
  double x = 275951.7782; // 275951.78
  double y = 6241946.516; // 6241946.52
  QcGeoCoordinateWGS84 coordinate1(longitude, latitude);
  QVERIFY(coordinate1.longitude() == longitude);
  QVERIFY(coordinate1.latitude() == latitude);

  QcGeoCoordinateWGS84 coordinate2(coordinate1);
  QVERIFY(coordinate2.longitude() == longitude);
  QVERIFY(coordinate2.latitude() == latitude);

  QcGeoCoordinateMercator coordinate3 = coordinate1.mercator();
  // qInfo() << coordinate1;
  // qInfo() << coordinate3;
  // std::cout.precision(10);
  // std::cout << coordinate3.x() << " " << coordinate3.y() << std::endl;
  // QVERIFY(qFuzzyCompare(coordinate3.x(), x));
  // QVERIFY(qFuzzyCompare(coordinate3.y(), y));
  QVERIFY(abs(coordinate3.x() - x) < 1e-1);
  QVERIFY(abs(coordinate3.y() - y) < 1e-1);

  QcGeoCoordinateMercator coordinate4;
  coordinate1.transform(coordinate4);
  // std::cout << coordinate4.x() << " " << coordinate4.y() << std::endl;
  // QVERIFY(qFuzzyCompare(coordinate3.x(), coordinate4.x()));
  // QVERIFY(qFuzzyCompare(coordinate3.y(), coordinate4.y()));
  QVERIFY(coordinate3 == coordinate4);

  QcGeoCoordinateNormalisedMercator normalised_coordinate1 = coordinate1.normalised_mercator();
  QcGeoCoordinateNormalisedMercator normalised_coordinate3 = coordinate3.normalised_mercator();
  // std::cout << normalised_coordinate1.x() << " " << normalised_coordinate1.y() << std::endl;
  // std::cout << normalised_coordinate3.x() << " " << normalised_coordinate3.y() << std::endl;
  QVERIFY(normalised_coordinate1 == normalised_coordinate3);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcGeoCoordinateWGS84)
#include "test_geo_coordinate.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
