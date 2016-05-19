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
#include "map/map_event_router.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapPathEditor : public QcMapEventClient
{
  Q_OBJECT

public:
  QcMapPathEditor();
  ~QcMapPathEditor();

  // void handle_mouse_press_event(const QcMapEvent & event) override;
  void handle_mouse_move_event(const QcMapEvent & event) override;
  // void handle_mouse_release_event(const QcMapEvent & event) override;
  // void handle_mouse_wheel_event(const QcMapEvent & event) override;

  void handle_mouse_press_and_hold_event(const QcMapEvent & event) override;

  // void handle_clicked_event(const QcMapEvent & event) override;
  // void handle_double_clicked_event(const QcMapEvent & event) override;

private:
  QcPath m_path;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __MAP_PATH_EDITOR_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
