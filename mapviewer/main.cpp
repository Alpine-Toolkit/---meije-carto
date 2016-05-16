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

#include <QDateTime>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickView>
#include <QStandardPaths>
#include <QSurfaceFormat>
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

  QSurfaceFormat surface_format;
  // surface_format.setSamples(4); // max is 8 ?
  QSurfaceFormat::setDefaultFormat(surface_format);

  create_user_application_directory();

  QUrl main_page("qrc:/pages/main.qml");

  QQmlApplicationEngine engine;
  engine.load(main_page);
  if (engine.rootObjects().isEmpty())
    return EXIT_FAILURE;

  for (auto * obj : engine.rootObjects()) {
    QQuickWindow * window = qobject_cast<QQuickWindow *>(obj);
    if (window != NULL) {
      QSurfaceFormat format = window->format();
      qInfo() << "QQuickWindow found" << format;
      // QQuickWindow found QSurfaceFormat(version 2.0, options QFlags(),
      // depthBufferSize 24,
      // redBufferSize 8, greenBufferSize 8, blueBufferSize 8, alphaBufferSize 0,
      // stencilBufferSize 8,
      // samples 4, swapBehavior 2, swapInterval 1, profile  0)
    }
  }

  return application.exec();
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
