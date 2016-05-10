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

#include "qtcarto_global.h"
#include "map/map_scene.h"
#include "map/tile_spec.h"
#include "map/wmts_manager.h" // circular
#include "map/wmts_plugin.h" // circular
#include "map/wmts_request_manager.h" // circular
#include "map/viewport.h"

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
  QcMapViewLayer(QcMapView * map_view, QcWmtsPlugin * plugin, int map_id);
  ~QcMapViewLayer();

  QcWmtsPlugin * plugin() { return m_plugin; }
  int map_id() const { return m_map_id; }
  QcWmtsRequestManager * request_manager() { return m_request_manager; };

  void update_tile(const QcTileSpec & tile_spec);
  void update_scene();

 signals:
  void scene_graph_changed();

 private:
  QcTileSpecSet intersec_polygon_with_grid(const QcPolygon & polygon, double tile_length_m, int zoom_level);

 private:
  QcMapView * m_map_view;
  QcViewport * m_viewport;
  QcMapLayerScene * m_map_scene;

  QcWmtsPlugin * m_plugin;
  int m_map_id;

  QPointer<QcWmtsManager> m_wmts_manager; // Fixme: why QPointer !
  QcWmtsRequestManager * m_request_manager;

  QcTileSpecSet m_west_visible_tiles;
  QcTileSpecSet m_middle_visible_tiles;
  QcTileSpecSet m_east_visible_tiles;
  QcTileSpecSet m_visible_tiles;
};

typedef QSet<QcMapViewLayer *> QcMapViewLayerPointerSet;

/**************************************************************************************************/

class QC_EXPORT QcMapView : public QObject
{
  Q_OBJECT

 public:
  QcMapView();
  ~QcMapView();

  QcViewport * viewport() { return m_viewport; };
  QcMapScene * map_scene() { return m_map_scene; };

  void add_layer(QcWmtsPlugin * plugin, int map_id);
  void remove_layer(QcWmtsPlugin * plugin, int map_id);

  void update_tile(const QcTileSpec & tile_spec);

  QSGNode * update_scene_graph(QSGNode *old_node, QQuickWindow *window) {
    return m_map_scene->update_scene_graph(old_node, window);
  }

 signals:
  void scene_graph_changed();

 public slots:
  void update_scene();

 private:
  QcViewport * m_viewport;
  QcMapScene * m_map_scene;
  // QHash<QString, QcMapViewLayer *> m_layers;
  QList<QcMapViewLayer *> m_layers;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __MAP_VIEW_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
