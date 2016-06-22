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

struct LocationCirclePoint2D {
  float x;
  float y;
  float u;
  float v;
  float cone_radius;
  float dot_radius;
  float accuracy_radius;
  float angle;
  // float r;
  // float g;
  // float b;
  // float a;

  void set(const QcVectorDouble & point,
           const QcVectorDouble & uv,
           float _cone_radius,
           float _dot_radius,
           float _accuracy_radius,
           float _angle
           ) {
    x = point.x();
    y = point.y();
    u = uv.x();
    v = uv.y();
    cone_radius = _cone_radius;
    dot_radius = _dot_radius;
    accuracy_radius = _accuracy_radius;
    angle = _angle;
    // r;
    // g;
    // b;
    // a;
  }
};

QSGGeometry::Attribute LocationCirclePoint2D_Attributes[] = {
  QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),  // xy
  QSGGeometry::Attribute::create(1, 2, GL_FLOAT, false), // uv
  QSGGeometry::Attribute::create(2, 3, GL_FLOAT, false), // radius
  QSGGeometry::Attribute::create(3, 1, GL_FLOAT, false) // angle
  // QSGGeometry::Attribute::create(5, 4, GL_FLOAT, false)  // colour
};

QSGGeometry::AttributeSet LocationCirclePoint2D_AttributeSet = {
    4, // count Fixme: ???
    sizeof(LocationCirclePoint2D), // stride
    LocationCirclePoint2D_Attributes
};

/**************************************************************************************************/

QcLocationCircleNode::QcLocationCircleNode(const QcViewport * viewport)
  : QSGOpacityNode(),
    m_viewport(viewport),
    m_geometry_node(new QSGGeometryNode())
{
  setOpacity(.25); // 1. black

  QSGGeometry * geometry = new QSGGeometry(LocationCirclePoint2D_AttributeSet, 0); // Fixme:
  geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  m_geometry_node->setGeometry(geometry);
  m_geometry_node->setFlag(QSGNode::OwnsGeometry);

  QSGSimpleMaterial<QcLocationCircleMaterialShaderState> * material = QcLocationCircleMaterialShader::createMaterial();
  material->state()->cone_r = 0; // Fixme: QColor
  material->state()->cone_g = 0;
  material->state()->cone_b = 1;
  material->state()->cone_a = 1.;
  material->state()->accuracy_r = 1;
  material->state()->accuracy_g = 0;
  material->state()->accuracy_b = 0;
  material->state()->accuracy_a = 1.;
  // QSGFlatColorMaterial * material = new QSGFlatColorMaterial();
  // material->setColor(QColor("black"));
  material->setFlag(QSGMaterial::Blending);
  m_geometry_node->setMaterial(material);
  m_geometry_node->setFlag(QSGNode::OwnsMaterial);

  appendChildNode(m_geometry_node);
}

void
QcLocationCircleNode::update(const QcLocationCircleData & location_circle_data)
{
  // QSGGeometry * location_circle_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 1);
  // location_circle_geometry->setLineWidth(100); // point size, max is 255
  // location_circle_geometry->setDrawingMode(GL_POINTS);

  QSGGeometry * geometry = new QSGGeometry(LocationCirclePoint2D_AttributeSet, 4);
  geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  m_geometry_node->setGeometry(geometry);
  m_geometry_node->setFlag(QSGNode::OwnsGeometry);

  LocationCirclePoint2D * vertices = static_cast<LocationCirclePoint2D *>(geometry->vertexData());
  float x = .5 * m_viewport->width(); // Fixme: vector
  float y = .5 * m_viewport->height();
  float accuracy_radius = m_viewport->to_px(location_circle_data.horizontal_precision());
  constexpr float dot_radius_minimum = 10.;
  constexpr float cone_scale_factor = 5.;
  float dot_radius = qMax(accuracy_radius, dot_radius_minimum);
  float radius = cone_scale_factor * dot_radius_minimum;
  float margin = 10;
  float size = radius + margin;
  float angle = location_circle_data.bearing() + 90.; // North point to y
  vertices[0].set(QcVectorDouble(x - size, y - size), QcVectorDouble(-size, -size), radius, dot_radius, accuracy_radius, angle);
  vertices[1].set(QcVectorDouble(x - size, y + size), QcVectorDouble(-size,  size), radius, dot_radius, accuracy_radius, angle);
  vertices[2].set(QcVectorDouble(x + size, y - size), QcVectorDouble( size, -size), radius, dot_radius, accuracy_radius, angle);
  vertices[3].set(QcVectorDouble(x + size, y + size), QcVectorDouble( size,  size), radius, dot_radius, accuracy_radius, angle);
}

/**************************************************************************************************/

// QC_END_NAMESPACE


/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
