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

#ifndef MAP_SCENE_H
#define MAP_SCENE_H

/**************************************************************************************************/

#include "cache/file_tile_cache.h"
#include "map/viewport.h"
#include "wmts/tile_matrix_set.h"
#include "wmts/tile_spec.h"
#include "wmts/wmts_plugin.h"

#include <QHash>
#include <QObject>
#include <QQuickWindow>
#include <QSGNode>
#include <QString>

#include <QSGClipNode>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGSimpleMaterial>
#include <QSGSimpleTextureNode>
#include <QSGTransformNode>
#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapLayerRootNode;

/**************************************************************************************************/

class QcMapLayerScene : public QObject
{
public:
  QcMapLayerScene(const QcWmtsPluginLayer * plugin_layer, const QcViewport * viewport, QObject * parent = nullptr);
  ~QcMapLayerScene();

  QString name() const { return m_plugin_layer->hash_name(); }
  const QcWmtsPluginLayer * plugin_layer() const { return m_plugin_layer; }

  float width() { return m_viewport->width(); }
  float height() { return m_viewport->height(); }

  float opacity() const { return m_opacity; };
  void set_opacity(float opacity) { m_opacity = opacity; };

  void add_tile(const QcTileSpec & tile_spec, QSharedPointer<QcTileTexture> texture);

  void set_visible_tiles(const QcTileSpecSet & tile_specs,
                         const QcTileSpecSet & west_tile_specs,
                         const QcTileSpecSet & middle_tile_specs,
                         const QcTileSpecSet & east_tile_specs);
  const QcTileSpecSet & visible_tiles() const { return m_visible_tiles; };
  QcTileSpecSet textured_tiles() const;

  QcMapLayerRootNode * make_node();
  void update_scene_graph(QcMapLayerRootNode * map_root_node, QQuickWindow * window);
  bool build_geometry(const QcTileSpec & tile_spec, QSGGeometry::TexturedPoint2D * vertices, const QcPolygon & polygon);

  // Fixme: protected
  QcMapLayerRootNode * scene_graph_node() { return m_scene_graph_node; }

private:
  void remove_tiles(const QcTileSpecSet & old_tiles);

public:
  QHash<QcTileSpec, QSharedPointer<QcTileTexture> > m_tile_textures;

private:
  const QcWmtsPluginLayer * m_plugin_layer;
  const QcViewport * m_viewport; // Fixme: &

  const QcTileMatrixSet & m_tile_matrix_set;

  QcTileSpecSet m_visible_tiles;
  QcTileSpecSet m_west_visible_tiles;
  QcTileSpecSet m_middle_visible_tiles;
  QcTileSpecSet m_east_visible_tiles;

  float m_opacity;

  QcMapLayerRootNode * m_scene_graph_node;
};

/**************************************************************************************************/

class QcMapScene : public QObject
{
public:
  QcMapScene(const QcViewport * viewport, QObject * parent = nullptr);
  ~QcMapScene();

  QSGNode * update_scene_graph(QSGNode * old_node, QQuickWindow * window);

  QcMapLayerScene * add_layer(const QcWmtsPluginLayer * plugin_layer);
  void remove_layer(const QcWmtsPluginLayer * plugin_layer);

  void update_path(const QcPathDouble & path) {
    m_path = path;
    m_dirty_path = true;
  }

private:
  float width() { return m_viewport->width(); }
  float height() { return m_viewport->height(); }

private:
  const QcViewport * m_viewport; // Fixme: &
  QList<QcMapLayerScene *> m_layers;
  QHash<QString, QcMapLayerScene *> m_layer_map;
  QList<QSGNode *> m_scene_graph_nodes_to_remove;

  QcPathDouble m_path;
  bool m_dirty_path;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif // MAP_SCENE_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
