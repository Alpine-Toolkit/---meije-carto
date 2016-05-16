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

QcLocationCircleNode::QcLocationCircleNode(const QcViewport * viewport)
  : QSGOpacityNode(),
    m_viewport(viewport)
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

  QSGGeometry::TexturedPoint2D * vertices = location_circle_geometry->vertexDataAsTexturedPoint2D();
  float x = .5 * m_viewport->width(); // Fixme: vector
  float y = .5 * m_viewport->height();
  float radius = 110.;
  vertices[0].set(x - radius, y - radius, -110, -110);
  vertices[1].set(x - radius, y + radius, -110, 110);
  vertices[2].set(x + radius, y - radius, 110, -110);
  vertices[3].set(x + radius, y + radius, 110, 110);

  QSGSimpleMaterial<QcLocationCircleMaterialShaderState> * material = QcLocationCircleMaterialShader::createMaterial();
  material->state()->r = 0;
  material->state()->g = 0;
  material->state()->b = 1;
  material->state()->a = 1.;
  // QSGFlatColorMaterial * material = new QSGFlatColorMaterial();
  // material->setColor(QColor("black"));
  material->setFlag(QSGMaterial::Blending);
  geometry_node->setMaterial(material);
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
