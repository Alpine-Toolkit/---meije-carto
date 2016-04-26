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

#include <cmath>

#include <QMatrix4x4>
#include <QSGClipNode>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGSimpleMaterial>
#include <QSGSimpleTextureNode>
#include <QSGTransformNode>
#include <QtDebug>

/**************************************************************************************************/

#include "map_scene.h"
#include "location_circle_material_shader.h"

#include "map/geo_coordinate.h"
#include "map/tile_spec.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapContainerNode : public QSGTransformNode
{
public:
  void add_child(const QcTileSpec & tile_spec, QSGSimpleTextureNode * node);

  QHash<QcTileSpec, QSGSimpleTextureNode *> texture_nodes;
};

void
QcMapContainerNode::add_child(const QcTileSpec & tile_spec, QSGSimpleTextureNode * texture_node)
{
  texture_nodes.insert(tile_spec, texture_node);
  appendChildNode(texture_node);
}

/**************************************************************************************************/

class QcMapRootNode : public QSGClipNode
{
public:
  QcMapRootNode();
  ~QcMapRootNode();

  void set_clip_rect(const QRect & rect);
  void update_tiles(QcMapContainerNode * root, QcMapScene * map_scene);

public:
  QSGGeometry geometry;
  QRect m_clip_rect;
  QSGTransformNode * root;
  QcMapContainerNode * map_container_node;
  QHash<QcTileSpec, QSGTexture *> textures;
};

QcMapRootNode::QcMapRootNode()
  : geometry(QSGGeometry::defaultAttributes_Point2D(), 4),
    root(new QSGTransformNode()),
    map_container_node(new QcMapContainerNode())
{
  qInfo();
  setIsRectangular(true);
  setGeometry(&geometry);
  root->appendChildNode(map_container_node);
  appendChildNode(root);

#ifndef ANDROID
  QSGGeometryNode * location_circle_node = new QSGGeometryNode();

  QSGGeometry * location_circle_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 1);
  location_circle_geometry->setLineWidth(100); // point size
  location_circle_geometry->setDrawingMode(GL_POINTS);
  location_circle_node->setGeometry(location_circle_geometry);
  location_circle_node->setFlag(QSGNode::OwnsGeometry);

  // Fixme: black instead of white
  QSGSimpleMaterial<QcLocationCircleMaterialShaderState> * location_circle_material = QcLocationCircleMaterialShader::createMaterial();
  location_circle_material->state()->r = 0;
  location_circle_material->state()->g = 0;
  location_circle_material->state()->b = 1;
  location_circle_material->state()->a = 0;
  location_circle_node->setMaterial(location_circle_material);
  location_circle_node->setFlag(QSGNode::OwnsMaterial);

  QSGGeometry::Point2D * location_circle_vertices = location_circle_geometry->vertexDataAsPoint2D();
  location_circle_vertices[0].set(0, 0);

  root->appendChildNode(location_circle_node);
#endif
}

QcMapRootNode::~QcMapRootNode()
{
  qDeleteAll(textures);
}

void
QcMapRootNode::set_clip_rect(const QRect & rect)
{
  qInfo() << rect;
  if (rect != m_clip_rect) {
    QSGGeometry::updateRectGeometry(&geometry, rect);
    QSGClipNode::setClipRect(rect);
    m_clip_rect = rect;
    markDirty(DirtyGeometry);
  }
}

