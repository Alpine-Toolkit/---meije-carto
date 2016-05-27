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

#ifndef __WMTS_NETWORK_TILE_FETCHER_H__
#define __WMTS_NETWORK_TILE_FETCHER_H__

/**************************************************************************************************/

#include "wmts/wmts_tile_fetcher.h"

#include <QNetworkAccessManager>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcWmtsPlugin; // circular

/**************************************************************************************************/

// Fixme: QcWmtsTileFetcher isn't networking aware, excepted QcWmtsReply relies on QNetworkReply

class QcWmtsNetworkTileFetcher : public QcWmtsTileFetcher
{
  Q_OBJECT

public:
  QcWmtsNetworkTileFetcher(QcWmtsPlugin * plugin);
  ~QcWmtsNetworkTileFetcher();

private:
  QcWmtsReply * get_tile_image(const QcTileSpec & tile_spec);

private:
  QcWmtsPlugin * m_plugin;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_NETWORK_TILE_FETCHER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
