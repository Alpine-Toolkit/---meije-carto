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

#ifndef __WMTS_REPLY_H__
#define __WMTS_REPLY_H__

/**************************************************************************************************/

#include "qtcarto_global.h"
#include "wmts/network_reply.h"
#include "wmts/tile_spec.h"

#include <QByteArray>
#include <QObject>
#include <QString>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// QcWmtsReply is a kind of future

class QC_EXPORT QcWmtsReply : public QcNetworkReply // QcNetworkFuture
{
  Q_OBJECT

 public:
  QcWmtsReply(QNetworkReply * reply, const QcTileSpec & tile_spec);
  virtual ~QcWmtsReply();

  //! Returns the request which corresponds to this reply.
  QcTileSpec tile_spec() const { return m_tile_spec; }

  //!   Returns the tile image data.
  QByteArray map_image_data() const { return m_map_image_data; }
  // Returns the format of the tile image.
  QString map_image_format() const { return m_map_image_format; }

 protected:
  //! Sets the tile image data to \a data.
  void set_map_image_data(const QByteArray & data) { m_map_image_data = data; }
  //! Sets the format of the tile image to \a format.
  void set_map_image_format(const QString & format) { m_map_image_format = format; }

 private:
  Q_DISABLE_COPY(QcWmtsReply);

 private:
  QcTileSpec m_tile_spec;
  QByteArray m_map_image_data;
  QString m_map_image_format;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
