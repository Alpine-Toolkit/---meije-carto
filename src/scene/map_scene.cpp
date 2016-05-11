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
#include "location_circle_material_shader.h"

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
QcMapScene::add_layer(QcWmtsPluginMap plugin_map)
{
  QcMapLayerScene * layer = new QcMapLayerScene(plugin_map, m_viewport);
  m_layers << layer;
  m_layer_map.insert(plugin_map.name(), layer);
  return layer;
}

void
QcMapScene::remove_layer(QcWmtsPluginMap plugin_map)
{
  QString name = plugin_map.name();
  if (m_layer_map.contains(name)) {
    QcMapLayerScene * layer = m_layer_map[name];
    m_layers.removeOne(layer);
    layer->deleteLater();
  }
}

QSGNode *
QcMapScene::update_scene_graph(QSGNode * old_node, QQuickWindow * window)
{
  qInfo() << old_node;

  QSize viewport_size = m_viewport->viewport_size();
  float width = m_viewport->width();
  float height = m_viewport->height();
  qInfo() << "viewport size" << viewport_size;
  // Check viewport has a rectangular shape
  if (width <= 0 || height <= 0) {
    delete old_node;
    return nullptr;
  }

  QcMapRootNode * map_root_node = static_cast<QcMapRootNode *>(old_node);
  if (!map_root_node) {
    qInfo() << "map_root_node is null";
    map_root_node = new QcMapRootNode(m_viewport);
  }

  // Fixme: ok ?
  map_root_node->update_clip_rect();

  /* Setup model matrix
   *   origin at center
   *   y downward
   *   diagonal is set (1, 1)
   */
  QMatrix4x4 item_space_matrix;
  item_space_matrix.scale(width/2, height/2);
  item_space_matrix.translate(1, 1);
  map_root_node->root->setMatrix(item_space_matrix);
  qInfo() << "item_space_matrix" << item_space_matrix;

  for (auto * layer : m_layers) {
    QcMapLayerRootNode * layer_node = nullptr;
    QString name = layer->name();
    if (map_root_node->layers.contains(name))
      layer_node = map_root_node->layers[name];
    else {
      layer_node = layer->make_node();
      map_root_node->layers.insert(name, layer_node);
      map_root_node->root->insertChildNodeBefore(layer_node, map_root_node->location_circle_root_node);
    }
    layer->update_scene_graph(layer_node, window);
  }
  // Fixme: map_root_node->grid_node->update();

  return map_root_node;
}

/**************************************************************************************************/

QcMapRootNode::QcMapRootNode(const QcViewport * viewport)
  : m_viewport(viewport),
    geometry(QSGGeometry::defaultAttributes_Point2D(), 4),
    root(new QSGTransformNode()),
    location_circle_root_node(new QSGOpacityNode())
{
  qInfo();
  setIsRectangular(true);
  setGeometry(&geometry);
  appendChildNode(root);

#ifndef ANDROID
  location_circle_root_node->setOpacity(.25); // 1. black

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
  location_circle_material->state()->a = 1.;
  location_circle_node->setMaterial(location_circle_material);
  location_circle_node->setFlag(QSGNode::OwnsMaterial);

  QSGGeometry::Point2D * location_circle_vertices = location_circle_geometry->vertexDataAsPoint2D();
  location_circle_vertices[0].set(0, 0);

  location_circle_root_node->appendChildNode(location_circle_node);
  // root->appendChildNode(location_circle_node);
  root->appendChildNode(location_circle_root_node);
#endif
}

QcMapRootNode::~QcMapRootNode()
{}

void
QcMapRootNode::update_clip_rect()
{
  int width = m_viewport->width();
  int height = m_viewport->height();
  QRect rect = QRect(0, 0, width, height);
  qInfo() << rect;
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
