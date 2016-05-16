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

#include "path_node.h"
#include "path_material_shader.h"

#include <QSGFlatColorMaterial>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

struct PathPoint2D {
  float x;
  float y;
  float u;
  float v;
  float line_length;
  float line_width;
  float cap;
  // float r;
  // float g;
  // float b;
  // float a;

  void set(const QcVectorDouble & point,
           const QcVectorDouble & uv,
           float _line_length,
           float _line_width,
           float _cap
           ) {
    x = point.x();
    y = point.y();
    u = uv.x();
    v = uv.y();
    line_length = _line_length;
    line_width = _line_width;
    cap = _cap;
    // r;
    // g;
    // b;
    // a;
  }
};

QSGGeometry::Attribute PathPoint2D_Attributes[] = {
  QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),  // xy
  QSGGeometry::Attribute::create(1, 2, GL_FLOAT, false), // uv
  QSGGeometry::Attribute::create(2, 1, GL_FLOAT, false), // line_length
  QSGGeometry::Attribute::create(3, 1, GL_FLOAT, false), // line_width
  QSGGeometry::Attribute::create(4, 1, GL_FLOAT, false), // cap
  // QSGGeometry::Attribute::create(5, 4, GL_FLOAT, false)  // colour
};

QSGGeometry::AttributeSet PathPoint2D_AttributeSet = {
    2, // count
    sizeof(PathPoint2D), // stride
    PathPoint2D_Attributes
};

/**************************************************************************************************/

QcVectorDouble
compute_offsets(const QcVectorDouble & dir1, const QcVectorDouble & dir2, double half_width,
                double & u_offset)
{
  /* WRONG
  double angle = dir1.atan_with(dir2) * .5;
  // double angle = dir1.radians_with(dir2) * .5;
  QcVectorDouble offset = (dir1 + dir2).rotate_counter_clockwise_90();
  offset.normalise();
  offset *= half_width / cos(angle);
  u_offset = half_width * tan(angle);
  */

  QcVectorDouble normal1 = dir1.rotate_counter_clockwise_90();
  QcVectorDouble tangent = dir1 + dir2;
  tangent.normalise();
  QcVectorDouble mitter = tangent.rotate_counter_clockwise_90();
  double cos = mitter.dot(normal1);
  double sin = mitter.cross(normal1);
  double tan = sin / cos;
  double mitter_length = abs(half_width / cos);
  u_offset = - half_width * tan; // Fixme: - ?

  return mitter * mitter_length;
}

/**************************************************************************************************/

