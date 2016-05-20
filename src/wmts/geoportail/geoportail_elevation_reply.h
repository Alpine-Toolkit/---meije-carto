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

#ifndef __GEOPORTAIL_ELEVATION_REPLY_H__
#define __GEOPORTAIL_ELEVATION_REPLY_H__

/**************************************************************************************************/

#include "coordinate/geo_coordinate.h"
#include "wmts/network_reply.h"

#include <QNetworkReply>
#include <QPointer>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcGeoportailElevationReply : public QcNetworkReply // QcElevationReply
{
  Q_OBJECT

public:
  explicit QcGeoportailElevationReply(QNetworkReply * reply, const QVector<QcGeoCoordinateWGS84> & coordinates);
  ~QcGeoportailElevationReply();

  const QVector<QcGeoElevationCoordinateWGS84> & elevations() const { return m_elevations; }

  virtual void abort();

private slots:
  void network_reply_finished();
  void network_reply_error(QNetworkReply::NetworkError error);

private:
  QPointer<QNetworkReply> m_reply;
  const QVector<QcGeoCoordinateWGS84> & m_coordinates;
  QVector<QcGeoElevationCoordinateWGS84> m_elevations;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_ELEVATION_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
