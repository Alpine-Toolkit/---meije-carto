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

#ifndef MAP_SCENE_PRIVATE_H
#define MAP_SCENE_PRIVATE_H

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

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

#endif // MAP_SCENE_PRIVATE_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
