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

#include "map/map_view.h"

#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QcMapView::QcMapView(QcWmtsManager * wmts_manager)
  : QObject(),
    m_wmts_manager(wmts_manager),
    m_request_manager(nullptr)
{
  m_request_manager = new QcWmtsRequestManager(this, wmts_manager);
}

QcMapView::~QcMapView()
{
  if (m_request_manager)
    delete m_request_manager;
}

QcWmtsRequestManager *
QcMapView::request_manager()
{
  return m_request_manager;
}

void
QcMapView::update_tile(const QcTileSpec & tile_spec)
{
  QSharedPointer<QcTileTexture> texture = m_request_manager->tile_texture(tile_spec);
  if (!texture.isNull()) {
    qInfo() << "QcMapView::update_tile" << tile_spec;
  }
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
