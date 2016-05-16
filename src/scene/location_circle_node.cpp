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

#include "location_circle_node.h"

#include "location_circle_material_shader.h"

#include <QSGFlatColorMaterial>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcLocationCircleNode::QcLocationCircleNode()
  : QSGOpacityNode()
{
  setOpacity(.25); // 1. black

  QSGGeometryNode * geometry_node = new QSGGeometryNode();
  // QSGGeometry * location_circle_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 1);
  // location_circle_geometry->setLineWidth(100); // point size, max is 255
  // location_circle_geometry->setDrawingMode(GL_POINTS);
  QSGGeometry * location_circle_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
  location_circle_geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  geometry_node->setGeometry(location_circle_geometry);
  geometry_node->setFlag(QSGNode::OwnsGeometry);

  QSGGeometry::TexturedPoint2D * location_circle_vertices = location_circle_geometry->vertexDataAsTexturedPoint2D();
  float radius = 110. / 500.;
  location_circle_vertices[0].set(-radius, -radius, -110, -110);
  location_circle_vertices[1].set(-radius, radius, -110, 110);
  location_circle_vertices[2].set(radius, -radius, 110, -110);
  location_circle_vertices[3].set(radius, radius, 110, 110);

  // Fixme: black instead of white
  QSGSimpleMaterial<QcLocationCircleMaterialShaderState> * location_circle_material = QcLocationCircleMaterialShader::createMaterial();
  location_circle_material->state()->r = 0;
  location_circle_material->state()->g = 0;
  location_circle_material->state()->b = 1;
  location_circle_material->state()->a = 1.;
  // QSGFlatColorMaterial * location_circle_material = new QSGFlatColorMaterial();
  // location_circle_material->setColor(QColor("black"));
  location_circle_material->setFlag(QSGMaterial::Blending);
  geometry_node->setMaterial(location_circle_material);
  geometry_node->setFlag(QSGNode::OwnsMaterial);

  appendChildNode(geometry_node);
}

void
QcLocationCircleNode::update()
{
}

/**************************************************************************************************/

// QC_END_NAMESPACE


/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