void
QcMapRootNode::update_tiles(QcMapContainerNode * map_container_node, QcMapScene * map_scene)
{
  QcTileSpecSet tiles_in_scene = QcTileSpecSet::fromList(map_container_node->texture_nodes.keys()); // Fixme: cf. textured_tiles
  QcTileSpecSet visible_tiles = map_scene->visible_tiles();
  // QcTileSpecSet to_remove = tiles_in_scene - visible_tiles;
  // QcTileSpecSet to_add = visible_tiles - tiles_in_scene;

  QcTileSpecSet to_remove = tiles_in_scene;
  QcTileSpecSet to_add = visible_tiles;

  qInfo() << "tiles_in_scene" << tiles_in_scene
          << "\nvisible_tiles" << visible_tiles
          << "\nto_remove" << to_remove
          << "\nto_add" << to_add;

  for (const auto & tile_spec : to_remove)
    delete map_container_node->texture_nodes.take(tile_spec);

  // Update tile geometries
  // for (auto * texture_node : map_container_node->texture_nodes) {
  for (QHash<QcTileSpec, QSGSimpleTextureNode *>::iterator it = map_container_node->texture_nodes.begin();
       it != map_container_node->texture_nodes.end(); ) {
    const QcTileSpec & tile_spec = it.key();
    QSGSimpleTextureNode * texture_node = it.value();
    qInfo() << "texture nodes loop" << tile_spec;

    // Compute new geometry
    QSGGeometry visual_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
    QSGGeometry::TexturedPoint2D * vertexes = visual_geometry.vertexDataAsTexturedPoint2D();
    bool ok = map_scene->build_geometry(tile_spec, vertexes); // && qgeotiledmapscene_isTileInViewport(v, map_container_node->matrix())

    QSGNode::DirtyState dirty_bits = 0;
    // Check and handle changes to vertex data.
    if (ok && memcmp(texture_node->geometry()->vertexData(), vertexes, 4 * sizeof(QSGGeometry::TexturedPoint2D)) != 0) {
      if (vertexes[0].x == vertexes[3].x ||
          vertexes[0].y == vertexes[3].y) { // top-left == bottom-right => invalid => remove
        ok = false;
      } else {
        // void *memcpy(void *dest, const void *src, size_t n);
        qInfo() << "update geometry" << tile_spec;
        memcpy(texture_node->geometry()->vertexData(), vertexes, 4 * sizeof(QSGGeometry::TexturedPoint2D));
        dirty_bits |= QSGNode::DirtyGeometry;
      }
    }

    if (ok) {
      // if (isTextureLinear != map_scene->m_linearScaling) {
      //   texture_node->setFiltering(map_scene->m_linearScaling ? QSGTexture::Linear : QSGTexture::Nearest);
      //   dirty_bits |= QSGNode::DirtyMaterial;
      // }
      if (dirty_bits != 0)
        texture_node->markDirty(dirty_bits);
      it++;
    } else {
      it = map_container_node->texture_nodes.erase(it);
      delete texture_node;
    }
  }

  for (const auto & tile_spec : to_add) {
    // Fixme: code !!!
    QcTileTexture * tile_texture = map_scene->m_tile_textures.value(tile_spec).data(); // Fixme: m_tile_textures public
    qInfo() << "texture to add" << tile_spec << tile_texture;
    if (tile_texture && !tile_texture->image.isNull()) {
      qInfo() << "create texture" << tile_spec;
      QSGSimpleTextureNode * tile_node = new QSGSimpleTextureNode();
      // note: setTexture will update coordinates so do it here, before we buildGeometry
      tile_node->setTexture(textures.value(tile_spec));
      // Q_ASSERT(tileNode->geometry());
      // Q_ASSERT(tileNode->geometry()->attributes() == QSGGeometry::defaultAttributes_TexturedPoint2D().attributes);
      // Q_ASSERT(tileNode->geometry()->vertexCount() == 4);
      if (map_scene->build_geometry(tile_spec, tile_node->geometry()->vertexDataAsTexturedPoint2D())) {
        // && qgeotiledmapscene_isTileInViewport(tileNode->geometry()->vertexDataAsTexturedPoint2D(), map_container_node->matrix())
        // map_scene->m_linearScaling ? QSGTexture::Linear : QSGTexture::Nearest
        tile_node->setFiltering(QSGTexture::Linear);
        map_container_node->add_child(tile_spec, tile_node);
      } else
        delete tile_node;
    }
  }
}

/**************************************************************************************************/

QcMapScene::QcMapScene(QcViewport * viewport, QcTileMatrixSet & tile_matrix_set, QObject * parent)
  : QObject(parent),
    m_viewport(viewport),
    m_tile_matrix_set(tile_matrix_set)
{
  qInfo();
}

QcMapScene::~QcMapScene()
{
  qInfo();
}

void
QcMapScene::add_tile(const QcTileSpec & tile_spec, QSharedPointer<QcTileTexture> texture)
{
  if (m_visible_tiles.contains(tile_spec)) { // Don't add the geometry if it isn't visible
    m_tile_textures.insert(tile_spec, texture);
    qInfo() << "add_tile" << tile_spec << "inserted";
  }
  else
    qInfo() << "add_tile" << tile_spec << "already there";
}

void
QcMapScene::set_visible_tiles(const QcTileSpecSet & tile_specs)
{
  QcTileSpecSet to_remove = m_visible_tiles - tile_specs;
  if (!to_remove.isEmpty())
    remove_tiles(to_remove);
  m_visible_tiles = tile_specs;
}

void
QcMapScene::remove_tiles(const QcTileSpecSet & old_tiles)
{
  qInfo() << old_tiles;
  for (auto tile_spec : old_tiles)
    m_tile_textures.remove(tile_spec);
}

QcTileSpecSet
QcMapScene::textured_tiles() const
{
  return QcTileSpecSet::fromList(m_tile_textures.keys());
}

