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

#include "math/polygon.h"

/***************************************************************************************************/

class TestQcPolygon: public QObject
{
  Q_OBJECT

private slots:
  void contains();
  void intersec_with_grid();
};

void
TestQcPolygon::contains()
{
  QcPolygon polygon(QVector<double>({1, 3, 6, 14, 9, 1, 17, 3, 12, 10, 5, 7, 12, 5}));

  QVERIFY(polygon.contains(QcVectorDouble(0, 0)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(100, 100)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(1, 3)) == true);
  QVERIFY(polygon.contains(QcVectorDouble(4, 5)) == true);
  QVERIFY(polygon.contains(QcVectorDouble(5, 7)) == true);
  QVERIFY(polygon.contains(QcVectorDouble(6, 7)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(7, 7)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(8, 7)) == true);
  QVERIFY(polygon.contains(QcVectorDouble(9, 5)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(10, 5)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(11, 5)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(12, 5)) == true);
  QVERIFY(polygon.contains(QcVectorDouble(12, 10)) == true);
  QVERIFY(polygon.contains(QcVectorDouble(13, 10)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(12, 11)) == false);
  QVERIFY(polygon.contains(QcVectorDouble(17, 3)) == true);
}

void
TestQcPolygon::intersec_with_grid()
{
  QVector<double> vertexes = {
    1.2, 1.2,
    0.2, 5.2,
    2.2, 7.2,
    5.2, 2.2,
    7.2, 8.2,
    10.2, 8.2,
    13.2, 10.2,
    15.2, 7.2,
    18.2, 6.2,
    18.2, 4.2,
    15.2, 3.2,
  };
  QcPolygon polygon(vertexes);
  double grid_step = 1.;
  polygon.intersec_with_grid(grid_step);

  // 1 1 6
  // 2 0 13
  // 3 0 17
  // 4 0 18
  // 5 0 3
  // 5 6 18
  // 6 0 2
  // 6 6 18
  // 7 1 2
  // 7 6 15
  // 8 7 14
  // 9 11 13
  // 10 12 13
}

/***************************************************************************************************/

QTEST_MAIN(TestQcPolygon)
#include "test_polygon.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
