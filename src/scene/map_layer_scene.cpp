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

#include "map_scene.h"
#include "map_scene_private.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

void
QcMapSideNode::add_child(const QcTileSpec & tile_spec, QSGSimpleTextureNode * texture_node)
{
  texture_nodes.insert(tile_spec, texture_node);
  appendChildNode(texture_node);
}

/**************************************************************************************************/

QcMapLayerRootNode::QcMapLayerRootNode(const QcTileMatrixSet & tile_matrix_set, const QcViewport * viewport)
  : QSGOpacityNode(),
    m_tile_matrix_set(tile_matrix_set),
    m_viewport(viewport),
    // grid_node(new QcGridNode(tile_matrix_set, viewport)),
    west_map_node(new QcMapSideNode()),
    central_map_node(new QcMapSideNode()),
    east_map_node(new QcMapSideNode())
{
  // qInfo();

  setOpacity(1.);

  // central_map_node->appendChildNode(grid_node); // Fixme:
  appendChildNode(west_map_node);
  appendChildNode(central_map_node);
  appendChildNode(east_map_node);
}

QcMapLayerRootNode::~QcMapLayerRootNode()
{
  qDeleteAll(textures);
}

void
QcMapLayerRootNode::update_central_maps()
{
  int number_of_full_maps = m_viewport->number_of_full_maps();
  int number_of_clones = qMax(number_of_full_maps - 1, 0);
  while (number_of_clones < central_map_nodes.size()) {
    qInfo() << "remove clone";
    auto * node = central_map_nodes.takeLast();
    removeChildNode(node);
  }
  if (number_of_clones) {
    while (central_map_nodes.size() < number_of_clones) {
    // for (int i = 0; i < (number_of_clones - central_map_nodes.size()); i++) {
      qInfo() << "add clone";
      auto * node = new QcMapSideNode();
      central_map_nodes << node;
      appendChildNode(node);
    }
  }
}

void
QcMapLayerRootNode::update_tiles(QcMapLayerScene * map_scene,
                                 QcMapSideNode * map_side_node,
                                 const QcTileSpecSet & visible_tiles,
                                 const QcPolygon & polygon,
                                 const QcViewportPart & part)
{
  // float width = map_scene->width();
  // float height = map_scene->height();

  QMatrix4x4 space_matrix;
  space_matrix.setToIdentity();
  const QcInterval2DDouble & screen_interval = part.screen_interval();
  double x_offset = screen_interval.x().inf();
  double y_offset = screen_interval.y().inf();
  space_matrix.translate(x_offset, y_offset);
  map_side_node->setMatrix(space_matrix);
  // qInfo() << "map side space matrix" << space_matrix;

  QcTileSpecSet tiles_in_scene = QcTileSpecSet::fromList(map_side_node->texture_nodes.keys()); // Fixme: cf. textured_tiles
  QcTileSpecSet to_remove = tiles_in_scene - visible_tiles;
  QcTileSpecSet to_add = visible_tiles - tiles_in_scene;

  // qInfo() << "Offset" << x_offset
  //         << "tiles_in_scene" << tiles_in_scene
  //         << "\nvisible_tiles" << visible_tiles
  //         << "\nto_remove" << to_remove
  //         << "\nto_add" << to_add;

  for (const auto & tile_spec : to_remove)
    delete map_side_node->texture_nodes.take(tile_spec);

  // Update tile geometries
  // for (auto * texture_node : map_side_node->texture_nodes) {
  for (QHash<QcTileSpec, QSGSimpleTextureNode *>::iterator it = map_side_node->texture_nodes.begin();
       it != map_side_node->texture_nodes.end(); ) {
    const QcTileSpec & tile_spec = it.key();
    QSGSimpleTextureNode * texture_node = it.value();
    // qInfo() << "texture nodes loop" << tile_spec;

    // Compute new geometry
    QSGGeometry visual_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
    QSGGeometry::TexturedPoint2D * vertexes = visual_geometry.vertexDataAsTexturedPoint2D();
    bool ok = map_scene->build_geometry(tile_spec, vertexes, polygon); // && qgeotiledmapscene_isTileInViewport(v, map_side_node->matrix())

    QSGNode::DirtyState dirty_bits = 0;
    // Check and handle changes to vertex data.
    if (ok && memcmp(texture_node->geometry()->vertexData(), vertexes, 4 * sizeof(QSGGeometry::TexturedPoint2D)) != 0) {
      if (vertexes[0].x == vertexes[3].x ||
          vertexes[0].y == vertexes[3].y) { // top-left == bottom-right => invalid => remove
        ok = false;
      } else {
        // void *memcpy(void *dest, const void *src, int n);
        // qInfo() << "update geometry" << tile_spec;
        memcpy(texture_node->geometry()->vertexData(), vertexes, 4 * sizeof(QSGGeometry::TexturedPoint2D));
        dirty_bits |= QSGNode::DirtyGeometry;
      }
    }

    if (ok) {
      if (dirty_bits != 0)
        texture_node->markDirty(dirty_bits);
      it++;
    } else {
      it = map_side_node->texture_nodes.erase(it);
      delete texture_node;
    }
  }

  for (const auto & tile_spec : to_add) {
    // Fixme: code !!!
    QcTileTexture * tile_texture = map_scene->m_tile_textures.value(tile_spec).data(); // Fixme: m_tile_textures public
    // qInfo() << "texture to add" << tile_spec << tile_texture;
    if (tile_texture && !tile_texture->image.isNull()) {
      // qInfo() << "create texture" << tile_spec;
      QSGSimpleTextureNode * tile_node = new QSGSimpleTextureNode();
      // note: setTexture will update coordinates so do it here, before we buildGeometry
      tile_node->setTexture(textures.value(tile_spec));
      if (map_scene->build_geometry(tile_spec, tile_node->geometry()->vertexDataAsTexturedPoint2D(), polygon)) {
        // && qgeotiledmapscene_isTileInViewport(tileNode->geometry()->vertexDataAsTexturedPoint2D(), map_side_node->matrix())
        tile_node->setFiltering(QSGTexture::Linear);
        map_side_node->add_child(tile_spec, tile_node);
      } else
        delete tile_node;
    }
  }
}

