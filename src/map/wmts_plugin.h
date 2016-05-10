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

#ifndef __WMTS_PLUGIN_H__
#define __WMTS_PLUGIN_H__

/**************************************************************************************************/

#include <QObject>
#include <QString>

#include "map/tile_matrix_set.h"
#include "map/wmts_manager.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcWmtsPlugin : public QObject
{
  Q_OBJECT

public:
  QcWmtsPlugin(const QString & name, size_t number_of_levels, size_t tile_size);
  ~QcWmtsPlugin();

  const QString & name() { return m_name; }
  QcTileMatrixSet & tile_matrix_set() { return m_tile_matrix_set; }
  QcWmtsManager * wmts_manager() { return &m_wmts_manager; }  // Fixme: & or *

  QcTileSpec create_tile_spec(int map_id, int level, int x, int y) const {
    return QcTileSpec(m_name, map_id, level, x, y);
  }

private:
  QString m_name;
  QcTileMatrixSet m_tile_matrix_set;
  QcWmtsManager m_wmts_manager;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
