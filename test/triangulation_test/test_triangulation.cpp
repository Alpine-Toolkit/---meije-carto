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

#include <QtDebug>
#include <QtTest/QtTest>

/**************************************************************************************************/

// #include "geometry/"

#include "polygon_seidler_triangulation.h"

/***************************************************************************************************/

class TestQcTriangulation: public QObject
{
  Q_OBJECT

private slots:
  void triangulation();
};

void
TestQcTriangulation::triangulation()
{
  int number_of_vertexes = 7;
  int number_of_segments = 100;

  double vertices[number_of_vertexes][2] = {
    0.0, 0.0,
    10.0, 0.0,
    20.0, 0.0,
    20.0, 5.0,
    20.0, 20.0,
    0.0, 20.0,
    5.0, 10.0
  };

  int triangles[number_of_segments][3];
  for (int i = 0; i < number_of_segments; i++) {
    for (int j = 0; j < 3; j++)
      triangles[i][j] = 0;
  }

  int rc = triangulate_polygon(number_of_vertexes, vertices, triangles);
  // for (int i = 0; i < number_of_segments; i++)
  //   qInfo() << i << triangles[i][0] << triangles[i][1] << triangles[i][2];
}

/***************************************************************************************************/

QTEST_MAIN(TestQcTriangulation)
#include "test_triangulation.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
