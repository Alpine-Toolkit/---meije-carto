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

#ifndef __OSM_WMTS_TILE_FETCHER_H__
#define __OSM_WMTS_TILE_FETCHER_H__

/**************************************************************************************************/

#include "wmts/wmts_tile_fetcher.h"
// #include "map/osm/osm_plugin.h" // Fixme: circular

#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcOsmPlugin; // Fixme: circular

/**************************************************************************************************/

// Fixme: merge code

class QcOsmWmtsTileFetcher : public QcWmtsTileFetcher
{
  Q_OBJECT

public:
  QcOsmWmtsTileFetcher(const QcOsmPlugin * plugin);

  void set_user_agent(const QByteArray & user_agent) { m_user_agent = user_agent; }

private:
  QcWmtsReply * get_tile_image(const QcTileSpec & tile_spec);

private:
  const QcOsmPlugin * m_plugin;
  QNetworkAccessManager * m_network_manager;
  QByteArray m_user_agent;
  QString m_reply_format;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OSM_WMTS_TILE_FETCHER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
