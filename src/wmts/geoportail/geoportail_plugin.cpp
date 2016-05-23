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

#include "geoportail_plugin.h"
#include "geoportail_wmts_tile_fetcher.h"

#include <QNetworkAccessManager>
#include <QString>
#include <QXmlStreamWriter>

/**************************************************************************************************/

QcGeoportailLayer::QcGeoportailLayer(QcGeoportailPlugin * plugin,
                                     int map_id,
                                     int position,
                                     const QString & title,
                                     const QString & name,
                                     const QString & image_format,
                                     const QString & style)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format),
    m_style(style)
{}

QcGeoportailLayer::QcGeoportailLayer(const QcGeoportailLayer & other)
  : QcWmtsPluginLayer(other),
    m_style(other.m_style)
{}

QcGeoportailLayer::~QcGeoportailLayer()
{}

QcGeoportailLayer &
QcGeoportailLayer::operator=(const QcGeoportailLayer & other)
{
  if (this != &other) {
    QcWmtsPluginLayer::operator=(other);
    m_style = other.m_style;
  }

  return *this;
}

QUrl
QcGeoportailLayer::url(const QcTileSpec & tile_spec) const
{
  /*
   * Vue aérienne LAYER = ORTHOIMAGERY.ORTHOPHOTOS
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=ORTHOIMAGERY.ORTHOPHOTOS
   * &STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23327&TILECOL=33919&FORMAT=image/jpeg
   *
   * Carte LAYER = GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD
   * &STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23327&TILECOL=33920&FORMAT=image/jpeg
   *
   * Carte topographique LAYER = GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR
   * &STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23326&TILECOL=33920&FORMAT=image/jpeg
   *
   * Parcelles cadastrales
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=CADASTRALPARCELS.PARCELS
   * &STYLE=bdparcellaire&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23325&TILECOL=33915&FORMAT=image/png
   *
   * Routes
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=TRANSPORTNETWORKS.ROADS&STYLE=normal
   * &TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23326&TILECOL=33917&FORMAT=image/png
   *
   */

  // Fixme: name, split ???
  QStringList parameters;
  parameters << QStringLiteral("SERVICE=WMTS");
  parameters << QStringLiteral("VERSION=1.0.0");
  parameters << QStringLiteral("REQUEST=GetTile");
  parameters << QStringLiteral("LAYER=") + name();
  parameters << QStringLiteral("STYLE=") + m_style;
  parameters << QStringLiteral("FORMAT=image/") + image_format();
  parameters << QStringLiteral("TILEMATRIXSET=PM");
  parameters << QStringLiteral("TILEMATRIX=") + QString::number(tile_spec.level());
  parameters << QStringLiteral("TILEROW=") + QString::number(tile_spec.y());
  parameters << QStringLiteral("TILECOL=") + QString::number(tile_spec.x());

  QString api_key = static_cast<QcGeoportailPlugin *>(plugin())->license().api_key();

  return QUrl(QStringLiteral("https://wxs.ign.fr/") +
              api_key +
              QStringLiteral("/geoportail/wmts?") +
              parameters.join(QString('&')));
}

/**************************************************************************************************/

