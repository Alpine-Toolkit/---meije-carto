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
QcMapPathEditor::set_vertex_edition_mode(bool value)
{
  if (m_vertex_edition_mode and !value and m_selected_vertex_index != -1) {
    m_path.set_attribute_at(m_selected_vertex_index, QcDecoratedPathDouble::AttributeType::Normal);
    update_path();
  }
  m_vertex_edition_mode = value;
  m_selected_vertex_index = -1;
}

void
QcMapPathEditor::update_path()
{
  m_path_property.set_length(m_path.length());
  // Fixme: QcPolygon
  // m_path_property.set_area(m_path.closed() ? m_path.area() : .0);
  m_map_view->update_path(&m_path);
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
    int vertex_index = m_path.nearest_vertex_index(position, distance);
    double distance_px = m_map_view->viewport()->to_px(distance);
    constexpr double distance_px_max = 20; // Fixme: hidpi
    bool on_point = distance_px <= distance_px_max;
    bool selected_point = m_selected_vertex_index != -1;
    qInfo() << "closest point" << position << m_path.vertex_at(vertex_index) << distance << distance_px << on_point << selected_point;
    if (on_point) {
      if (selected_point)
        m_path.set_attribute_at(m_selected_vertex_index, QcDecoratedPathDouble::AttributeType::Normal);
      if (vertex_index != m_selected_vertex_index) {
        m_path.set_attribute_at(vertex_index, QcDecoratedPathDouble::AttributeType::Selected | QcDecoratedPathDouble::AttributeType::Touched);
        m_selected_vertex_index = vertex_index;
      } else
        m_selected_vertex_index = -1;
      update_path();
    } else if (selected_point) {
      qInfo() << "update point" << m_selected_vertex_index;
      QcDecoratedPathDouble::VertexListType vertexes = m_path.vertexes();
      vertexes[m_selected_vertex_index] = position;
      m_path = QcDecoratedPathDouble(vertexes, m_path.closed());
      m_path.set_attribute_at(m_selected_vertex_index, QcDecoratedPathDouble::AttributeType::Selected | QcDecoratedPathDouble::AttributeType::Touched);
      update_path();
    }
  } else {
    // Fixme: store wgs coordinate
    QcVectorDouble position = event.projected_coordinate();
    m_path.add_vertex(position);
    m_path.set_attribute_at(m_path.last_vertex_index(), QcDecoratedPathDouble::AttributeType::Touched);
    update_path();
  }
}

void
QcMapPathEditor::handle_mouse_press_and_hold_released_event(const QcMapEvent & event)
{
  qInfo() << event;
  if (m_selected_vertex_index != -1)
    m_path.set_attribute_at(m_selected_vertex_index, QcDecoratedPathDouble::AttributeType::Selected);
  else
    m_path.set_attribute_at(m_path.last_vertex_index(), QcDecoratedPathDouble::AttributeType::Normal);
  update_path();
}

void
QcMapPathEditor::clear()
{
  m_path.clear();
  // Fixme: coherent with QML, duplicated
  m_selected_vertex_index = -1;
  m_vertex_edition_mode = false;
  update_path();
}

void
QcMapPathEditor::set_closed(bool value)
{
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
