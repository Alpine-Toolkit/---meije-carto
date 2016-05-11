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

#ifndef __GRID_NODE_H__
#define __GRID_NODE_H__

/**************************************************************************************************/

#include <QSGGeometry>

/**************************************************************************************************/

#include "map/viewport.h"
#include "map/wmts/tile_matrix_set.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

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

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GRID_NODE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
