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

#include <iostream>

#include <QtTest/QtTest>

/**************************************************************************************************/

#include "math/line.h"

/***************************************************************************************************/

class TestQcLine: public QObject
{
  Q_OBJECT
private slots:
  void constructor();
};

void TestQcLine::constructor()
{
  QcLineDouble line1(QcVectorDouble(1., 10.), QcVectorDouble(2., 3.));

  const QcVectorDouble & point = line1.point();
  QVERIFY(point.x() == 1.);
  QVERIFY(point.y() == 10.);
  const QcVectorDouble & vector = line1.vector();
  QVERIFY(vector.x() == 2.);
  QVERIFY(vector.y() == 3.);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcLine)
#include "test_line.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
