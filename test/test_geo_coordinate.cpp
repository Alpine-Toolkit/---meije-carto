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
  QcGeoCoordinateWGS84 wgs84_coordinate1(longitude, latitude);
  QVERIFY(wgs84_coordinate1.longitude() == longitude);
  QVERIFY(wgs84_coordinate1.latitude() == latitude);

  QcGeoCoordinateWGS84 wgs84_coordinate2(wgs84_coordinate1);
  QVERIFY(wgs84_coordinate2.longitude() == longitude);
  QVERIFY(wgs84_coordinate2.latitude() == latitude);

  QcGeoCoordinateMercator mercator_coordinate1 = wgs84_coordinate1.mercator();
  QVERIFY(abs(mercator_coordinate1.x() - x) < 1e-1);
  QVERIFY(abs(mercator_coordinate1.y() - y) < 1e-1);
  QVERIFY(mercator_coordinate1.wgs84() == wgs84_coordinate1);
  // qInfo() << wgs84_coordinate1;
  // qInfo() << mercator_coordinate1;
  // std::cout.precision(10);
  // std::cout << mercator_coordinate1.x() << " " << mercator_coordinate1.y() << std::endl;
  // QVERIFY(qFuzzyCompare(mercator_coordinate1.x(), x));
  // QVERIFY(qFuzzyCompare(mercator_coordinate1.y(), y));

  QcGeoCoordinateMercator mercator_coordinate2;
  wgs84_coordinate1.transform(mercator_coordinate2);
  QVERIFY(mercator_coordinate1 == mercator_coordinate2);
  // std::cout << mercator_coordinate2.x() << " " << mercator_coordinate2.y() << std::endl;
  // QVERIFY(qFuzzyCompare(mercator_coordinate1.x(), mercator_coordinate2.x()));
  // QVERIFY(qFuzzyCompare(mercator_coordinate1.y(), mercator_coordinate2.y()));

  QcGeoCoordinateNormalisedMercator normalised_coordinate1 = wgs84_coordinate1.normalised_mercator();
  QcGeoCoordinateNormalisedMercator normalised_coordinate2 = mercator_coordinate1.normalised_mercator();
  QVERIFY(normalised_coordinate1 == normalised_coordinate2);
  QVERIFY(normalised_coordinate2.wgs84() == wgs84_coordinate1);
  QVERIFY(normalised_coordinate2.mercator() == mercator_coordinate1);
  // std::cout << normalised_coordinate1.x() << " " << normalised_coordinate1.y() << std::endl;
  // std::cout << normalised_coordinate2.x() << " " << normalised_coordinate2.y() << std::endl;
}

/***************************************************************************************************/

QTEST_MAIN(TestQcGeoCoordinateWGS84)
#include "test_geo_coordinate.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
