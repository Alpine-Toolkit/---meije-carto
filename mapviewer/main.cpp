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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <QtDebug>

/**************************************************************************************************/

#include "configuration/configuration.h"
#include "tools/debug_data.h"
#include "tools/logger.h"
#include "tools/platform.h"

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
  // Fixme:
#ifdef ANDROID
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication application(argc, argv);

  QSurfaceFormat surface_format;
  // surface_format.setSamples(4); // max is 8 ?
  QSurfaceFormat::setDefaultFormat(surface_format);

  QcConfig & config = QcConfig::instance();
  config.init();

  QcDebugData debug_data;
  debug_data.write_json(config.join_application_user_directory(QLatin1Literal("debug_data.json")));
  qInfo() << debug_data.to_json();

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