/**************************************************************************************************/

QcMapLayerScene::QcMapLayerScene(const QcWmtsPluginLayer * plugin_layer, const QcViewport * viewport, QObject * parent)
  : QObject(parent),
    m_plugin_layer(plugin_layer),
    m_viewport(viewport),
    m_tile_matrix_set(plugin_layer->plugin()->tile_matrix_set()),
    m_opacity(1.),
    m_scene_graph_node(nullptr)
{
  // qInfo();
}

QcMapLayerScene::~QcMapLayerScene()
{}

void
QcMapLayerScene::add_tile(const QcTileSpec & tile_spec, QSharedPointer<QcTileTexture> texture)
{
  if (m_visible_tiles.contains(tile_spec)) { // Don't add the geometry if it isn't visible
    m_tile_textures.insert(tile_spec, texture);
    // qInfo() << "add_tile" << tile_spec << "inserted";
  }
  // else
  //   qInfo() << "add_tile" << tile_spec << "already there";
}

void
QcMapLayerScene::set_visible_tiles(const QcTileSpecSet & tile_specs,
                                   const QcTileSpecSet & west_tile_specs,
                                   const QcTileSpecSet & central_tile_specs,
                                   const QcTileSpecSet & east_tile_specs)
{
  QcTileSpecSet to_remove = m_visible_tiles - tile_specs;
  if (!to_remove.isEmpty())
    remove_tiles(to_remove);
  m_visible_tiles = tile_specs;
  // Fixme: better ?
  m_west_visible_tiles = west_tile_specs;
  m_central_visible_tiles = central_tile_specs;
  m_east_visible_tiles = east_tile_specs;
}

void
QcMapLayerScene::remove_tiles(const QcTileSpecSet & old_tiles)
{
  // qInfo() << old_tiles;
  for (auto tile_spec : old_tiles)
    m_tile_textures.remove(tile_spec);
}

QcTileSpecSet
QcMapLayerScene::textured_tiles() const
{
  return QcTileSpecSet::fromList(m_tile_textures.keys());
}

