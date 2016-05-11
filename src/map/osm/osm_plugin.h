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

#ifndef __OSM_PLUGIN_H__
#define __OSM_PLUGIN_H__

/**************************************************************************************************/

#include <QObject>
#include <QString>

#include "osm_wmts_tile_fetcher.h"
#include "map/wmts_plugin.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcOsmPlugin : public QcWmtsPlugin
{
public:
  QcOsmPlugin();
  ~QcOsmPlugin();

  QcOsmWmtsTileFetcher * tile_fetcher() {
    return &m_tile_fetcher;
  }

  // off-line cache : load tiles from a polygon

private:
  QcOsmWmtsTileFetcher m_tile_fetcher;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OSM_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
