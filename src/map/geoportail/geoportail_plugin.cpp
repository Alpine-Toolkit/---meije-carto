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

#include "geoportail_plugin.h"

/**************************************************************************************************/

constexpr const char * PLUGIN_NAME = "geoportail";
constexpr size_t NUMBER_OF_LEVELS = 20;
constexpr size_t TILE_SIZE = 256;

QcGeoportailPlugin::QcGeoportailPlugin(QcGeoportailWmtsLicence & licence)
  : QcWmtsPlugin(PLUGIN_NAME, NUMBER_OF_LEVELS, TILE_SIZE),
    m_licence(licence),
    m_tile_fetcher(m_licence)
{
  wmts_manager()->set_tile_fetcher(&m_tile_fetcher);
  wmts_manager()->tile_cache(); // create a file tile cache

  // wmts_manager()->tile_cache()->clear_all();
}

QcGeoportailPlugin::~QcGeoportailPlugin()
{}

/**************************************************************************************************/

// #include "geoportail_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
