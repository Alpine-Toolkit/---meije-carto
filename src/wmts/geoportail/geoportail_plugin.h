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
#include <QList>
#include <QHash>

#include "wmts/geoportail/geoportail_license.h"
#include "wmts/geoportail/geoportail_wmts_tile_fetcher.h" // Fixme: circular
#include "wmts/wmts_plugin.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcGeoportailLayer
{
public:
  QcGeoportailLayer();
  QcGeoportailLayer(int map_id,
                    int position,
                    const QString & title,
                    const QString & name,
                    const QString & style,
                    const QString & image_format);
  QcGeoportailLayer(const QcGeoportailLayer & other);
  ~QcGeoportailLayer();

  QcGeoportailLayer & operator=(const QcGeoportailLayer & other);

  int map_id() const { return m_map_id; }
  int position() const { return m_position; }
  const QString & title() const { return m_title; }
  const QString & name() const { return m_name; }
  const QString & style() const { return m_style; }
  const QString & image_format() const { return m_image_format; }

  QUrl url(const QcTileSpec & tile_spec, const QcGeoportailWmtsLicense & license) const;

private:
  int m_map_id;
  int m_position;
  QString m_title;
  QString m_name;
  QString m_style;
  QString m_image_format;
};

/**************************************************************************************************/

class QcGeoportailPlugin : public QcWmtsPlugin
{
public:
  QcGeoportailPlugin(const QcGeoportailWmtsLicense & license);
  ~QcGeoportailPlugin();

  const QcGeoportailWmtsLicense & license() const { return m_license; }

  QcGeoportailWmtsTileFetcher * tile_fetcher() { return &m_tile_fetcher; }

  // off-line cache : load tiles from a polygon

  int number_of_layers() const { return m_layers.size(); }
  QcGeoportailLayer layer(int i) const { return m_layers[i]; } // Fixme: const &
  QcGeoportailLayer layer(const QString & title) const;

  QUrl make_layer_url(const QcTileSpec & tile_spec) const;

private:
  void add_layer(const QcGeoportailLayer & layer);

private:
  QcGeoportailWmtsLicense m_license;
  QcGeoportailWmtsTileFetcher m_tile_fetcher;
  QHash<int, QcGeoportailLayer> m_layers;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
