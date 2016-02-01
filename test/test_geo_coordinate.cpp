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

#include <QtTest/QtTest>

/**************************************************************************************************/

#include "geo_coordinate.h"

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
  QVERIFY(coordinate0.latitude() == .0);
  QVERIFY(coordinate0.longitude() == .0);

  double latitude = 2.456;
  double longitude = 3.678;
  QcGeoCoordinateWGS84 coordinate1(latitude, longitude);
  QVERIFY(coordinate1.latitude() == latitude);
  QVERIFY(coordinate1.longitude() == longitude);

  QcGeoCoordinateWGS84 coordinate2(coordinate1);
  QVERIFY(coordinate2.latitude() == latitude);
  QVERIFY(coordinate2.longitude() == longitude);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcGeoCoordinateWGS84)
#include "test_geo_coordinate.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
