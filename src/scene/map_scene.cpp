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

QcMapScene::QcMapScene(const QcViewport * viewport, QObject * parent)
  : QObject(parent),
    m_viewport(viewport)
{}

QcMapScene::~QcMapScene()
{}

QcMapLayerScene *
QcMapScene::add_layer(const QcWmtsPluginLayer * plugin_layer)
{
  QcMapLayerScene * layer = new QcMapLayerScene(plugin_layer, m_viewport);
  m_layers << layer;
  m_layer_map.insert(plugin_layer->hash_name(), layer);
  return layer;
}

void
QcMapScene::remove_layer(const QcWmtsPluginLayer * plugin_layer)
{
  QString name = plugin_layer->hash_name();
  if (m_layer_map.contains(name)) {
    QcMapLayerScene * layer = m_layer_map[name];
    m_layers.removeOne(layer);
    m_layer_map.remove(name);
    m_scene_graph_nodes_to_remove << layer->scene_graph_node();
    layer->deleteLater();
  }
}

void
QcMapScene::update_path(const QcPathDouble & path) {
  m_path = path;
  m_dirty_path = true;
}

void
QcMapScene::set_dirty_path()
{
  m_dirty_path = true;
  // Fixme:
  m_dirty_location_circle = true;
}

void
QcMapScene::set_gps_horizontal_precision(double horizontal_precision)
{
  m_gps_horizontal_precision = horizontal_precision;
  m_dirty_location_circle = true;
}

QSGNode *
QcMapScene::update_scene_graph(QSGNode * old_node, QQuickWindow * window)
{
  // qInfo() << old_node;

  // QSize viewport_size = m_viewport->viewport_size();
  float width = m_viewport->width();
  float height = m_viewport->height();
  // qInfo() << "viewport size" << viewport_size;
  // Check viewport has a rectangular shape
  if (width <= 0 || height <= 0) {
    delete old_node;
    return nullptr;
  }

  QcMapRootNode * map_root_node = static_cast<QcMapRootNode *>(old_node);
  if (!map_root_node) {
    // qInfo() << "map_root_node is null";
    map_root_node = new QcMapRootNode(m_viewport);
  }

  // Fixme: ok ?
  map_root_node->update_clip_rect();

  /* Reset matrix to physical pixel
   * | 2S/W   0     -1 |
   * |  0    2S/H   -1 |
   */
  QMatrix4x4 item_space_matrix;
  qreal device_pixel_ratio_inverse = 1.; // / window->devicePixelRatio();
  item_space_matrix.scale(device_pixel_ratio_inverse, device_pixel_ratio_inverse);
  map_root_node->root->setMatrix(item_space_matrix);
  // qInfo() << "item_space_matrix" << item_space_matrix;

  // Remove disabled layers
  for (auto * node : m_scene_graph_nodes_to_remove)
    map_root_node->root->removeChildNode(node);
  m_scene_graph_nodes_to_remove.clear();

  for (auto * layer : m_layers) {
    QcMapLayerRootNode * layer_node = nullptr;
    QString name = layer->name();
    if (map_root_node->layers.contains(name))
      layer_node = map_root_node->layers[name];
    else {
      layer_node = layer->make_node();
      map_root_node->layers.insert(name, layer_node);
      map_root_node->root->insertChildNodeBefore(layer_node, map_root_node->location_circle_node);
    }
    layer->update_scene_graph(layer_node, window);
  }

  if (m_dirty_path) {
    qInfo() << "Path is dirty" << m_path.number_of_edges();
    if (m_path.number_of_edges())
      map_root_node->path_node->update(m_path);
    m_dirty_path = false;
  }

  if (m_dirty_location_circle) {
    qInfo() << "Location circle is dirty";
    map_root_node->location_circle_node->update(m_gps_horizontal_precision);
    m_dirty_location_circle = false;
  }

  // Fixme: map_root_node->grid_node->update();

  return map_root_node;
}

/**************************************************************************************************/

QcMapRootNode::QcMapRootNode(const QcViewport * viewport)
  : m_viewport(viewport),
    geometry(QSGGeometry::defaultAttributes_Point2D(), 4),
    root(new QSGTransformNode()),
    location_circle_node(new QcLocationCircleNode(viewport)),
    path_node(new QcPathNode(viewport))
{
  // qInfo();
  setIsRectangular(true);
  setGeometry(&geometry);
  appendChildNode(root);

  root->appendChildNode(location_circle_node);
  root->appendChildNode(path_node);
}

QcMapRootNode::~QcMapRootNode()
{}

void
QcMapRootNode::update_clip_rect()
{
  int width = m_viewport->width();
  int height = m_viewport->height();
  QRect rect = QRect(0, 0, width, height);
  // qInfo() << "clip rect" << rect;
  if (rect != m_clip_rect) {
    QSGGeometry::updateRectGeometry(&geometry, rect);
    QSGClipNode::setClipRect(rect);
    m_clip_rect = rect;
    markDirty(DirtyGeometry);
  }
}

/**************************************************************************************************/

// #include "map_scene.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
