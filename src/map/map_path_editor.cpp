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

#include "map_path_editor.h"

#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcMapPathEditor::QcMapPathEditor(QcMapView * map_view)
  : QcMapEventClient(QLatin1Literal("path-editor")),
    m_map_view(map_view)
{}

QcMapPathEditor::~QcMapPathEditor()
{}

void
QcMapPathEditor::update_path()
{
  m_path_property.set_length(m_path.length());
  // Fixme: QcPolygon
  // m_path_property.set_area(m_path.closed() ? m_path.area() : .0);
  m_map_view->update_path(m_path);
  emit path_changed();
}

void
QcMapPathEditor::handle_mouse_move_event(const QcMapEvent & event)
{
  qInfo() << event;
}

void
QcMapPathEditor::handle_mouse_press_and_hold_event(const QcMapEvent & event)
{
  if (m_vertex_edition_mode) {
    QcVectorDouble position = event.projected_coordinate();
    double distance;
    QcVectorDouble vertex = m_path.nearest_vertex(position, distance);
    qInfo() << "closest point" << position << vertex << distance;
  } else {
    // Fixme: store wgs coordinate
    QcVectorDouble position = event.projected_coordinate();
    m_path.add_vertex(position);
    qInfo() << event << position << m_path.number_of_edges();
    update_path();
  }
}

void
QcMapPathEditor::clear()
{
  qInfo();
  m_path.clear();
  update_path();
}

void
QcMapPathEditor::set_closed(bool value)
{
  qInfo();
  m_path.set_closed(value);
  update_path();
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
