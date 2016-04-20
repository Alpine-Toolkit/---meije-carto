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

#include <QObject>
#include <QQuickWindow>
#include <QSGNode>

/**************************************************************************************************/

#include "map/tile_matrix_set.h"
#include "map/viewport.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcMapScene : public QObject
{
public:
  QcMapScene(QcViewport * viewport, QcTileMatrixSet & tile_matrix_set, QObject * parent = nullptr);
  ~QcMapScene();

  void set_item_size (const QSize & size) { m_item_size = size; };

  QSGNode * update_scene_graph(QSGNode * old_node, QQuickWindow * window);

private:
  QSize m_item_size; // in pixels
  QcViewport * m_viewport;
  QcTileMatrixSet & m_tile_matrix_set;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif // MAP_SCENE_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
