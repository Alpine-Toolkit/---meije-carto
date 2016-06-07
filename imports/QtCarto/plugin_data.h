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

#ifndef __PLUGIN_DATA_H__
#define __PLUGIN_DATA_H__

/**************************************************************************************************/

#include "wmts/wmts_plugin_manager.h"

#include <QString>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcWmtsPluginData
{
  Q_GADGET
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString title READ title CONSTANT)

public:
  QcWmtsPluginData();
  QcWmtsPluginData(const QString & name, const QString & title);
  QcWmtsPluginData(const QcWmtsPluginData & other);

  QcWmtsPluginData & operator=(const QcWmtsPluginData & other);

  const QString & name() const { return m_name; }
  const QString & title() const { return m_title; }

private:
  QString m_name;
  QString m_title;
};

// Q_DECLARE_METATYPE(QcWmtsPluginData)

/**************************************************************************************************/

class QcWmtsPluginLayerData : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString plugin READ plugin CONSTANT)
  Q_PROPERTY(QString title READ title CONSTANT)
  Q_PROPERTY(QString projection READ projection CONSTANT)
  Q_PROPERTY(bool status READ status WRITE set_status NOTIFY statusChanged)
  Q_PROPERTY(float opacity READ opacity WRITE set_opacity NOTIFY opacityChanged)

public:
  QcWmtsPluginLayerData();
  QcWmtsPluginLayerData(const QcWmtsPluginLayer * plugin_layer);
  QcWmtsPluginLayerData(const QcWmtsPluginLayerData & other);

  QcWmtsPluginLayerData & operator=(const QcWmtsPluginLayerData & other);

  const QcWmtsPluginLayer * plugin_layer() const { return m_plugin_layer; }
  QString plugin() const { return m_plugin_layer->plugin_name(); }
  QString title() const { return m_plugin_layer->title(); }
  QString projection() const { return m_plugin_layer->projection_name(); }

  bool status() const { return m_status; }
  float opacity() const { return m_opacity; }

  void set_status(bool status);
  void set_opacity(float opacity);

signals:
  void statusChanged(bool status);
  void opacityChanged(float opacity);

private:
  const QcWmtsPluginLayer * m_plugin_layer;
  bool m_status;
  float m_opacity;
};

Q_DECLARE_METATYPE(QcWmtsPluginLayerData)

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif // __PLUGIN_DATA_H__

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