bool
QcMapScene::build_geometry(const QcTileSpec & tile_spec, QSGGeometry::TexturedPoint2D * vertices)
{
  // if (x < m_tileXWrapsBelow)
  //     x += m_sideLength;

  // if ((x < m_minTileX)
  //         || (m_maxTileX < x)
  //         || (spec.y() < m_minTileY)
  //         || (m_maxTileY < spec.y())
  //         || (spec.zoom() != m_intZoomLevel)) {
  //     return false;
  // }

  // double edge = m_scaleFactor * m_tileSize;

  // double x1 = (x - m_minTileX);
  // double x2 = x1 + 1.0;

  // double y1 = (m_minTileY - spec.y());
  // double y2 = y1 - 1.0;

  // x1 *= edge;
  // x2 *= edge;
  // y1 *= edge;
  // y2 *= edge;

  int tile_size = m_tile_matrix_set.tile_size();
  const QcTileMatrix & tile_matrix = m_tile_matrix_set[m_viewport->zoom_level()];
  //double tile_length_m = tile_matrix.tile_length_m();
  double resolution = tile_matrix.resolution(); // [m/px]

  const QcPolygon & polygon = m_viewport->polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  double x_inf_m = interval.x().inf();
  double y_inf_m = interval.y().inf();
  // double y_sup_m = interval.y().sup();
  double x_inf_px = x_inf_m / resolution;
  double y_inf_px = y_inf_m / resolution;
  //double y_sup_px = y_sup_m / resolution;

  double x = tile_spec.x() * tile_size;
  double y = tile_spec.y() * tile_size;

  double x1 = (x - x_inf_px) * 1;
  double y1 = (y - y_inf_px) * 1;
  double x2 = x1 + 1.0 * tile_size * 1;
  double y2 = y1 + 1.0 * tile_size * 1;

  // Texture coordinate order for veritcal flip of texture
  vertices[0].set(x1, y1, 0, 0);
  vertices[1].set(x1, y2, 0, 1);
  vertices[2].set(x2, y1, 1, 0);
  vertices[3].set(x2, y2, 1, 1);

  qInfo() << "geometry" << tile_spec << "x" << x1 << x2 << "  y" << y1 << y2;

  return true;
}

