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

#ifndef __ELEVATION_SERVICE_REPLY_H__
#define __ELEVATION_SERVICE_REPLY_H__

/**************************************************************************************************/

#include "coordinate/mercator.h"
#include "coordinate/wgs84.h"
#include "wmts/network_reply.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcElevationServiceReply : public QcNetworkReply
{
  Q_OBJECT

public:
  explicit QcElevationServiceReply(QNetworkReply * reply, const QVector<QcWgsCoordinate> & coordinates);
  ~QcElevationServiceReply();

  const QVector<QcWgsElevationCoordinate> & elevations() const { return m_elevations; }

protected:
  QVector<QcWgsElevationCoordinate> & elevations() { return m_elevations; }

private:
  const QVector<QcWgsCoordinate> & m_coordinates;
  QVector<QcWgsElevationCoordinate> m_elevations;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __ELEVATION_SERVICE_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