const QString QcGeoportailPlugin::PLUGIN_NAME = "geoportail";
const QString PLUGIN_TITLE = "Géoportail";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcGeoportailPlugin::QcGeoportailPlugin(const QcGeoportailWmtsLicense & license)
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE, NUMBER_OF_LEVELS, TILE_SIZE),
    m_license(license),
    m_network_manager(new QNetworkAccessManager()),
    m_user_agent("QtCarto based application"),
    m_tile_fetcher(this)
{
  connect(m_network_manager,
          SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
	  this,
	  SLOT(on_authentication_request_slot(QNetworkReply*, QAuthenticator*)));

  wmts_manager()->set_tile_fetcher(&m_tile_fetcher);
  wmts_manager()->tile_cache(); // create a file tile cache

  // wmts_manager()->tile_cache()->clear_all();

  // Fixme: to json
  int map_id = -1;
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 1
                                  2,
                                  QStringLiteral("Carte topographique"),
                                  QLatin1Literal("GEOGRAPHICALGRIDSYSTEMS.MAPS"), // .SCAN25TOUR
                                  QLatin1Literal("jpeg"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 2
                                  3,
                                  QStringLiteral("Vue aérienne"),
                                  QLatin1Literal("ORTHOIMAGERY.ORTHOPHOTOS"),
                                  QLatin1Literal("jpeg"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 0
                                  1,
                                  QStringLiteral("Carte"),
                                  QLatin1Literal("GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD"),
                                  QLatin1Literal("jpeg"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 3
                                  4,
                                  QStringLiteral("Routes"),
                                  QLatin1Literal("TRANSPORTNETWORKS.ROADS"),
                                  QLatin1Literal("png"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 4
                                  5,
                                  QStringLiteral("Parcelles cadastrales"),
                                  QLatin1Literal("CADASTRALPARCELS.PARCELS"),
                                  QLatin1Literal("png"),
                                  QLatin1Literal("bdparcellaire")
                                  ));

  // QVector<QcGeoCoordinateWGS84> coordinates;
  // coordinates << QcGeoCoordinateWGS84(0.23, 48.05);
  // coordinates << QcGeoCoordinateWGS84(2.15, 46.60);
  // coordinates << QcGeoCoordinateWGS84(4.39, 43.91);
  // m_elevation_reply = coordinate_elevations(coordinates);

  // QcLocationServiceQuery query;
  // query.set_request_id(QString::number(1));
  // query.set_maximum_responses(1);
  // query.set_srs_name(QLatin1Literal("epsg:4326"));
  // query.set_country_code(QLatin1Literal("StreetAddress"));
  // // QLatin1Literal("PositionOfInterest")
  // // QLatin1Literal("Administratif")
  // // QLatin1Literal("CadastralParcel")
  // query.set_free_form_address("7 rue jean baillet 95870 bezons");
  // m_location_reply = geocode_request(query);

  QcLocationServiceReverseQuery query;
  query.set_request_id(QString::number(1));
  query.set_maximum_responses(1);
  // query.set_srs_name(QLatin1Literal("epsg:4326"));
  query.set_coordinate(QcGeoCoordinateWGS84(2.3241667, 48.8033333));
  m_location_reverse_reply = reverse_geocode_request(query);
}

QcGeoportailPlugin::~QcGeoportailPlugin()
{
  delete m_network_manager;
}

// Fixme: move to wmts_plugin
QNetworkReply *
QcGeoportailPlugin::get(const QUrl & url) const
{
  QNetworkRequest request;
  request.setRawHeader("User-Agent", m_user_agent);
  request.setUrl(url);

  QNetworkReply * reply = m_network_manager->get(request);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return reply;
}

QNetworkReply *
QcGeoportailPlugin::post(const QUrl & url, const QByteArray & data) const
{
  QNetworkRequest request;
  request.setHeader(QNetworkRequest::UserAgentHeader, m_user_agent);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml;charset=UTF-8");
  request.setUrl(url);

  QNetworkReply * reply = m_network_manager->post(request, data);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return reply;
}

void
QcGeoportailPlugin::on_authentication_request_slot(QNetworkReply * reply,
                                                   QAuthenticator * authenticator)
{
  Q_UNUSED(reply);
  // qInfo() << "on_authentication_request_slot";
  authenticator->setUser(m_license.user());
  authenticator->setPassword(m_license.password());
}

QSharedPointer<QcLocationServiceReply>
QcGeoportailPlugin::geocode_request(const QcLocationServiceQuery & query) const
{
  QByteArray xml_request;
  QXmlStreamWriter stream(&xml_request);
  stream.setAutoFormatting(true);
  stream.setCodec("UTF-8");
  stream.writeStartDocument();
  stream.writeStartElement(QLatin1Literal("XLS"));
  QXmlStreamAttributes xls_attributes;
  xls_attributes.append(QLatin1Literal("xmlns:xls"), QLatin1Literal("http://www.opengis.net/xls"));
  xls_attributes.append(QLatin1Literal("xmlns:gml"), QLatin1Literal("http://www.opengis.net/gml"));
  xls_attributes.append(QLatin1Literal("xmlns"), QLatin1Literal("http://www.opengis.net/xls"));
  xls_attributes.append(QLatin1Literal("xmlns:xsi"), QLatin1Literal("http://www.w3.org/2001/XMLSchema-instance"));
  xls_attributes.append(QLatin1Literal("version"), QLatin1Literal("1.2"));
  xls_attributes.append(QLatin1Literal("xsi:schemaLocation"), QLatin1Literal("http://www.opengis.net/xls http://schemas.opengis.net/ols/1.2/olsAll.xsd"));
  stream.writeAttributes(xls_attributes);
  stream.writeEmptyElement(QLatin1Literal("RequestHeader"));
  QXmlStreamAttributes header_attributes;
  header_attributes.append(QLatin1Literal("srsName"), query.srs_name());
  stream.writeStartElement(QLatin1Literal("Request"));
  QXmlStreamAttributes request_attributes;
  request_attributes.append(QLatin1Literal("maximumResponses"), QString::number(query.maximum_responses()));
  request_attributes.append(QLatin1Literal("requestID"), query.request_id());
  request_attributes.append(QLatin1Literal("version"), QLatin1Literal("1.2"));
  request_attributes.append(QLatin1Literal("methodName"), QLatin1Literal("LocationUtilityService"));
  stream.writeAttributes(request_attributes);

  stream.writeStartElement(QLatin1Literal("GeocodeRequest"));
  stream.writeAttribute(QLatin1Literal("returnFreeForm"), QLatin1Literal("false"));
  stream.writeStartElement(QLatin1Literal("Address"));
  stream.writeAttribute(QLatin1Literal("countryCode"), query.country_code());
  if (!query.free_form_address().isEmpty())
    stream.writeTextElement(QLatin1Literal("freeFormAddress"), query.free_form_address());
  else if (!query.street().isEmpty()) {
    stream.writeStartElement(QLatin1Literal("StreetAddress"));
    stream.writeTextElement(QLatin1Literal("Street"), query.street());
    stream.writeEndElement();
    stream.writeTextElement(QLatin1Literal("PostalCode"), query.postale_code());
    // for (const auto & pair : query.place()) {
    const QMap<QString, QString> & place = query.place();
    QMap<QString, QString>::const_iterator it;
    for (it = place.begin(); it != place.end(); ++it)
      stream.writeTextElement(it.key(), it.value());
  }
  stream.writeEndElement();

  stream.writeEndElement();
  stream.writeEndDocument();

  // http://wxs.ign.fr/<API_KEY>/geoportail/ols?
  QUrl url = QStringLiteral("https://wxs.ign.fr/") +
    m_license.api_key() +
    QStringLiteral("/geoportail/ols?")
    ;

  qInfo() << url << xml_request;
  QNetworkReply * reply = post(url, xml_request);

  return QSharedPointer<QcLocationServiceReply>(new QcGeoportailLocationServiceReply(reply, query));
}

QSharedPointer<QcLocationServiceReverseReply>
QcGeoportailPlugin::reverse_geocode_request(const QcLocationServiceReverseQuery & query) const
{
  // Fixme: duplicated code

  QByteArray xml_request;
  QXmlStreamWriter stream(&xml_request);
  stream.setAutoFormatting(true);
  stream.setCodec("UTF-8");
  stream.writeStartDocument();
  stream.writeStartElement(QLatin1Literal("XLS"));
  QXmlStreamAttributes xls_attributes;
  xls_attributes.append(QLatin1Literal("xmlns:xls"), QLatin1Literal("http://www.opengis.net/xls"));
  xls_attributes.append(QLatin1Literal("xmlns:gml"), QLatin1Literal("http://www.opengis.net/gml"));
  xls_attributes.append(QLatin1Literal("xmlns"), QLatin1Literal("http://www.opengis.net/xls"));
  xls_attributes.append(QLatin1Literal("xmlns:xsi"), QLatin1Literal("http://www.w3.org/2001/XMLSchema-instance"));
  xls_attributes.append(QLatin1Literal("version"), QLatin1Literal("1.2"));
  xls_attributes.append(QLatin1Literal("xsi:schemaLocation"), QLatin1Literal("http://www.opengis.net/xls http://schemas.opengis.net/ols/1.2/olsAll.xsd"));
  stream.writeAttributes(xls_attributes);
  stream.writeEmptyElement(QLatin1Literal("RequestHeader"));
  QXmlStreamAttributes header_attributes;
  // header_attributes.append(QLatin1Literal("srsName"), query.srs_name());
  stream.writeStartElement(QLatin1Literal("Request"));
  QXmlStreamAttributes request_attributes;
  request_attributes.append(QLatin1Literal("maximumResponses"), QString::number(query.maximum_responses()));
  request_attributes.append(QLatin1Literal("requestID"), query.request_id());
  request_attributes.append(QLatin1Literal("version"), QLatin1Literal("1.2"));
  request_attributes.append(QLatin1Literal("methodName"), QLatin1Literal("ReverseGeocodeRequest"));
  stream.writeAttributes(request_attributes);

  stream.writeStartElement(QLatin1Literal("ReverseGeocodeRequest"));
  // stream.writeAttribute(QLatin1Literal("returnFreeForm"), QLatin1Literal("false"));
  stream.writeTextElement(QLatin1Literal("ReverseGeocodePreference"), QLatin1Literal("StreetAddress"));
  stream.writeStartElement(QLatin1Literal("Position"));
  stream.writeStartElement(QLatin1Literal("gml:Point"));
  const QcGeoCoordinateWGS84 & coordinate = query.coordinate();
  QString coordinate_string = QString::number(coordinate.latitude()) + ' ' + QString::number(coordinate.longitude());
  stream.writeTextElement(QLatin1Literal("gml:pos"), coordinate_string);
  stream.writeEndElement();
  if (query.radius()) {
    stream.writeStartElement(QLatin1Literal("gml:CircleByCenterPoint"));
    stream.writeTextElement(QLatin1Literal("gml:pos"), coordinate_string);
    stream.writeTextElement(QLatin1Literal("gml:radius"), QString::number(query.radius()));
  stream.writeEndElement();
  }
  stream.writeEndElement();
  stream.writeEndElement();

  stream.writeEndElement();
  stream.writeEndDocument();

  // http://wxs.ign.fr/<API_KEY>/geoportail/ols?
  QUrl url = QStringLiteral("https://wxs.ign.fr/") +
    m_license.api_key() +
    QStringLiteral("/geoportail/ols?")
    ;

  qInfo() << url << xml_request;
  QNetworkReply * reply = post(url, xml_request);

  return QSharedPointer<QcLocationServiceReverseReply>(new QcGeoportailLocationServiceReverseReply(reply, query));
}

QSharedPointer<QcElevationServiceReply>
QcGeoportailPlugin::coordinate_elevations(const QVector<QcGeoCoordinateWGS84> & coordinates) const
{
  QStringList longitudes;
  QStringList latitudes;
  for (const auto & coordinate : coordinates) {
    longitudes << QString::number(coordinate.longitude());
    latitudes  << QString::number(coordinate.latitude());
  }

  // https://wxs.ign.fr/<API_KEY>/alti/rest/elevation.json?lon=0.2367|2.1570|4.3907&lat=48.0551|46.6077|43.91
  // https://wxs.ign.fr/<API_KEY>/alti/rest/elevationLine.json?sampling=10&lon=0.2367|2.1570|4.3907&lat=48.0551|46.6077|43.91

  // ==========================================================
  // WARNING: USE GEOPORTAIL KEY FOR TEST PURPOSE ONLY
  // QString api_key = m_license.api_key();
  QString api_key = QStringLiteral("j5tcdln4ya4xggpdu4j0f0cn");
  // ==========================================================

  QUrl url = QStringLiteral("https://wxs.ign.fr/") +
    api_key +
    QStringLiteral("/alti/rest/elevation.json?") +
    QStringLiteral("lon=") + longitudes.join(QString('|')) +
    QStringLiteral("&lat=") + latitudes.join(QString('|'))
    ;
  // qInfo() << url;

  QNetworkReply * reply = get(url);

  return QSharedPointer<QcElevationServiceReply>(new QcGeoportailElevationServiceReply(reply, coordinates));
}

/**************************************************************************************************/

// #include "geoportail_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
