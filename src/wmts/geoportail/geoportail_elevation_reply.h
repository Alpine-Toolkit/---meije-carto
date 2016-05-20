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

#include <QNetworkReply>
#include <QPointer>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcGeoportailElevationReply : public QObject // QcElevationReply
{
  Q_OBJECT

public:
  enum Error { // Fixme: check
    NoError,
    CommunicationError,
    ParseError,
    UnknownError
  };

public:
  explicit QcGeoportailElevationReply(QNetworkReply * reply, const QVector<QcGeoCoordinateWGS84> & coordinates);
  ~QcGeoportailElevationReply();

  const QVector<QcGeoElevationCoordinateWGS84> & elevations() const { return m_elevations; }

  bool is_finished() const;
  Error error() const;
  QString error_string() const;

  virtual void abort();

  QNetworkReply * network_reply() const; // Fixme: ???

signals:
  void finished();
  void error(Error error, const QString & error_string = QString());

protected:
  void set_error(Error error, const QString & error_string);
  void set_finished(bool finished);

private:
  Q_DISABLE_COPY(QcGeoportailElevationReply);

private slots:
  void network_reply_finished();
  void network_reply_error(QNetworkReply::NetworkError error);

private:
  QPointer<QNetworkReply> m_reply;
  const QVector<QcGeoCoordinateWGS84> & m_coordinates;
  QVector<QcGeoElevationCoordinateWGS84> m_elevations;
  Error m_error;
  QString m_error_string;
  bool m_is_finished;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_ELEVATION_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
