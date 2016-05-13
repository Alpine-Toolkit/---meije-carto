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
#include <QUrl>

#include "wmts/tile_matrix_set.h"
#include "wmts/wmts_manager.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcWmtsPlugin;

/**************************************************************************************************/

class QcWmtsPluginLayer // : public QObject
{
  // Q_GADGET
  // Q_OBJECT
  // Q_PROPERTY(QString name READ name CONSTANT)
  // Q_PROPERTY(QString title READ title CONSTANT)

public:
  QcWmtsPluginLayer(QcWmtsPlugin * plugin,
                    int map_id,
                    int position,
                    const QString & title,
                    const QString & name,
                    const QString & image_format);
  QcWmtsPluginLayer(const QcWmtsPluginLayer & other);
  ~QcWmtsPluginLayer();

  QcWmtsPluginLayer & operator=(const QcWmtsPluginLayer & other);

  QcWmtsPlugin * plugin() const { return m_plugin; }
  const QString & plugin_name() const;
  int map_id() const { return m_map_id; }
  int position() const { return m_position; }
  const QString & title() const { return m_title; }
  const QString & name() const { return m_name; }
  const QString & image_format() const { return m_image_format; }

  QString hash_name() const;
  QcTileSpec create_tile_spec(int level, int x, int y) const;

  virtual QUrl url(const QcTileSpec & tile_spec) const = 0;

private:
  QcWmtsPlugin * m_plugin;
  int m_map_id;
  int m_position;
  QString m_title;
  QString m_name;
  QString m_image_format;
};

// Q_DECLARE_METATYPE(QcWmtsPluginLayer) // abstract

/**************************************************************************************************/

class QcWmtsPlugin // : public QObject
{
  // Q_GADGET
  // Q_OBJECT
  // Q_PROPERTY(QString name READ name CONSTANT)
  // Q_PROPERTY(QString title READ title CONSTANT)

  // Q_DISABLE_COPY(QcWmtsPlugin)

public:
  QcWmtsPlugin(const QString & name, const QString & title, size_t number_of_levels, size_t tile_size);
  ~QcWmtsPlugin();

  const QString & name() const { return m_name; }
  const QString & title() const { return m_title; }
  QcTileMatrixSet & tile_matrix_set() { return m_tile_matrix_set; } // Fixme: const ?
  QcWmtsManager * wmts_manager() { return &m_wmts_manager; }  // Fixme: & or *

  void add_layer(const QcWmtsPluginLayer * layer);
  const QList<const QcWmtsPluginLayer *> & layers() const { return m_layers; }
  // Fimxe: * vs & ?
  const QcWmtsPluginLayer * layer(int map_id) const;
  const QcWmtsPluginLayer * layer(const QcTileSpec & tile_spec) const;
  const QcWmtsPluginLayer * layer(const QString & title) const;

  // Fixme: protected ?
  QcTileSpec create_tile_spec(int map_id, int level, int x, int y) const {
    return QcTileSpec(m_name, map_id, level, x, y);
  }
  // Fixme: usefull ?
  QUrl make_layer_url(const QcTileSpec & tile_spec) const;

private:
  QString m_name;
  QString m_title;
  QList<const QcWmtsPluginLayer *> m_layers;
  QHash<int, const QcWmtsPluginLayer *> m_layer_map;
  QcTileMatrixSet m_tile_matrix_set;
  QcWmtsManager m_wmts_manager;
};

// Q_DECLARE_METATYPE(QcWmtsPlugin)

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
