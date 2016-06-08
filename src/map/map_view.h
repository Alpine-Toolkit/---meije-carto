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

#ifndef __MAP_VIEW_H__
#define __MAP_VIEW_H__

/**************************************************************************************************/

#include <QList>
#include <QObject>

#include "map/location_circle_data.h"
#include "map/viewport.h"
#include "qtcarto_global.h"
#include "scene/map_scene.h"
#include "wmts/tile_spec.h"
#include "wmts/wmts_plugin.h" // circular
#include "wmts/wmts_request_manager.h" // circular

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

// class QcWmtsManager;
// class QcWmtsRequestManager;

/**************************************************************************************************/

class QcMapView;

class QC_EXPORT QcMapViewLayer : public QObject
{
  Q_OBJECT

 public:
  QcMapViewLayer(const QcWmtsPluginLayer * plugin_layer, QcViewport * m_viewport, QcMapLayerScene * layer_scene);
  ~QcMapViewLayer();

  const QcWmtsPluginLayer * plugin_layer() const { return m_plugin_layer; }
  QcWmtsPlugin * plugin() { return m_plugin_layer->plugin(); }
  int map_id() const { return m_plugin_layer->map_id(); }

  QcWmtsRequestManager * request_manager() { return m_request_manager; };

  float opacity() const;
  void set_opacity(float opacity);

  void update_tile(const QcTileSpec & tile_spec);
  void update_scene();

 signals:
  void scene_graph_changed();

 private:
  QcPolygon transform_polygon(const QcPolygon & polygon); // Fixme: const;
  QcTileSpecSet intersec_polygon_with_grid(const QcPolygon & polygon, double tile_length_m, int zoom_level);

 private:
  const QcWmtsPluginLayer * m_plugin_layer;
  QcViewport * m_viewport;
  QcMapLayerScene * m_layer_scene;

  QcWmtsRequestManager * m_request_manager;

  QcTileSpecSet m_west_visible_tiles;
  QcTileSpecSet m_middle_visible_tiles;
  QcTileSpecSet m_east_visible_tiles;
  QcTileSpecSet m_visible_tiles;
};

// typedef QSet<QcMapViewLayer *> QcMapViewLayerSet;

/**************************************************************************************************/

class QC_EXPORT QcMapView : public QObject
{
  Q_OBJECT

 public:
  QcMapView();
  ~QcMapView();

  QcViewport * viewport() { return m_viewport; };
  const QcProjection & projection() const { return m_viewport->projection(); }
  void set_projection(const QcProjection * projection);

  void add_layer(const QcWmtsPluginLayer * plugin_layer);
  void remove_layer(const QcWmtsPluginLayer * plugin_layer);
  void remove_all_layers();
  float opacity(const QcWmtsPluginLayer * plugin_layer);
  void set_opacity(const QcWmtsPluginLayer * plugin_layer, float opacity);
  QList<const QcWmtsPluginLayer *> layers() const;

  QSGNode * update_scene_graph(QSGNode * old_node, QQuickWindow * window) {
    return m_map_scene->update_scene_graph(old_node, window);
  }

  void update_path(const QcPathDouble & path) {
    m_map_scene->update_path(path);
  }

  QcLocationCircleData & location_circle_data() { return m_location_circle_data; }
  const QcLocationCircleData & location_circle_data() const { return m_location_circle_data; }

 signals:
  void scene_graph_changed();

 public slots:
  void update_scene();

 private:
  QcMapViewLayer * get_layer(const QcWmtsPluginLayer * plugin_layer);

 private:
  QcViewport * m_viewport;
  QcMapScene * m_map_scene;
  QcLocationCircleData m_location_circle_data;
  QList<QcMapViewLayer *> m_layers;
  QHash<QString, QcMapViewLayer *> m_layer_map;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __MAP_VIEW_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