QSGNode *
QcMapScene::update_scene_graph(QSGNode * old_node, QQuickWindow * window)
{
  qInfo() << old_node;

  qInfo() << "item size" << m_item_size;
  float width = m_item_size.width();
  float height = m_item_size.height();
  if (width <= 0 || height <= 0) {
    delete old_node;
    return nullptr;
  }

  QcMapRootNode * map_root_node = static_cast<QcMapRootNode *>(old_node);
  if (!map_root_node) {
    qInfo() << "map_root_node is null";
    map_root_node = new QcMapRootNode();
  }

  QRect rect = QRect(0, 0, width, height);
  map_root_node->set_clip_rect(rect);

  QMatrix4x4 item_space_matrix;
  item_space_matrix.scale(width/2, height/2);
  item_space_matrix.translate(1, 1);
  item_space_matrix.scale(1, 1); // y is downward
  map_root_node->root->setMatrix(item_space_matrix);
  qInfo() << "item_space_matrix" << item_space_matrix;

  int tile_size = m_tile_matrix_set.tile_size();
  const QcTileMatrix & tile_matrix = m_tile_matrix_set[m_viewport->zoom_level()];
  double tile_length_m = tile_matrix.tile_length_m();
  double resolution = tile_matrix.resolution(); // [m/px]
  qInfo() << "tile_matrix" << m_viewport->zoom_level() << tile_length_m << resolution;

  const QcPolygon & polygon = m_viewport->polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  // qInfo() << "Normalised Mercator polygon interval [m]"
  //         << "[" << (int) interval.x().inf() << ", " << (int) interval.x().sup() << "]"
  //         << "x"
  //         << "[" << (int) interval.y().inf() << ", " << (int) interval.y().sup() << "]";
  // QcTiledPolygon tiled_polygon = polygon.intersec_with_grid(tile_length_m);
  // for (const QcTiledPolygonRun & run:  tiled_polygon.runs()) {
  //   const QcIntervalInt & run_interval = run.interval();
  //   qInfo() << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]";
  // }

  QcGeoCoordinateWebMercator mercator_coordinate = m_viewport->web_mercator();
  QcGeoCoordinateNormalisedWebMercator normalised_mercator_coordinate = m_viewport->normalised_web_mercator();
  QcVectorDouble center = normalised_mercator_coordinate.vector() * EQUATORIAL_PERIMETER;
  // qInfo() << "center mercator" << mercator_coordinate << normalised_mercator_coordinate << center;
  double x_inf_m = interval.x().inf();
  double y_inf_m = interval.y().inf();
  //double x_center_px = (center.x() - x_inf_m) / resolution; // vector
  //double y_center_px = (center.y() - y_inf_m) / resolution;
  // qInfo() << "xy center px" << x_center_px << y_center_px; // == width/2 height/2
  // qInfo() << (int)center.x() << (int)x_inf_m << resolution;

  // Use QcInterval2D ???
  int x_grid_inf = ceil(interval.x().inf() / tile_length_m); // duplicate
  int x_grid_sup = ceil(interval.x().sup() / tile_length_m);
  int y_grid_inf = ceil(interval.y().inf() / tile_length_m);
  int y_grid_sup = ceil(interval.y().sup() / tile_length_m);
  size_t number_of_vertical_lines = x_grid_sup - x_grid_inf +1;
  size_t number_of_horizontal_lines = y_grid_sup - y_grid_inf +1;
  // qInfo() << "x grid" << x_grid_inf << x_grid_sup << number_of_vertical_lines;
  // qInfo() << "y grid" << y_grid_inf << y_grid_sup << number_of_horizontal_lines;

  QcMapContainerNode * container_node = map_root_node->map_container_node;
  // QSGNode * node = container_node->firstChild();
  // container_node->removeAllChildNodes();
  // if (node) {
  //   qInfo() << "Delete node";
  //   // delete node;
  // }

  QMatrix4x4 container_space_matrix;
  container_space_matrix.scale(2/width, 2/height);
  container_space_matrix.translate(-width/2, -height/2);
  container_node->setMatrix(container_space_matrix);
  qInfo() << "container_space_matrix" << container_space_matrix;

  QSGGeometryNode * grid_node = new QSGGeometryNode();

  const int vertex_count = 2*(number_of_horizontal_lines + number_of_vertical_lines) + 100;
  QSGGeometry * grid_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertex_count);
  grid_geometry->setLineWidth(2);
  grid_geometry->setDrawingMode(GL_LINES);
  grid_node->setGeometry(grid_geometry);
  grid_node->setFlag(QSGNode::OwnsGeometry);

  QSGFlatColorMaterial * grid_material = new QSGFlatColorMaterial;
  grid_material->setColor(QColor(255, 0, 0));
  grid_node->setMaterial(grid_material);
  grid_node->setFlag(QSGNode::OwnsMaterial);

  QSGGeometry::Point2D * vertices = grid_geometry->vertexDataAsPoint2D();
  size_t vertex_index = 0;
  double x_inf_px = x_inf_m / resolution;
  double y_inf_px = y_inf_m / resolution;
  // qInfo() << "inf px" << x_inf_px << y_inf_px;
  double x_offset = x_grid_inf * tile_size - x_inf_px;
  double y_offset = y_grid_inf * tile_size - y_inf_px;
  // qInfo() << "offset px" << x_offset << y_offset;
  for (size_t i = 0; i <= number_of_vertical_lines; i++) {
    float x = i * tile_size + x_offset;
    // qInfo() << i << x;
    vertices[vertex_index].set(x, 0);
    vertices[vertex_index +1].set(x, height);
    vertex_index += 2;
  }
  for (size_t i = 0; i <= number_of_horizontal_lines; i++) {
    float y = i * tile_size + y_offset;
    // qInfo() << i << y;
    vertices[vertex_index].set(0, y);
    vertices[vertex_index +1].set(width, y);
    vertex_index += 2;
  }
  // parasite lines at left and bottom ???

  container_node->removeAllChildNodes(); // delete
  container_node->appendChildNode(grid_node);
  // grid_node->markDirty(QSGNode::DirtyGeometry);

  // Fixme: duplicated code?
  QcTileSpecSet textures_in_scene = QcTileSpecSet::fromList(map_root_node->textures.keys()); // cf. textured_tiles
  QcTileSpecSet to_remove = textures_in_scene - m_visible_tiles;
  QcTileSpecSet to_add = m_visible_tiles - textures_in_scene;
  qInfo() << "textures in scene" << textures_in_scene
          << "to remove:" << to_remove
          << "to add" << to_add;
  for (const auto & tile_spec : to_remove)
    map_root_node->textures.take(tile_spec)->deleteLater();
  for (const auto & tile_spec : to_add) {
    QcTileTexture * tile_texture = m_tile_textures.value(tile_spec).data();
    if (tile_texture && !tile_texture->image.isNull()) {
      qInfo() << "create texture from image" << tile_spec;
      QSGTexture * texture = window->createTextureFromImage(tile_texture->image);
      map_root_node->textures.insert(tile_spec, texture);
    }
  }

  map_root_node->update_tiles(map_root_node->map_container_node, this);

  return map_root_node;
}

/**************************************************************************************************/

// #include "map_scene.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
