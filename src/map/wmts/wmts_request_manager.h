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

/****************************************************************************
 **
 ** Copyright (C) 2015 The Qt Company Ltd.
 ** Contact: http://www.qt.io/licensing/
 **
 ** This file is part of the QtLocation module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL3$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see http://www.qt.io/terms-conditions. For further
 ** information use the contact form at http://www.qt.io/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPLv3 included in the
 ** packaging of this file. Please review the following information to
 ** ensure the GNU Lesser General Public License version 3 requirements
 ** will be met: https://www.gnu.org/licenses/lgpl.html.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 2.0 or later as published by the Free
 ** Software Foundation and appearing in the file LICENSE.GPL included in
 ** the packaging of this file. Please review the following information to
 ** ensure the GNU General Public License version 2.0 requirements will be
 ** met: http://www.gnu.org/licenses/gpl-2.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

/**************************************************************************************************/

#ifndef __WMTS_REQUEST_MANAGER_H__
#define __WMTS_REQUEST_MANAGER_H__

/**************************************************************************************************/

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QSharedPointer>
#include <QSize>

#include "qtcarto_global.h"
#include "map/cache/file_tile_cache.h"
#include "map/wmts/tile_spec.h"
#include "map/wmts/wmts_manager.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcMapViewLayer; // circular

/**************************************************************************************************/

// Represents a tile that needs to be retried after a certain period of time
class QcRetryFuture : public QObject
{
  Q_OBJECT

 public:
  QcRetryFuture(const QcTileSpec & tile_spec, QcMapViewLayer * map_view_layer, QcWmtsManager * wmts_manager);

 public slots:
  void retry();

 private:
  QcTileSpec m_tile_spec;
  QcMapViewLayer * m_map_view_layer;
  QPointer<QcWmtsManager> m_wmts_manager;
};

/*! This class implements a WMTS Request Manager for a map view.
 *
 * It works as a proxy between the map view and WTMS Request Manager.
 *
 */
class QcWmtsRequestManager : public QObject
{
  Q_OBJECT

 public:
  explicit QcWmtsRequestManager(QcMapViewLayer * map_view_layer, QcWmtsManager * wmts_manager);
  ~QcWmtsRequestManager();

  QList<QSharedPointer<QcTileTexture> > request_tiles(const QcTileSpecSet & tile_specs);

  void tile_fetched(const QcTileSpec & tile_spec);
  void tile_error(const QcTileSpec & tile_spec, const QString & error_string);

  QSharedPointer<QcTileTexture> tile_texture(const QcTileSpec & tile_spec);

 private:
  QcMapViewLayer * m_map_view_layer;
  QPointer<QcWmtsManager> m_wmts_manager;
  QHash<QcTileSpec, int> m_retries;
  QHash<QcTileSpec, QSharedPointer<QcRetryFuture> > m_futures;
  QcTileSpecSet m_requested;

  Q_DISABLE_COPY(QcWmtsRequestManager)
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_REQUEST_MANAGER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
