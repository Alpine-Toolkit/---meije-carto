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

#include "geometry/path.h"

/***************************************************************************************************/

class TestQcPath: public QObject
{
  Q_OBJECT

private slots:
  void length();
};

void
TestQcPath::length()
{
  double l = 10;
  QcPath path(QVector<double>({0, 0,  0, l,  l, l,  l, 0}), true);
  for (const auto & vertex : path.vertexes())
    qInfo() << vertex;
  for (const auto & edge : path.edges())
    qInfo() << edge.p1() << edge.p2();
  QVERIFY(path.length() == 4*l);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcPath)
#include "test_path.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
