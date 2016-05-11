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

#include "grid_node.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcGridNode::QcGridNode(const QcTileMatrixSet & tile_matrix_set, const QcViewport * viewport)
  : QSGGeometryNode(),
    m_tile_matrix_set(tile_matrix_set),
    m_viewport(viewport)
{
  // A geometry node must have both geometry and a normal material before it is added to the scene graph.

  setGeometry(new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0));
  setFlag(QSGNode::OwnsGeometry); // The node is owned by its parent and will be deleted when the parent is deleted.

  QSGFlatColorMaterial * grid_material = new QSGFlatColorMaterial;
  grid_material->setColor(QColor(255, 0, 0));
  setMaterial(grid_material);
  setFlag(QSGNode::OwnsMaterial);
}

void
QcGridNode::update()
{
  int tile_size = m_tile_matrix_set.tile_size();
  const QcTileMatrix & tile_matrix = m_tile_matrix_set[m_viewport->zoom_level()];
  double tile_length_m = tile_matrix.tile_length_m();
  double resolution = tile_matrix.resolution(); // [m/px]
  qInfo() << "tile_matrix" << m_viewport->zoom_level() << tile_length_m << resolution;

  const QcPolygon & polygon = m_viewport->middle_polygon(); // Fixme: when cross date line
  const QcInterval2DDouble & interval = polygon.interval();

  double x_inf_m = interval.x().inf();
  double y_inf_m = interval.y().inf();

  // Use QcInterval2D ???
  int x_grid_inf = ceil(x_inf_m / tile_length_m);
  int y_grid_inf = ceil(y_inf_m / tile_length_m);
  int x_grid_sup = ceil(interval.x().sup() / tile_length_m);
  int y_grid_sup = ceil(interval.y().sup() / tile_length_m);
  size_t number_of_vertical_lines = x_grid_sup - x_grid_inf +1;
  size_t number_of_horizontal_lines = y_grid_sup - y_grid_inf +1;

  const int vertex_count = 2*(number_of_horizontal_lines + number_of_vertical_lines);
  QSGGeometry * grid_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertex_count);
  grid_geometry->setLineWidth(2);
  grid_geometry->setDrawingMode(GL_LINES);

  QSGGeometry::Point2D * vertices = grid_geometry->vertexDataAsPoint2D();
  double x_inf_px = x_inf_m / resolution;
  double y_inf_px = y_inf_m / resolution;
  double x_offset = x_grid_inf * tile_size - x_inf_px;
  double y_offset = y_grid_inf * tile_size - y_inf_px;
  int width = m_viewport->width();
  int height = m_viewport->height();
  size_t vertex_index = 0;
  for (size_t i = 0; i < number_of_vertical_lines; i++) {
    float x = i * tile_size + x_offset;
    vertices[vertex_index].set(x, 0);
    vertices[vertex_index +1].set(x, height);
    vertex_index += 2;
  }
  for (size_t i = 0; i < number_of_horizontal_lines; i++) {
    float y = i * tile_size + y_offset;
    vertices[vertex_index].set(0, y);
    vertices[vertex_index +1].set(width, y);
    vertex_index += 2;
  }

  setGeometry(grid_geometry); // delete old one
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
