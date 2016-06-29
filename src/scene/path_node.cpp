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
#include "point_material_shader.h"

#include <QSGFlatColorMaterial>
#include <QtDebug>

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
  float r;
  float g;
  float b;
  float a;

  void set(const QcVectorDouble & point,
           const QcVectorDouble & uv,
           float _line_length,
           float _line_width,
           float _cap,
           const QColor & colour
           ) {
    x = point.x();
    y = point.y();
    u = uv.x();
    v = uv.y();
    line_length = _line_length;
    line_width = _line_width;
    cap = _cap;
    r = colour.redF();
    g = colour.greenF();
    b = colour.blueF();
    a = colour.alphaF();
  }
};

QSGGeometry::Attribute PathPoint2D_Attributes[] = {
  QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),  // xy
  QSGGeometry::Attribute::create(1, 2, GL_FLOAT, false), // uv
  QSGGeometry::Attribute::create(2, 1, GL_FLOAT, false), // line_length
  QSGGeometry::Attribute::create(3, 1, GL_FLOAT, false), // line_width
  QSGGeometry::Attribute::create(4, 1, GL_FLOAT, false), // cap
  QSGGeometry::Attribute::create(5, 4, GL_FLOAT, false)  // colour
};

QSGGeometry::AttributeSet PathPoint2D_AttributeSet = {
    6, // count Fixme: ???
    sizeof(PathPoint2D), // stride
    PathPoint2D_Attributes
};

/**************************************************************************************************/

struct CirclePoint2D {
  float x;
  float y;
  float u;
  float v;
  float radius;
  float r;
  float g;
  float b;
  float a;

  void set(const QcVectorDouble & point,
           const QcVectorDouble & uv,
           float _radius,
           const QColor & colour
           ) {
    x = point.x();
    y = point.y();
    u = uv.x();
    v = uv.y();
    radius = _radius;
    r = colour.red();
    g = colour.green();
    b = colour.blue();
    a = colour.alpha();
  }
};

QSGGeometry::Attribute CirclePoint2D_Attributes[] = {
  QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),  // xy
  QSGGeometry::Attribute::create(1, 2, GL_FLOAT, false), // uv
  QSGGeometry::Attribute::create(2, 1, GL_FLOAT, false), // radius
  QSGGeometry::Attribute::create(3, 4, GL_FLOAT, false)  // colour
};

QSGGeometry::AttributeSet CirclePoint2D_AttributeSet = {
  4, // count Fixme: ???
  sizeof(CirclePoint2D), // stride
  CirclePoint2D_Attributes
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
  double mitter_length = fabs(half_width / cos);
  u_offset = - half_width * tan; // Fixme: - ?

  return mitter * mitter_length;
}

/**************************************************************************************************/

QcPathNode::QcPathNode(const QcViewport * viewport)
  : QSGOpacityNode(),
    m_viewport(viewport),
    m_path_geometry_node(new QSGGeometryNode()),
    m_polygon_geometry_node(new QSGGeometryNode()),
    m_point_geometry_node(new QSGGeometryNode())
{
  setOpacity(1.); // 1. black

  // Polygon
  QSGGeometry * polygon_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
  polygon_geometry->setDrawingMode(GL_TRIANGLES);
  m_polygon_geometry_node->setGeometry(polygon_geometry);
  m_polygon_geometry_node->setFlag(QSGNode::OwnsGeometry);

  QSGFlatColorMaterial * polygon_material = new QSGFlatColorMaterial;
  polygon_material->setColor(QColor(0, 0, 255, 50));
  polygon_material->setFlag(QSGMaterial::Blending);
  m_polygon_geometry_node->setMaterial(polygon_material);
  m_polygon_geometry_node->setFlag(QSGNode::OwnsMaterial);

  appendChildNode(m_polygon_geometry_node);

  // Path
  QSGGeometry * path_geometry = new QSGGeometry(PathPoint2D_AttributeSet, 0); // Fixme:
  path_geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  m_path_geometry_node->setGeometry(path_geometry);
  m_path_geometry_node->setFlag(QSGNode::OwnsGeometry);

  QSGSimpleMaterial<QcPathMaterialShaderState> * path_material = QcPathMaterialShader::createMaterial();
  // QcPathMaterialShaderState * path_state = path_material->state();
  // path_state->r = 0;
  // path_state->g = 0;
  // path_state->b = 1.;
  // path_state->a = 1.;
  // state->cap_type = 1;
  // state->line_join = 1;
  // state->antialias_diameter = 1.;
  path_material->setFlag(QSGMaterial::Blending);
  m_path_geometry_node->setMaterial(path_material);
  m_path_geometry_node->setFlag(QSGNode::OwnsMaterial);

  appendChildNode(m_path_geometry_node);

  // Point
  QSGGeometry * point_geometry = new QSGGeometry(CirclePoint2D_AttributeSet, 0); // Fixme:
  point_geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  m_point_geometry_node->setGeometry(point_geometry);
  m_point_geometry_node->setFlag(QSGNode::OwnsGeometry);

  QSGSimpleMaterial<QcPointMaterialShaderState> * point_material = QcPointMaterialShader::createMaterial();
  // point_material->state()->r = 0; // Fixme: QColor
  // point_material->state()->g = 0;
  // point_material->state()->b = 1;
  // point_material->state()->a = 1.;
  // QSGFlatColorMaterial * material = new QSGFlatColorMaterial();
  // material->setColor(QColor("black"));
  point_material->setFlag(QSGMaterial::Blending);
  m_point_geometry_node->setMaterial(point_material);
  m_point_geometry_node->setFlag(QSGNode::OwnsMaterial);

  appendChildNode(m_point_geometry_node);
}

