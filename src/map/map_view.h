// -*- mode: c++ -*-

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

#ifndef __MAP_VIEW_H__
#define __MAP_VIEW_H__

/**************************************************************************************************/

#include <QObject>

#include "qtcarto_global.h"
#include "map/map_scene.h"
#include "map/tile_spec.h"
#include "map/wmts_manager.h" // circular
#include "map/wmts_plugin.h" // circular
#include "map/wmts_request_manager.h" // circular
#include "map/viewport.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

// class QcWmtsManager;
// class QcWmtsRequestManager;

/**************************************************************************************************/

class QC_EXPORT QcMapView : public QObject
{
  Q_OBJECT

 public:
  QcMapView(QcWmtsPlugin * wmts_plugin);
  ~QcMapView();

  QcWmtsRequestManager * request_manager() { return m_request_manager; };
  QcViewport * viewport() { return m_viewport; };

  void update_tile(const QcTileSpec & tile_spec);

  QSGNode * update_scene_graph(QSGNode *old_node, QQuickWindow *window)
  {
    return m_map_scene->update_scene_graph(old_node, window);
  }

  QcMapScene * map_scene() { return m_map_scene; };

 private:
  QcWmtsPlugin * m_plugin; // could use several plugins !
  QPointer<QcWmtsManager> m_wmts_manager; // Fixme: why QPointer !
  QcWmtsRequestManager * m_request_manager;
  QcViewport * m_viewport;
  QcMapScene * m_map_scene;
};

typedef QSet<QcMapView *> QcMapViewPointerSet;

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __MAP_VIEW_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