QcPathNode::QcPathNode(const QcViewport * viewport)
  : QSGOpacityNode(),
    m_viewport(viewport)
{
  setOpacity(1.); // 1. black

  // Fixme: float ?
  QList<QcVectorDouble> path_vertexes;
  // ok
  // path_vertexes << QcVectorDouble(100, 300);
  // path_vertexes << QcVectorDouble(400, 300);
  // path_vertexes << QcVectorDouble(400, 600);

  // ok
  // path_vertexes << QcVectorDouble(100, 300);
  // path_vertexes << QcVectorDouble(400, 300);
  // path_vertexes << QcVectorDouble(100, 600);

  // ok
  // path_vertexes << QcVectorDouble(100, 300);
  // path_vertexes << QcVectorDouble(400, 300);
  // path_vertexes << QcVectorDouble(600, 600);

  // ok
  // path_vertexes << QcVectorDouble(100, 100);
  // path_vertexes << QcVectorDouble(400, 400);
  // path_vertexes << QcVectorDouble(700, 100);
  // path_vertexes << QcVectorDouble(1000, 400);

  path_vertexes << QcVectorDouble(100, 100);
  path_vertexes << QcVectorDouble(400, 450);
  path_vertexes << QcVectorDouble(625, 100);
  path_vertexes << QcVectorDouble(800, 610);

  double line_width = 10;
  double antialias_diameter = 1.;
  // Fixme: linewidth/2.0 + 1.5*antialias;
  double half_width = ceil(1.25*antialias_diameter + line_width) * .5;

  int number_of_path_vertexes = path_vertexes.size();
  int number_of_segments = number_of_path_vertexes -1;
  int number_of_vertexes = number_of_segments * 4;

  QSGGeometryNode * geometry_node = new QSGGeometryNode();
  // QSGGeometry::defaultAttributes_Point2D()
  QSGGeometry * geometry = new QSGGeometry(PathPoint2D_AttributeSet, number_of_vertexes);
  geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  geometry_node->setGeometry(geometry);
  geometry_node->setFlag(QSGNode::OwnsGeometry);

  // QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();
  PathPoint2D * vertices = static_cast<PathPoint2D *>(geometry->vertexData());
  int last_i = number_of_path_vertexes -2;
  for (int i = 0; i <= last_i; i++) {
    QcVectorDouble point1 = path_vertexes[i];
    QcVectorDouble point2 = path_vertexes[i+1];
    // Fixme: optimise
    QcVectorDouble point0 = i > 0      ? path_vertexes[i-1] : point1;
    QcVectorDouble point3 = i < last_i ? path_vertexes[i+2] : point2;
    qInfo() << i << point0 << point1 << point2 << point3;

    QcVectorDouble dir1 = point2 - point1;
    double segment_length = dir1.magnitude();
    dir1.normalise();
    QcVectorDouble normal1 = dir1.rotate_counter_clockwise_90();
    QcVectorDouble tangential_offset1 = dir1 * half_width;
    QcVectorDouble normal_offset1 = normal1 * half_width;

    QcVectorDouble vertex;
    int j = 4*i;

    double cap1 = 0;
    if (point0 == point1) {
      cap1 = -1;
      point1 -= tangential_offset1;

      vertex = point1 - normal_offset1;
      // qInfo() << "1   0 == 1" << vertex;
      // vertices[j].set(vertex.x(), vertex.y());
      vertices[j].set(vertex, QcVectorDouble(-half_width, -half_width), segment_length, line_width, cap1);
      vertex = point1 + normal_offset1;
      // qInfo() << "2   0 == 1" << vertex;
      vertices[j+1].set(vertex, QcVectorDouble(-half_width, half_width), segment_length, line_width, cap1);
    } else {
      QcVectorDouble dir0 = point1 - point0;
      dir0.normalise();
      double u1;
      QcVectorDouble offset1 = compute_offsets(dir0, dir1, half_width, u1);
      // qInfo() << "offset1" << offset1;

      vertex = point1 - offset1;
      // qInfo() << "1" << vertex;
      vertices[j].set(vertex, QcVectorDouble(-u1, -half_width), segment_length, line_width, cap1);
      vertex = point1 + offset1;
      // qInfo() << "2" << vertex;
      vertices[j+1].set(vertex, QcVectorDouble(u1, half_width), segment_length, line_width, cap1);
    }

    double cap2 = 0;
    if (point2 == point3) {
      cap2 = 1;
      point2 += tangential_offset1;
      double u = segment_length + half_width;

      vertex = point2 - normal_offset1;
      // qInfo() << "3   2 == 3" << vertex;
      vertices[j+2].set(vertex, QcVectorDouble(u, -half_width), segment_length, line_width, cap2);
      vertex = point2 + normal_offset1;
      // qInfo() << "4   2 == 3"  << vertex;
      vertices[j+3].set(vertex, QcVectorDouble(u, half_width), segment_length, line_width, cap2);
    } else {
      QcVectorDouble dir2 = point3 - point2;
      dir2.normalise();
      double u2;
      QcVectorDouble offset2 = compute_offsets(dir1, dir2, half_width, u2);
      // qInfo() << "offset2" << offset2;

      vertex = point2 - offset2;
      // qInfo() << "3" << vertex;
      vertices[j+2].set(vertex, QcVectorDouble(segment_length + u2, -half_width), segment_length, line_width, cap2);
      vertex = point2 + offset2;
      // qInfo() << "4" << vertex;
      vertices[j+3].set(vertex, QcVectorDouble(segment_length - u2,  half_width), segment_length, line_width, cap2);
    }
  }

  QSGSimpleMaterial<QcPathMaterialShaderState> * material = QcPathMaterialShader::createMaterial();
  material->state()->r = 0;
  material->state()->g = 0;
  material->state()->b = 1.;
  material->state()->a = 1.;
  // QSGFlatColorMaterial * material = new QSGFlatColorMaterial();
  // material->setColor(QColor("black"));
  material->setFlag(QSGMaterial::Blending);
  geometry_node->setMaterial(material);
  geometry_node->setFlag(QSGNode::OwnsMaterial);

  appendChildNode(geometry_node);
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
