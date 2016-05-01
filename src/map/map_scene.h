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

/**************************************************************************************************/

#include "map/file_tile_cache.h"
#include "map/tile_matrix_set.h"
#include "map/tile_spec.h"
#include "map/viewport.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcMapScene : public QObject
{
public:
  QcMapScene(const QcViewport * viewport, const QcTileMatrixSet & tile_matrix_set, QObject * parent = nullptr);
  ~QcMapScene();

  void add_tile(const QcTileSpec & tile_spec, QSharedPointer<QcTileTexture> texture);

  void set_visible_tiles(const QcTileSpecSet & tile_specs,
                         const QcTileSpecSet & west_tile_specs,
                         const QcTileSpecSet & middle_tile_specs,
                         const QcTileSpecSet & east_tile_specs);
  const QcTileSpecSet & visible_tiles() const { return m_visible_tiles; };
  QcTileSpecSet textured_tiles() const;

  QSGNode * update_scene_graph(QSGNode * old_node, QQuickWindow * window);

  float width() { return m_viewport->width(); }
  float height() { return m_viewport->height(); }

  bool build_geometry(const QcTileSpec & tile_spec, QSGGeometry::TexturedPoint2D * vertices, const QcPolygon & polygon);

private:
  void remove_tiles(const QcTileSpecSet & old_tiles);

public:
  QHash<QcTileSpec, QSharedPointer<QcTileTexture> > m_tile_textures;

private:
  const QcViewport * m_viewport; // Fixme: &
  const QcTileMatrixSet & m_tile_matrix_set;
  QcTileSpecSet m_visible_tiles;
  QcTileSpecSet m_west_visible_tiles;
  QcTileSpecSet m_middle_visible_tiles;
  QcTileSpecSet m_east_visible_tiles;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif // MAP_SCENE_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
