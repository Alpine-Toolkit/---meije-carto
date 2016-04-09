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
#include <QtDebug>

/**************************************************************************************************/

#include "map/cache3q.h"

/***************************************************************************************************/

class TestQcCache3Q: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

class MyObject: public QObject
{
  Q_OBJECT

public:
  MyObject(int value) : QObject(), value(value) {}

  int value;
};

void TestQcCache3Q::constructor()
{
  size_t number_of_items = 10;
  QcCache3Q<int, MyObject> cache(number_of_items, -1, -1);
  QVector<QSharedPointer<MyObject> > data_ptr(number_of_items);

  for (size_t i = 0; i < number_of_items; i++ ) {
    QSharedPointer<MyObject> shared_ptr(new MyObject(i));
    data_ptr[i] = shared_ptr;
    cache.insert(i, shared_ptr);
  }

  cache.print_stats();

  for (size_t i = 0; i < number_of_items; i++ ) {
    qInfo() << cache[i].data()->value;
    QVERIFY(cache[i].data()->value == data_ptr[i]->value);
  }

  QVERIFY(cache.total_cost() == number_of_items);
  cache.remove(0);
  QVERIFY(cache.total_cost() == number_of_items - 1);
  cache.clear();
  QVERIFY(cache.total_cost() == 0);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcCache3Q)
#include "test_cache3q.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
