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

#include <QHash>
#include <QObject>
#include <QQuickWindow>
#include <QSGNode>
#include <QString>

//#include <QMatrix4x4>
#include <QSGClipNode>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGSimpleMaterial>
#include <QSGSimpleTextureNode>
#include <QSGTransformNode>
#include <QtDebug>

/**************************************************************************************************/

#include "map/file_tile_cache.h"
#include "map/tile_matrix_set.h"
#include "map/tile_spec.h"
#include "map/viewport.h"
#include "map/wmts_plugin.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapLayerRootNode;

/**************************************************************************************************/

class QcMapLayerScene : public QObject
{
public:
  QcMapLayerScene(QcWmtsPluginMap plugin_map, const QcViewport * viewport, QObject * parent = nullptr);
  ~QcMapLayerScene();

  const QString name() const { return m_name; }

  float width() { return m_viewport->width(); }
  float height() { return m_viewport->height(); }

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

private:
  void remove_tiles(const QcTileSpecSet & old_tiles);

public:
  QHash<QcTileSpec, QSharedPointer<QcTileTexture> > m_tile_textures;

private:
  QString m_name;
  const QcTileMatrixSet & m_tile_matrix_set;
  const QcViewport * m_viewport; // Fixme: &
  QcTileSpecSet m_visible_tiles;
  QcTileSpecSet m_west_visible_tiles;
  QcTileSpecSet m_middle_visible_tiles;
  QcTileSpecSet m_east_visible_tiles;
};

/**************************************************************************************************/

class QcMapScene : public QObject
{
public:
  QcMapScene(const QcViewport * viewport, QObject * parent = nullptr);
  ~QcMapScene();

  QSGNode * update_scene_graph(QSGNode * old_node, QQuickWindow * window);

  QcMapLayerScene * add_layer(QcWmtsPluginMap plugin_map);
  void remove_layer(QcWmtsPluginMap plugin_map);

private:
  float width() { return m_viewport->width(); }
  float height() { return m_viewport->height(); }

private:
  const QcViewport * m_viewport; // Fixme: &
  QList<QcMapLayerScene *> m_layers;
};

/**************************************************************************************************/

class QcGridNode : public QSGGeometryNode
{
public:
  QcGridNode(const QcTileMatrixSet & tile_matrix_set, const QcViewport * viewport);

  void update();

private:
  const QcTileMatrixSet & m_tile_matrix_set;
  const QcViewport * m_viewport;
};

/**************************************************************************************************/

class QcMapSideNode : public QSGTransformNode
{
public:
  void add_child(const QcTileSpec & tile_spec, QSGSimpleTextureNode * node);

  QHash<QcTileSpec, QSGSimpleTextureNode *> texture_nodes;
};

/**************************************************************************************************/

class QcMapLayerRootNode : public QSGOpacityNode
{
public:
  QcMapLayerRootNode(const QcTileMatrixSet & tile_matrix_set, const QcViewport * viewport);
  ~QcMapLayerRootNode();

  void update_tiles(QcMapLayerScene * map_scene,
                    QcMapSideNode * map_side_node, const QcTileSpecSet & visible_tiles, const QcPolygon & polygon,
                    double offset);

private:
  const QcTileMatrixSet & m_tile_matrix_set;
  const QcViewport * m_viewport;

public:
  // QcGridNode * grid_node;
  QcMapSideNode * west_map_node;
  QcMapSideNode * middle_map_node;
  QcMapSideNode * east_map_node;
  QHash<QcTileSpec, QSGTexture *> textures;
};

/**************************************************************************************************/

class QcMapRootNode : public QSGClipNode
{
public:
  QcMapRootNode(const QcViewport * viewport);
  ~QcMapRootNode();

  void update_clip_rect();

private:
  const QcViewport * m_viewport;
  QRect m_clip_rect;

public:
  QSGGeometry geometry;
  QSGTransformNode * root;
  QSGOpacityNode * location_circle_root_node;
  QHash<QString, QcMapLayerRootNode *> layers;
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
