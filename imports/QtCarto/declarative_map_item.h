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

#include "map_gesture_area.h"
#include "plugin_data.h"

#include "map/map_view.h"
#include "map/viewport.h"
#include "wmts/wmts_plugin_manager.h"

#include <QColor>
#include <QGeoCoordinate>
#include <QHash>
#include <QQuickItem>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapItem : public QQuickItem
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QColor color READ color WRITE set_color NOTIFY colorChanged)
  Q_PROPERTY(QcMapGestureArea * gesture READ gesture CONSTANT)
  Q_PROPERTY(unsigned int zoom_level READ zoom_level WRITE set_zoom_level NOTIFY zoom_levelChanged)
  Q_PROPERTY(QGeoCoordinate center READ center WRITE set_center NOTIFY centerChanged)
  Q_PROPERTY(QVariantList plugins READ plugins CONSTANT)

public:
  QcMapItem(QQuickItem *parent = 0);
  ~QcMapItem();

  QcMapGestureArea * gesture() { return m_gesture_area; }

  void set_color(const QColor & color);
  QColor color() const { return m_color; }

  void set_zoom_level(unsigned int zoom_level);
  unsigned int zoom_level() const;

  void set_center(const QGeoCoordinate & center);
  QGeoCoordinate center() const;

  // QDoubleVector2D / QVector2D use float ...
  Q_INVOKABLE QGeoCoordinate to_coordinate(const QVector2D & position, bool clip_to_viewport = true) const;
  Q_INVOKABLE QVector2D from_coordinate(const QGeoCoordinate & coordinate, bool clip_to_viewport = true) const;

  Q_INVOKABLE void pan(int dx, int dy);
  Q_INVOKABLE void stable_zoom(QPointF position_px, int zoom_level);
  Q_INVOKABLE void stable_zoom_by_increment(QPointF position_px, int zoom_increment);

  Q_INVOKABLE void prefetch_data(); // optional hint for prefetch

  QVariantList plugins() const;
  Q_INVOKABLE QVariantList plugin_layers(const QString & plugin_name);

  bool childMouseEventFilter(QQuickItem * item, QEvent * event) Q_DECL_OVERRIDE ;

signals:
  void colorChanged(const QColor & color);
  void zoom_levelChanged(int zoom_level);
  void centerChanged(const QGeoCoordinate & coordinate);

protected:
  void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE ;
  void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE ;
  void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE ;
  void mouseUngrabEvent() Q_DECL_OVERRIDE ;
  void touchEvent(QTouchEvent * event) Q_DECL_OVERRIDE ;
  void touchUngrabEvent() Q_DECL_OVERRIDE;
  void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE ;

  /* bool sendMouseEvent(QMouseEvent *event); */
  /* bool sendTouchEvent(QTouchEvent *event); */

  void componentComplete() Q_DECL_OVERRIDE ;
  void geometryChanged(const QRectF & new_geometry, const QRectF & old_geometry) Q_DECL_OVERRIDE ;
  QSGNode * updatePaintNode(QSGNode * old_node, UpdatePaintNodeData *) Q_DECL_OVERRIDE ;

private slots:
  void layer_status_changed(bool status);
  void layer_opacity_changed(float opacity);

private:
  bool is_interactive();
  bool send_touch_event(QTouchEvent * event);
  bool send_mouse_event(QMouseEvent * event);
  QVariantList make_plugin_layers(const QString & plugin);

private:
  QColor m_color;
  QcMapGestureArea * m_gesture_area;
  QcWmtsPluginManager & m_plugin_manager;
  QcMapView * m_map_view;
  QcViewport * m_viewport; // ???
  QHash<QString, QVariantList> m_plugin_layers;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif // DECLARATIVE_MAP_ITEM_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