void
QcPathNode::set_path_points(PathPoint2D * path_points,
                            int i,
                            QcVectorDouble & point0,
                            QcVectorDouble & point1,
                            QcVectorDouble & point2,
                            QcVectorDouble & point3,
                            double line_width,
                            double half_width,
                            const QColor & colour)
{
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
    // path_points[j].set(vertex.x(), vertex.y());
    path_points[j].set(vertex, QcVectorDouble(-half_width, -half_width), segment_length, line_width, cap1, colour);
    vertex = point1 + normal_offset1;
    // qInfo() << "2   0 == 1" << vertex;
    path_points[j+1].set(vertex, QcVectorDouble(-half_width, half_width), segment_length, line_width, cap1, colour);
  } else {
    QcVectorDouble dir0 = point1 - point0;
    dir0.normalise();
    double u1;
    QcVectorDouble offset1 = compute_offsets(dir0, dir1, half_width, u1);
    // qInfo() << "offset1" << offset1;

    vertex = point1 - offset1;
    // qInfo() << "1" << vertex;
    path_points[j].set(vertex, QcVectorDouble(-u1, -half_width), segment_length, line_width, cap1, colour);
    vertex = point1 + offset1;
    // qInfo() << "2" << vertex;
    path_points[j+1].set(vertex, QcVectorDouble(u1, half_width), segment_length, line_width, cap1, colour);
  }

  double cap2 = 0;
  if (point2 == point3) {
    cap2 = 1;
    point2 += tangential_offset1;
    double u = segment_length + half_width;

    vertex = point2 - normal_offset1;
    // qInfo() << "3   2 == 3" << vertex;
    path_points[j+2].set(vertex, QcVectorDouble(u, -half_width), segment_length, line_width, cap2, colour);
    vertex = point2 + normal_offset1;
    // qInfo() << "4   2 == 3"  << vertex;
    path_points[j+3].set(vertex, QcVectorDouble(u, half_width), segment_length, line_width, cap2, colour);
  } else {
    QcVectorDouble dir2 = point3 - point2;
    dir2.normalise();
    double u2;
    QcVectorDouble offset2 = compute_offsets(dir1, dir2, half_width, u2);
    // qInfo() << "offset2" << offset2;

    vertex = point2 - offset2;
    // qInfo() << "3" << vertex;
    path_points[j+2].set(vertex, QcVectorDouble(segment_length + u2, -half_width), segment_length, line_width, cap2, colour);
    vertex = point2 + offset2;
    // qInfo() << "4" << vertex;
    path_points[j+3].set(vertex, QcVectorDouble(segment_length - u2,  half_width), segment_length, line_width, cap2, colour);
  }
}

