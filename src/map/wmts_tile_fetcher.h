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

#ifndef __WMTS_TILE_FETCHER_H__
#define __WMTS_TILE_FETCHER_H__

/**************************************************************************************************/

#include <QObject>

#include <QHash>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QSize>
#include <QTimer>

#include "qtcarto_global.h"
#include "tile_spec.h"
#include "wmts_reply.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

// class QGeoMapRequestOptions;

// class QGeoTiledMappingManagerEngine;

class QC_EXPORT QcWmtsTileFetcher : public QObject
{
  Q_OBJECT

 public:
  QcWmtsTileFetcher(QObject * parent = 0);
  virtual ~QcWmtsTileFetcher();

 public slots:
  void update_tile_requests(const QSet<QcTileSpec> & tiles_added, const QSet<QcTileSpec> & tiles_removed);

 private slots:
  void cancel_tile_requests(const QSet<QcTileSpec> & tile_specs);
  void request_next_tile();
  void finished();

 signals:
  void tile_finished(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format);
  void tile_error(const QcTileSpec & tile_spec, const QString & errorString);

 protected:
  void timerEvent(QTimerEvent * event);
  // QGeoTiledMappingManagerEngine::CacheAreas cache_hint() const;

 private:
  virtual QcWmtsReply * get_tile_image(const QcTileSpec & tile_spec) = 0;
  void handle_reply(QcWmtsReply * wmts_reply, const QcTileSpec & tile_spec);

  // Q_DECLARE_PRIVATE(QcWmtsTileFetcher);
  // Q_DISABLE_COPY(QcWmtsTileFetcher);

  // friend class QGeoTiledMappingManagerEngine;

 private:
  bool m_enabled;
  QBasicTimer m_timer;
  QMutex m_queue_mutex;
  QList<QcTileSpec> m_queue;
  QHash<QcTileSpec, QcWmtsReply *> m_invmap;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_TILE_FETCHER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
