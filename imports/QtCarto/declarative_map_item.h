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

#include <QColor>
#include <QGeoCoordinate>
#include <QHash>
#include <QQuickItem>

/**************************************************************************************************/

#include "map/map_view.h"
#include "map/viewport.h"
#include "wmts/wmts_plugin_manager.h"

#include "map_gesture_area.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcWmtsPluginData
{
  Q_GADGET
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString title READ title CONSTANT)

public:
  QcWmtsPluginData();
  QcWmtsPluginData(const QString & name, const QString & title);
  QcWmtsPluginData(const QcWmtsPluginData & other);

  QcWmtsPluginData & operator=(const QcWmtsPluginData & other);

  const QString & name() const { return m_name; }
  const QString & title() const { return m_title; }

private:
  QString m_name;
  QString m_title;
};

Q_DECLARE_METATYPE(QcWmtsPluginData)

/**************************************************************************************************/

class QcWmtsPluginLayerData : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString plugin READ plugin CONSTANT)
  Q_PROPERTY(QString title READ title CONSTANT)
  Q_PROPERTY(bool status READ status WRITE set_status NOTIFY statusChanged)
  Q_PROPERTY(float opacity READ opacity WRITE set_opacity NOTIFY opacityChanged)

public:
  QcWmtsPluginLayerData();
  QcWmtsPluginLayerData(const QcWmtsPluginLayer * plugin_layer);
  QcWmtsPluginLayerData(const QcWmtsPluginLayerData & other);

  QcWmtsPluginLayerData & operator=(const QcWmtsPluginLayerData & other);

  const QcWmtsPluginLayer * plugin_layer() const { return m_plugin_layer; }
  QString plugin() const { return m_plugin_layer->plugin_name(); }
  QString title() const { return m_plugin_layer->title(); }

  bool status() const { return m_status; }
  float opacity() const { return m_opacity; }

  void set_status(bool status);
  void set_opacity(float opacity);

signals:
  void statusChanged(bool status);
  void opacityChanged(float opacity);

private:
  const QcWmtsPluginLayer * m_plugin_layer;
  bool m_status;
  float m_opacity;
};

Q_DECLARE_METATYPE(QcWmtsPluginLayerData)

/**************************************************************************************************/

class QcMapItem : public QQuickItem
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(unsigned int zoom_level READ zoom_level WRITE set_zoom_level NOTIFY zoom_levelChanged)
  Q_PROPERTY(QGeoCoordinate center READ center WRITE set_center NOTIFY centerChanged)
  Q_PROPERTY(QcMapGestureArea * gesture READ gesture CONSTANT)
  Q_PROPERTY(QColor color READ color WRITE set_color NOTIFY colorChanged)
  // Q_PROPERTY(QVariantList plugins READ plugins CONSTANT)

public:
  QcMapItem(QQuickItem *parent = 0);
  ~QcMapItem();

  QcMapGestureArea * gesture();

  void set_color(const QColor & color);
  QColor color() const;

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

  Q_INVOKABLE QVariantList plugins() const;
  Q_INVOKABLE QVariantList plugin_layers(const QString & plugin_name);

  bool childMouseEventFilter(QQuickItem * item, QEvent * event) Q_DECL_OVERRIDE ;

signals:
  void zoom_levelChanged(int zoom_level);
  void centerChanged(const QGeoCoordinate & coordinate);
  void colorChanged(const QColor & color);

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
