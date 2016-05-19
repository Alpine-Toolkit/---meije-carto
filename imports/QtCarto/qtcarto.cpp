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

#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/qqml.h>

/**************************************************************************************************/

#include "declarative_map_item.h"
#include "map_gesture_area.h"

// Fixme:
#include "coordinate/geo_coordinate.h"
#include "map/map_event_router.h"

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QtCartoDeclarativeModule : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
  // Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0"
  //                   FILE "plugin.json")

 public:
  void registerTypes(const char *uri)
  {
    // Q_ASSERT(uri == QLatin1String("fr.alpine_toolkit.QtCarto"));
    if (QLatin1String(uri) == QLatin1String("QtCarto")) { // fr.alpine_toolkit.

      // @uri QtCarto
      int major = 1;
      int minor = 0;

      // Register the 1.0 types

      qRegisterMetaType<QcGeoCoordinateWGS84>();
      QMetaType::registerEqualsComparator<QcGeoCoordinateWGS84>();
      qRegisterMetaType<QcGeoCoordinateWebMercator>();
      QMetaType::registerEqualsComparator<QcGeoCoordinateWebMercator>();
      qRegisterMetaType<QcGeoCoordinatePseudoWebMercator>();
      QMetaType::registerEqualsComparator<QcGeoCoordinatePseudoWebMercator>();
      qRegisterMetaType<QcGeoCoordinateNormalisedWebMercator>();
      // qRegisterAnimationInterpolator<QGeoCoordinate>(q_coordinateInterpolator);

      qRegisterMetaType<QcMapEvent>();
      qRegisterMetaType<QcMapScale>();
      qRegisterMetaType<QcWmtsPluginData>();
      qRegisterMetaType<QcWmtsPluginLayerData>();

      // QDeclarativeGeoMap
      qmlRegisterType<QcMapItem>(uri, major, minor, "QcMapItem");

      qmlRegisterUncreatableType<QcMapPinchEvent>(uri, major, minor, "MapPinchEvent",
                                                  QStringLiteral("(Map)PinchEvent is not intended instantiable by developer."));
      qmlRegisterUncreatableType<QcMapGestureArea>(uri, major, minor, "MapGestureArea",
                                                   QStringLiteral("(Map)GestureArea is not intended instantiable by developer."));

      qmlRegisterUncreatableType<QcMapEventRouter>(uri, major, minor, "QcMapEventRouter",
                                                   QStringLiteral("QcMapEventRouter is not intended instantiable by developer."));

      // registrations below are version independent
    } else {
      qDebug() << "Unsupported URI given to load location QML plugin: " << QLatin1String(uri);
    }
  }
};

/**************************************************************************************************/

#include "qtcarto.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