void
QcPathNode::update(const QcDecoratedPathDouble * path)
{
  QList<QcVectorDouble> path_vertexes;
  // Fixme:
  if (path)
    for (const auto & vertex : path->vertexes()) {
      QcVectorDouble screen_position = m_viewport->coordinate_to_screen(vertex);
      // qInfo() << vertex << screen_position;
      path_vertexes << screen_position;
    }
  int number_of_path_vertexes = path_vertexes.size();
  int number_of_vertexes = 0;

  bool path_closed = number_of_path_vertexes >= 3 and path->closed();

  const QColor path_colour(0, 0, 255, 255);
  const QColor selected_colour(255, 0, 0, 255);

  // Polygon
  /*
  QSGGeometry * polygon_geometry;
  if (path_closed) {
    // QcPolygonTriangulation triangulation = path.triangulate();
    QcPathDouble screen_path(path_vertexes, path_closed);
    QcPolygonTriangulation triangulation(screen_path);
    QList<QcTriangleVertex> triangles = triangulation.triangle_vertexes();
    number_of_vertexes = triangles.size() * 3;

    polygon_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), number_of_vertexes);
    QSGGeometry::Point2D * path_points = static_cast<QSGGeometry::Point2D *>(polygon_geometry->vertexData());
    int i = 0;
    for (const auto & triangle : triangles) {
      int j = i*3;
      const QcVectorDouble & p1 = triangle.p1;
      path_points[j].set(p1.x(), p1.y());
      const QcVectorDouble & p2 = triangle.p2;
      path_points[j+1].set(p2.x(), p2.y());
      const QcVectorDouble & p3 = triangle.p3;
      path_points[j+2].set(p3.x(), p3.y());
      qInfo() << i << p1 << p2 << p3;
      i++;
    }
  } else
    // Fixme: better
    polygon_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
  polygon_geometry->setDrawingMode(GL_TRIANGLES);
  m_polygon_geometry_node->setGeometry(polygon_geometry);
  m_polygon_geometry_node->setFlag(QSGNode::OwnsGeometry);
  */

  // Path
  int number_of_segments = (number_of_path_vertexes) ? number_of_path_vertexes -1 : 0;
  if (path_closed)
    number_of_segments += 1;
  number_of_vertexes = number_of_segments * 4;
  QSGGeometry * path_geometry = new QSGGeometry(PathPoint2D_AttributeSet, number_of_vertexes);
  path_geometry->setDrawingMode(GL_TRIANGLE_STRIP);
  m_path_geometry_node->setGeometry(path_geometry);
  m_path_geometry_node->setFlag(QSGNode::OwnsGeometry);
  if (number_of_segments) {
    constexpr double line_width = 10;
    constexpr double antialias_diameter = 1.; // Fixme: according shader !
    // Fixme: linewidth/2.0 + 1.5*antialias;
    double half_width = ceil(1.25*antialias_diameter + line_width) * .5;

    PathPoint2D * path_points = static_cast<PathPoint2D *>(path_geometry->vertexData());
    int last_i = number_of_path_vertexes -2;
    for (int i = 0; i <= last_i; i++) {
      QcVectorDouble point1 = path_vertexes[i];
      QcVectorDouble point2 = path_vertexes[i+1];
      // Fixme: optimise
      QcVectorDouble point0 = i > 0      ? path_vertexes[i-1] : point1;
      QcVectorDouble point3 = i < last_i ? path_vertexes[i+2] : point2;
      // qInfo() << i << point0 << point1 << point2 << point3;
      set_path_points(path_points, i, point0, point1, point2, point3, line_width, half_width, path_colour);
    }
    if (path_closed) {
      QcVectorDouble point0 = path_vertexes[last_i];
      QcVectorDouble point1 = path_vertexes[last_i +1];
      QcVectorDouble point2 = path_vertexes[0];
      QcVectorDouble point3 = path_vertexes[1];
      set_path_points(path_points, number_of_segments -1, point0, point1, point2, point3, line_width, half_width, path_colour);
    }
  }

  // Points
  number_of_vertexes = number_of_path_vertexes * 6;
  QSGGeometry * point_geometry = new QSGGeometry(CirclePoint2D_AttributeSet, number_of_vertexes);
  point_geometry->setDrawingMode(GL_TRIANGLES);
  m_point_geometry_node->setGeometry(point_geometry);
  m_point_geometry_node->setFlag(QSGNode::OwnsGeometry);
  if (number_of_path_vertexes) {
    CirclePoint2D * circle_points = static_cast<CirclePoint2D *>(point_geometry->vertexData());
    constexpr float point_radius = 10; // Fixme: setting
    constexpr float margin = 10;
    int path_vertex_index = 0;
    int vertex_index = 0;
    for (const auto & vertex : path_vertexes) {
      QcDecoratedPathDouble::AttributeType attribute_type = path->attribute_at(path_vertex_index);
      QColor colour = test_bit(attribute_type, QcDecoratedPathDouble::AttributeType::Selected) ? selected_colour : path_colour;
      float radius = point_radius;
      if (test_bit(attribute_type, QcDecoratedPathDouble::AttributeType::Touched))
        // Fixme: set larger than a finger 2cm
        radius *= 4;
      float size = radius + margin;
      double x = vertex.x();
      double y = vertex.y();
      QcVectorDouble point1(x - size, y - size);
      QcVectorDouble point2(x - size, y + size);
      QcVectorDouble point3(x + size, y - size);
      QcVectorDouble point4(x + size, y + size);
      QcVectorDouble uv1(-size, -size);
      QcVectorDouble uv2(-size,  size);
      QcVectorDouble uv3( size, -size);
      QcVectorDouble uv4( size,  size);
      circle_points[vertex_index    ].set(point1, uv1, radius, colour);
      circle_points[vertex_index + 1].set(point2, uv2, radius, colour);
      circle_points[vertex_index + 2].set(point3, uv3, radius, colour);
      circle_points[vertex_index + 3].set(point2, uv2, radius, colour);
      circle_points[vertex_index + 4].set(point4, uv4, radius, colour);
      circle_points[vertex_index + 5].set(point3, uv3, radius, colour);
      path_vertex_index++;
      vertex_index += 6;
    }
  }
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
