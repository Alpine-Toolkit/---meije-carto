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

#include "map/viewport.h"
#include "map/earth.h"

/***************************************************************************************************/

class TestQcViewport: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

void TestQcViewport::constructor()
{
  QcGeoCoordinateNormalisedMercator coordinate(0, 0);
  QcTiledZoomLevel tiled_zoom_level(EQUATORIAL_PERIMETER, 256, 0); // map can have different tile size !
  QcViewportState viewport_state(coordinate, tiled_zoom_level, 0);
  QSize viewport_size(2000, 1000);
  QcViewport viewport(viewport_state, viewport_size);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcViewport)
#include "test_viewport.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
