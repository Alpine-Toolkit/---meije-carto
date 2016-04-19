// -*- mode: c++ -*-

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

#ifndef DECLARATIVE_MAP_ITEM_H
#define DECLARATIVE_MAP_ITEM_H

/**************************************************************************************************/

#include <QGeoCoordinate>
#include <QQuickItem>

/**************************************************************************************************/

#include "map/geoportail/geoportail_plugin.h"
#include "map/viewport.h"
#include "map_gesture_area.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcMapItem : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(int zoom_level READ zoom_level WRITE set_zoom_level NOTIFY zoom_levelChanged)
  Q_PROPERTY(QGeoCoordinate center READ center WRITE set_center NOTIFY centerChanged)

  // Q_PROPERTY(QcMapGestureArea * gesture READ gesture CONSTANT)
  // Q_INTERFACES(QQmlParserStatus)

public:
  QcMapItem(QQuickItem *parent = 0);
  ~QcMapItem();

  void set_zoom_level(int zoom_level);
  int zoom_level() const;

  void set_center(const QGeoCoordinate & center);
  QGeoCoordinate center() const;

  Q_INVOKABLE QGeoCoordinate to_coordinate(const QPointF & position, bool clip_to_view_port = true) const;
  Q_INVOKABLE QPointF from_coordinate(const QGeoCoordinate & coordinate, bool clip_to_view_port = true) const;

  // QDoubleVector2D / QVector2D use float ...
  QGeoCoordinate item_position_to_coordinate(const QVector2D & pos, bool clip_to_viewport = true) const;
  QVector2D coordinate_to_item_position(const QGeoCoordinate & coordinate, bool clip_to_viewport = true) const;

  Q_INVOKABLE void prefetch_data(); // optional hint for prefetch

  QcMapGestureArea * gesture();

  Q_INVOKABLE void pan(int dx, int dy);

signals:
  void zoom_levelChanged(int zoom_level);
  void centerChanged(const QGeoCoordinate & coordinate);

protected:
  void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE ;
  void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE ;
  void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE ;
  void mouseUngrabEvent() Q_DECL_OVERRIDE ;
  void touchUngrabEvent() Q_DECL_OVERRIDE;
  void touchEvent(QTouchEvent * event) Q_DECL_OVERRIDE ;
  void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE ;

  /* bool sendMouseEvent(QMouseEvent *event); */
  /* bool sendTouchEvent(QTouchEvent *event); */

  void componentComplete() Q_DECL_OVERRIDE;
  QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *);
  void geometryChanged(const QRectF & new_geometry, const QRectF & old_geometry);

private:
    bool is_interactive();

private:
  QcWmtsPlugin * m_plugin;
  QcViewport * m_viewport;
  QcMapGestureArea * m_gesture_area;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif // DECLARATIVE_MAP_ITEM_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
