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

#include "plugin_data.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcWmtsPluginData::QcWmtsPluginData()
  : m_name(),
    m_title()
{}

QcWmtsPluginData::QcWmtsPluginData(const QString & name, const QString & title)
  : m_name(name),
    m_title(title)
{}

QcWmtsPluginData::QcWmtsPluginData(const QcWmtsPluginData & other)
  : m_name(other.m_name),
    m_title(other.m_title)
{}

QcWmtsPluginData &
QcWmtsPluginData::operator=(const QcWmtsPluginData & other)
{
  if (this != &other) {
    m_name = other.m_name;
    m_title = other.m_title;
  }

  return *this;
}

/**************************************************************************************************/

QcWmtsPluginLayerData::QcWmtsPluginLayerData()
  : QObject(),
    m_plugin_layer(nullptr),
    m_status(false),
    m_opacity(1.)
{}

QcWmtsPluginLayerData::QcWmtsPluginLayerData(const QcWmtsPluginLayer * plugin_layer)
  : QObject(),
    m_plugin_layer(plugin_layer),
    m_status(false),
    m_opacity(1.)
{}

QcWmtsPluginLayerData::QcWmtsPluginLayerData(const QcWmtsPluginLayerData & other)
  : QObject(),
    m_plugin_layer(other.m_plugin_layer),
    m_status(other.m_status),
    m_opacity(other.m_opacity)
{}

QcWmtsPluginLayerData &
QcWmtsPluginLayerData::operator=(const QcWmtsPluginLayerData & other)
{
  if (this != &other) {
    m_plugin_layer = other.m_plugin_layer;
    m_status = other.m_status;
    m_opacity = other.m_opacity;
  }

  return *this;
}

void
QcWmtsPluginLayerData::set_status(bool status)
{
  if (m_status != status) {
    m_status = status;
    emit statusChanged(status);
  }
}

void
QcWmtsPluginLayerData::set_opacity(float opacity)
{
  if (m_opacity != opacity) {
    m_opacity = opacity;
    emit opacityChanged(opacity);
  }
}

/**************************************************************************************************/

// #include "plugin_data.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
