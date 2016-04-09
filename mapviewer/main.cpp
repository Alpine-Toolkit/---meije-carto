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

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtDebug>

/**************************************************************************************************/

int
main(int argc, char *argv[])
{
  QGuiApplication application(argc, argv);
  QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", "QtCarto"));

  // Q_INIT_RESOURCE(mapviewer);

  // QQuickView class provides a window for displaying a Qt Quick user interface
  // QQuickView view;
  // QSurfaceFormat format = view.format();
  // format.setSamples(16);
  // view.setFormat(format);
  // view.setSource(QUrl("qrc:///..."));
  // view.show();

  // QQmlApplicationEngine provides a convenient way to load an application from a single QML file
  QQmlApplicationEngine engine;
  // engine.addImportPath(QStringLiteral(":/imports"));
  engine.load(QUrl(QStringLiteral("qrc:///mapviewer.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  // QObject::connect(&engine, SIGNAL(quit()), qApp, SLOT(quit()));

  // QObject * qml_application = engine.rootObjects().first();

  return application.exec();
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
