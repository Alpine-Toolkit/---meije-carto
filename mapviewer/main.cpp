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

#include <cstdio>
#include <cstdlib>

#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtDebug>

/**************************************************************************************************/

#include "tools/logger.h"

/**************************************************************************************************/

QDir
create_user_application_directory()
{
  // on Android
  //   DataLocation = /data/data/org.alpine_toolkit
  //   GenericDataLocation = <USER> = /storage/emulated/0 = user land root
  // on Linux
  //   GenericDataLocation = /home/fabrice/.local/share
  QString generic_data_location_path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
  qInfo() << "GenericDataLocation:" << generic_data_location_path;
  // /storage/emulated/0/Android/data/org.xxx/files Alarms Download Notifications cache

  QString application_user_directory_path = QDir(generic_data_location_path).filePath("alpine-toolkit");
  QDir application_user_directory(application_user_directory_path);
  if (! application_user_directory.exists()) {
    if (!application_user_directory.mkpath(application_user_directory.absolutePath())) {
      qWarning() << "Cannot create application user directory";
    }
  }

  return application_user_directory;
}

/**************************************************************************************************/

int
main(int argc, char *argv[])
{
#ifndef ANDROID
  qInstallMessageHandler(message_handler);
#endif

  // QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Alpine Toolkit "));
  QGuiApplication::setApplicationName("Alpine Toolkit");
  QGuiApplication::setOrganizationName("FabriceSalvaire"); // overridden ???
  // QGuiApplication::setOrganizationDomain("fabrice-salvaire.fr")
  // QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication application(argc, argv);

  create_user_application_directory();

  QQmlApplicationEngine engine;
  engine.load(QUrl("qrc:/pages/main.qml"));
  if (engine.rootObjects().isEmpty())
    return EXIT_FAILURE;

  return application.exec();
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
