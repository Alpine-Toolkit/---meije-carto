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

#include "wmts/wmts_plugin.h"
#include "wmts/plugin_wmts_tile_fetcher.h"

#include <QString>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcOsmPlugin;

/**************************************************************************************************/

class QcOsmLayer : public QcWmtsPluginLayer
{
public:
  QcOsmLayer(QcOsmPlugin * plugin,
             int map_id,
             int position,
             const QString & title,
             const QString & name,
             const QString & image_format,
             const QString & base);
  QcOsmLayer(const QcOsmLayer & other);
  ~QcOsmLayer();

  QcOsmLayer & operator=(const QcOsmLayer & other);

  const QString & base() const { return m_base; }

  QUrl url(const QcTileSpec & tile_spec) const;

private:
  QString m_base;
};

/**************************************************************************************************/

class QcOsmPlugin : public QcWmtsPlugin
{
  Q_OBJECT

public:
  static const QString PLUGIN_NAME;

public:
  QcOsmPlugin();
  ~QcOsmPlugin();

  QcPluginWmtsTileFetcher * tile_fetcher() { return &m_tile_fetcher; }

  // off-line cache : load tiles from a polygon

private:
  QcPluginWmtsTileFetcher m_tile_fetcher;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OSM_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
