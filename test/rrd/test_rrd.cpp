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

#include "rrd/rrd.h"

#include <QString>
#include <QDataStream>
#include <QtDebug>

/**************************************************************************************************/

struct MyRoundRobinDatabaseSlot
{
  quint64 index;
  double x;
  double y;
};
typedef struct MyRoundRobinDatabaseSlot  MyRoundRobinDatabaseSlot;

class MyRoundRobinDatabase : public QcRoundRobinDatabase
{
public:
  MyRoundRobinDatabase(const QString & path)
    : QcRoundRobinDatabase(path)
  {}
  MyRoundRobinDatabase(const QString & path, size_t number_of_slots)
    : QcRoundRobinDatabase(path, number_of_slots, QLatin1Literal("qdd"))
  {}

  void write (const MyRoundRobinDatabaseSlot & data) {
    QByteArray buffer;
    buffer.reserve(slot_size()); // Fixme: required?
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << data.index << data.x << data.y;
    QcRoundRobinDatabase::write(buffer);
  }

  void read (size_t position, MyRoundRobinDatabaseSlot & data) {
    QByteArray buffer;
    buffer.resize(slot_size());
    QcRoundRobinDatabase::read(position, buffer);
    QDataStream stream(&buffer, QIODevice::ReadOnly);
    stream >> data.index >> data.x >> data.y;
  }
};

/***************************************************************************************************/

class TestQcRrd: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

void TestQcRrd::constructor()
{
  QString path("test.rrd");

  size_t number_of_slots = 100;
  MyRoundRobinDatabase output_rrd(path, number_of_slots);
  size_t number_of_iterations = 10; // number_of_slots + 10;
  for (size_t i = 0; i < number_of_iterations; i++) {
    MyRoundRobinDatabaseSlot slot = { .index = i, .x = 1.1 * i, .y = 2.2 * i};
    output_rrd.write(slot);
  }
  output_rrd.flush();

  MyRoundRobinDatabase input_rrd(path);
  for (size_t i = 0; i < number_of_iterations; i++) {
    MyRoundRobinDatabaseSlot slot;
    output_rrd.read(i, slot);
    qInfo() << "@" << i << slot.index << slot.x << slot.y;
  }
}

/***************************************************************************************************/

QTEST_MAIN(TestQcRrd)
#include "test_rrd.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
