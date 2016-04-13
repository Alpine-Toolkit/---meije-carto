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

#ifndef __GEOPORTAIL_PLUGIN_H__
#define __GEOPORTAIL_PLUGIN_H__

/**************************************************************************************************/

#include <QObject>
#include <QString>

#include "geoportail_wmts_tile_fetcher.h"
#include "map/tile_matrix_set.h"
#include "map/wmts_manager.h"

// #include "map/earth.h"
// #include "map/viewport.h"
// #include "map/mosaic_painter.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcWmtsPlugin : public QObject
{
  Q_OBJECT

public:
  QcWmtsPlugin(const QString & name, size_t number_of_levels, size_t tile_size);
  ~QcWmtsPlugin();

  QcWmtsManager * wmts_manager() {
    return &m_wmts_manager;
  }

  QcTileSpec create_tile_spec(int map_id, int level, int x, int y) const {
    return QcTileSpec(m_name, 1, 16, 33885, 23658);
  }

private:
  QString m_name;
  QcTileMatrixSet m_tile_matrix_set;
  QcWmtsManager m_wmts_manager;
};

/**************************************************************************************************/

class QcGeoportailPlugin : public QcWmtsPlugin
{
  Q_OBJECT

public:
  QcGeoportailPlugin(QcGeoPortailWmtsLicence & licence);
  ~QcGeoportailPlugin();

  const QcGeoPortailWmtsLicence & licence() {
    return m_licence;
  }

  QcGeoportailWmtsTileFetcher * tile_fetcher() {
    return &m_tile_fetcher;
  }

  // off-line cache : load tiles from a polygon

private:
  QcGeoPortailWmtsLicence m_licence;
  QcGeoportailWmtsTileFetcher m_tile_fetcher;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