bool
QcMapLayerScene::build_geometry(const QcTileSpec & tile_spec, QSGGeometry::TexturedPoint2D * vertices, const QcPolygon & polygon)
{
  int tile_size = m_tile_matrix_set.tile_size();
  const QcTileMatrix & tile_matrix = m_tile_matrix_set[m_viewport->zoom_level()];
  // double tile_length_m = tile_matrix.tile_length_m();
  double resolution = tile_matrix.resolution(); // [m/px]

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

  // qInfo() << "geometry" << tile_spec << "x" << x1 << x2 << "  y" << y1 << y2;

  return true;
}

QcMapLayerRootNode *
QcMapLayerScene::make_node()
{
  m_scene_graph_node = new QcMapLayerRootNode(m_tile_matrix_set, m_viewport);
  return m_scene_graph_node;
}

//! Transform the polygon to the tile referential
// Fixme: cf. map_view
QcPolygon
QcMapLayerScene::transform_polygon(const QcPolygon & polygon) const
{
  QcPolygon transformed_polygon;
  for (const auto & vertex : polygon.vertexes()) {
    auto transformed_vertex = (vertex - m_tile_matrix_set.origin()) * m_tile_matrix_set.scale();
    transformed_polygon.add_vertex(transformed_vertex);
  }
  return transformed_polygon;
}

void
QcMapLayerScene::update_scene_graph(QcMapLayerRootNode * map_root_node, QQuickWindow * window)
{
  // qInfo();

  if (map_root_node->opacity() != m_opacity)
    map_root_node->setOpacity(m_opacity);
  // dirty

  // Fixme: duplicated code?
  QcTileSpecSet textures_in_scene = QcTileSpecSet::fromList(map_root_node->textures.keys()); // cf. textured_tiles
  QcTileSpecSet to_remove = textures_in_scene - m_visible_tiles;
  QcTileSpecSet to_add = m_visible_tiles - textures_in_scene;
  // qInfo() << "textures in scene" << textures_in_scene
  //         << "to remove:" << to_remove
  //         << "to add" << to_add;
  for (const auto & tile_spec : to_remove)
    map_root_node->textures.take(tile_spec)->deleteLater();
  for (const auto & tile_spec : to_add) {
    QcTileTexture * tile_texture = m_tile_textures.value(tile_spec).data();
    if (tile_texture && !tile_texture->image.isNull()) {
      // qInfo() << "create texture from image" << tile_spec;
      QSGTexture * texture = window->createTextureFromImage(tile_texture->image);
      map_root_node->textures.insert(tile_spec, texture);
    }
  }

  const QcTileMatrix & tile_matrix = m_tile_matrix_set[m_viewport->zoom_level()];
  double resolution = tile_matrix.resolution(); // [m/px]

  // Fixme: should be called when west_part is true
  // when we cross west line
  const QcViewportPart & west_part = m_viewport->west_part();
  map_root_node->update_tiles(this,
                              map_root_node->west_map_node,
                              m_west_visible_tiles,
                              transform_polygon(west_part.polygon()),
                              west_part);

  const QcViewportPart & central_part = m_viewport->central_part();
  QcPolygon transformed_central_polygon = transform_polygon(central_part.polygon());
  // qInfo() << "central" << central_offset;
  map_root_node->update_tiles(this,
                              map_root_node->central_map_node,
                              m_central_visible_tiles,
                              transformed_central_polygon,
                              central_part);
  map_root_node->update_central_maps();
  int clone_index = 0;
  const QList<QcViewportPart> & clone_parts = m_viewport->central_part_clones();
  for (auto * node : map_root_node->central_map_nodes) {
    // qInfo() << "clone" << east_offset;
    map_root_node->update_tiles(this,
                                node,
                                m_central_visible_tiles,
                                transformed_central_polygon,
                                clone_parts[clone_index++]);
  }

  // qInfo() << "east" << east_offset;
  const QcViewportPart & east_part = m_viewport->east_part();
  map_root_node->update_tiles(this,
                              map_root_node->east_map_node,
                              m_east_visible_tiles,
                              transform_polygon(east_part.polygon()),
                              east_part);
}

/**************************************************************************************************/

// #include "map_layer_scene.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
