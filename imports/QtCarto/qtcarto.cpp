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

#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/qqml.h>

#include <qdebug.h>

#include <QtCore/QDebug>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

#include <QtQuick/QQuickItem>

class TestItem : public QQuickItem
{
  Q_OBJECT

  // Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
  // Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
  // Q_PROPERTY(QPointF p3 READ p3 WRITE setP3 NOTIFY p3Changed)
  // Q_PROPERTY(QPointF p4 READ p4 WRITE setP4 NOTIFY p4Changed)

  // Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)

public:
  TestItem(QQuickItem *parent = 0);
  ~TestItem();

  QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

  // QPointF p1() const { return m_p1; }
  // QPointF p2() const { return m_p2; }
  // QPointF p3() const { return m_p3; }
  // QPointF p4() const { return m_p4; }

  // int segmentCount() const { return m_segmentCount; }

  // void setP1(const QPointF &p);
  // void setP2(const QPointF &p);
  // void setP3(const QPointF &p);
  // void setP4(const QPointF &p);

  // void setSegmentCount(int count);

  // signals:
  // void p1Changed(const QPointF &p);
  // void p2Changed(const QPointF &p);
  // void p3Changed(const QPointF &p);
  // void p4Changed(const QPointF &p);

  // void segmentCountChanged(int count);

  // private:
  // QPointF m_p1;
  // QPointF m_p2;
  // QPointF m_p3;
  // QPointF m_p4;

  // int m_segmentCount;
};

/**************************************************************************************************/

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>

TestItem::TestItem(QQuickItem *parent)
  : QQuickItem(parent)
    // , m_p1(0, 0)
    // , m_p2(1, 0)
    // , m_p3(0, 1)
    // , m_p4(1, 1)
    // , m_segmentCount(32)
{
  setFlag(ItemHasContents, true);
}

TestItem::~TestItem()
{
}

// void TestItem::setP1(const QPointF &p)
// {
//     if (p == m_p1)
//         return;

//     m_p1 = p;
//     emit p1Changed(p);
//     update();
// }

// void TestItem::setP2(const QPointF &p)
// {
//     if (p == m_p2)
//         return;

//     m_p2 = p;
//     emit p2Changed(p);
//     update();
// }

// void TestItem::setP3(const QPointF &p)
// {
//     if (p == m_p3)
//         return;

//     m_p3 = p;
//     emit p3Changed(p);
//     update();
// }

// void TestItem::setP4(const QPointF &p)
// {
//     if (p == m_p4)
//         return;

//     m_p4 = p;
//     emit p4Changed(p);
//     update();
// }

// void TestItem::setSegmentCount(int count)
// {
//     if (m_segmentCount == count)
//         return;

//     m_segmentCount = count;
//     emit segmentCountChanged(count);
//     update();
// }

QSGNode *
TestItem::updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *)
{
  qInfo() << "updatePaintNode" << old_node;

  const int vertex_count = 2;

  QSGGeometryNode * node = 0;
  QSGGeometry * geometry = 0;

  if (!old_node) {
    node = new QSGGeometryNode;

    geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertex_count);
    geometry->setLineWidth(2);
    geometry->setDrawingMode(GL_LINE_STRIP);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);

    QSGFlatColorMaterial * material = new QSGFlatColorMaterial;
    material->setColor(QColor(255, 0, 0));
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);
  } else {
    node = static_cast<QSGGeometryNode *>(old_node);
    geometry = node->geometry();
    // geometry->allocate(vertex_count);
  }

  QRectF bounds = boundingRect();
  qInfo() << bounds;
  QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();
  float x, y;
  x = bounds.x() + 0 * bounds.width();
  y = bounds.y() + 0 * bounds.height();
  qInfo() << x << y;
  vertices[0].set(x, y);
  x = bounds.x() + 1. * bounds.width();
  y = bounds.y() + 1. * bounds.height();
  qInfo() << x << y;
  vertices[1].set(x, y);
  node->markDirty(QSGNode::DirtyGeometry);

  return node;
}

/**************************************************************************************************/

class QtCartoDeclarativeModule : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0"
		    FILE "plugin.json")

 public:
  void registerTypes(const char *uri)
  {
    // Q_ASSERT(uri == QLatin1String("QtCarto"));
    if (QLatin1String(uri) == QLatin1String("QtCarto")) {

      // @uri QtCarto
      int major = 1;
      int minor = 0;

      // Register the 1.0 types

      qmlRegisterType<TestItem>(uri, 1, 0, "TestItem");

      // qmlRegisterType<QDeclarativeGeoMap>(uri, major, minor, "Map");

      // qmlRegisterUncreatableType<QGeoMapPinchEvent >(uri, major, minor, "MapPinchEvent",
      // 						     QStringLiteral("(Map)PinchEvent is not intended instantiable by developer."));
      // qmlRegisterUncreatableType<QQuickGeoMapGestureArea>(uri, major, minor, "MapGestureArea",
      // 							  QStringLiteral("(Map)GestureArea is not intended instantiable by developer."));
      // qmlRegisterUncreatableType<QQuickGeoMapGestureArea, 1>(uri, major, minor, "MapGestureArea",
      // 							     QStringLiteral("(Map)GestureArea is not intended instantiable by developer."));

      // registrations below are version independent
    } else {
      qDebug() << "Unsupported URI given to load location QML plugin: " << QLatin1String(uri);
    }
  }
};

/**************************************************************************************************/

#include "qtcarto.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
