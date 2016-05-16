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

#include <QSqlDatabase>
#include <QSqlError> // Fixme: ???
#include <QSqlQuery>
#include <QtDebug>

/**************************************************************************************************/

int
main(int argc, char *argv[])
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "db1");
   db.setDatabaseName("db.sqlite");
   if (!db.open()) {
     qWarning() << "cannot open db";
     return 1;
   }

   QSqlQuery query;

   query = db.exec("SELECT load_extension('mod_spatialite')");
   qWarning() << query.size() << db.lastError();

   query = db.exec("SELECT spatialite_version()");
   query.first();
   qWarning() << query.size() << db.lastError();
   qInfo() << query.value(0).toString();

   query = db.exec("INSERT INTO places (name, geometry) values (\"place3\", MakePoint(4.1, 3.2, 4326))");
   qWarning() << db.lastError();

   // query = db.exec("INSERT INTO places (name, geometry) values (\"place4\", \"\x00\x01\xE6\x10\x00\x00\x9A\x99\x99\x99\x99\x99\xF1?\x9A\x99\x99\x99\x99\x99\x01@\x9A\x99\x99\x99\x99\x99\xF1?\x9A\x99\x99\x99\x99\x99\x01@|\x01\x00\x00\x00\x9A\x99\x99\x99\x99\x99\xF1?\x9A\x99\x99\x99\x99\x99\x01@\xFE\"");
   // qWarning() << db.lastError();

   query = db.exec("SELECT name, geometry FROM places");
   qWarning() << query.size() << db.lastError();
   while (query.next()) {
     // QVariant(QByteArray,
     qInfo() << query.value(0).toString() << query.value(1);
   }

   query = db.exec("SELECT name, AsText(geometry) FROM places");
   qWarning() << query.size() << db.lastError();
   while (query.next()) {
     // QVariant(QByteArray,
     qInfo() << query.value(0).toString() << query.value(1);
   }

   // QSqlQuery query("SELECT name FROM places", db);
   // while (query.next()) {
   //   qInfo() << query.value(0).toString();
   // }

   // if (!query.exec()) {
   //   qWarning() << query.lastError(); // db.
   //   return 1;
   // }
}

/***************************************************************************************************/

// #include "spatialite-test.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
