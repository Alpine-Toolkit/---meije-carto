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

#ifndef __MAP_PATH_EDITOR_H__
#define __MAP_PATH_EDITOR_H__

/**************************************************************************************************/

#include "geometry/path.h"
#include "map/decorated_path.h"
#include "map/map_event_router.h"
#include "map/map_view.h"
#include "map/path_property.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapPathEditor : public QcMapEventClient
{
  Q_OBJECT
  Q_PROPERTY(QcPathProperty * path_property READ path_property_ptr CONSTANT)

public:
  QcMapPathEditor(QcMapView * map_view);
  ~QcMapPathEditor();

  // void handle_mouse_press_event(const QcMapEvent & event) override;
  void handle_mouse_move_event(const QcMapEvent & event) override;
  // void handle_mouse_release_event(const QcMapEvent & event) override;
  // void handle_mouse_wheel_event(const QcMapEvent & event) override;

  void handle_mouse_press_and_hold_event(const QcMapEvent & event) override;
  void handle_mouse_press_and_hold_released_event(const QcMapEvent & event) override;

  // void handle_clicked_event(const QcMapEvent & event) override;
  // void handle_double_clicked_event(const QcMapEvent & event) override;

  Q_INVOKABLE void clear();
  Q_INVOKABLE void set_closed(bool value);
  Q_INVOKABLE void set_vertex_edition_mode(bool value);

  QcDecoratedPathDouble path() const { return m_path; }
  void set_path(const QcPathDouble & path) { m_path = path ;}

  QcPathProperty path_property() const { return m_path_property; }

signals:
  void path_changed();

private:
  void update_path();
  QcPathProperty * path_property_ptr() { return &m_path_property; } // const

private:
  QcDecoratedPathDouble m_path;
  QcPathProperty m_path_property;
  QcMapView * m_map_view;
  bool m_vertex_edition_mode = false;
  int m_selected_vertex_index = -1;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

// Q_DECLARE_METATYPE(QcMapPathEditor)

/**************************************************************************************************/

#endif /* __MAP_PATH_EDITOR_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
