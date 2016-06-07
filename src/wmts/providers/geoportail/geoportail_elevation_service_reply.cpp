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

#include "geoportail_elevation_service_reply.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

/**************************************************************************************************/

QcGeoportailElevationServiceReply::QcGeoportailElevationServiceReply(QNetworkReply * reply,
                                                                     const QVector<QcWgsCoordinate> & coordinates)
  : QcElevationServiceReply(reply, coordinates)
{}

QcGeoportailElevationServiceReply::~QcGeoportailElevationServiceReply()
{}

// Handle a successful request : store image data
void
QcGeoportailElevationServiceReply::process_payload()
{
  QString ELEVATIONS = "elevations";
  QByteArray json_data = network_reply()->readAll();
  // { "elevations": [ { "lon": 0.23, "lat": 48.05, "z": 112.73, "acc": 2.5}, ... ] }
  // qInfo() << json_data;
  QJsonDocument json_document(QJsonDocument::fromJson(json_data));
  const QJsonObject & json_root_object = json_document.object();
  if (json_root_object.contains(ELEVATIONS)) {
    QJsonArray json_array = json_root_object[ELEVATIONS].toArray();
    for (const auto & json_ref : json_array) {
      const QJsonObject & json_object = json_ref.toObject();
      double longitude = json_object["lon"].toDouble();
      double latitude = json_object["lat"].toDouble();
      double elevation = json_object["z"].toDouble();
      double elevation_accuracy = json_object["acc"].toDouble(); // unused
      elevations() << QcWgsElevationCoordinate(longitude, latitude, elevation);
    }
  }
  qInfo() << elevations();
}

/**************************************************************************************************/

// #include "geoportail_elevation_service_reply.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
