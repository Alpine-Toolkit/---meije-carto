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

#include "map_event_router.h"

#include<QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcMapEvent::QcMapEvent()
  : m_button(),
    m_buttons(),
    m_modifiers(),
    m_was_held(),
    m_coordinate()
{}

QcMapEvent::QcMapEvent(int button, int buttons, int modifiers, int was_held,
                       const QcGeoCoordinateWGS84 & coordinate)
  : m_button(button),
    m_buttons(buttons),
    m_modifiers(modifiers),
    m_was_held(was_held),
    m_coordinate(coordinate)
{}

QcMapEvent::QcMapEvent(const QcMapEvent & other)
  : m_button(other.m_button),
    m_buttons(other.m_buttons),
    m_modifiers(other.m_modifiers),
    m_was_held(other.m_was_held),
    m_coordinate(other.m_coordinate)
{}

QcMapEvent::~QcMapEvent()
{}

QcMapEvent &
QcMapEvent::operator=(const QcMapEvent & other)
{
  if (this != &other) {
    m_button = other.m_button;
    m_buttons = other.m_buttons;
    m_modifiers = other.m_modifiers;
    m_was_held = other.m_was_held;
    m_coordinate = other.m_coordinate;
  }

  return *this;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug
operator<<(QDebug debug, const QcMapEvent & map_event)
{
    QDebugStateSaver saver(debug);

    debug.nospace() << "QcMapEvent(";
    debug << map_event.button();
    debug << ',';
    debug << map_event.buttons();
    debug << ',';
    debug << map_event.modifiers();
    debug << ',';
    debug << map_event.was_held();
    debug << ',';
    debug << map_event.coordinate();
    debug << ')';

    return debug;
}
#endif

/**************************************************************************************************/

QcMapEventClient::QcMapEventClient(const QString & name)
  : QObject(),
    m_name(name)
{}

QcMapEventClient::~QcMapEventClient()
{}

/**************************************************************************************************/

QcMapEventRouter::QcMapEventRouter()
  : QObject()
{
}

QcMapEventRouter::~QcMapEventRouter()
{
}

QcMapEvent
QcMapEventRouter::create_map_event(int button, int buttons, int modifiers, int was_held,
                                   const QcGeoCoordinateWGS84 & coordinate)
{
  return QcMapEvent(button, buttons, modifiers, was_held, coordinate);
}

void
QcMapEventRouter::handle_mouse_press_event(const QcMapEvent & event)
{
  if (!m_current_client)
    m_current_client->handle_mouse_press_event(event);
}

void
QcMapEventRouter::handle_mouse_move_event(const QcMapEvent & event)
{
  if (!m_current_client)
    m_current_client->handle_mouse_move_event(event);
}

void
QcMapEventRouter::handle_mouse_release_event(const QcMapEvent & event)
{
  if (!m_current_client)
    m_current_client->handle_mouse_release_event(event);
}

void
QcMapEventRouter::handle_mouse_wheel_event(const QcMapEvent & event)
{
  if (!m_current_client)
    m_current_client->handle_mouse_wheel_event(event);
}

void
QcMapEventRouter::handle_mouse_press_and_hold_event(const QcMapEvent & event)
{
  qInfo() << event;
  if (!m_current_client.isNull())
    m_current_client->handle_mouse_press_and_hold_event(event);
}

void
QcMapEventRouter::handle_clicked_event(const QcMapEvent & event)
{
  if (!m_current_client)
    m_current_client->handle_clicked_event(event);
}

void
QcMapEventRouter::handle_double_clicked_event(const QcMapEvent & event)
{
  if (!m_current_client)
    m_current_client->handle_double_clicked_event(event);
}

void
QcMapEventRouter::register_client(ClientSharedPointer client)
{
  m_clients.insert(client->name(), client);
}

void
QcMapEventRouter::unregister_client(const QString & name)
{
  // Fixme: in stack
  m_clients.remove(name);
}

void
QcMapEventRouter::push_client(const QString & name)
{
  if (m_clients.contains(name)) {
    qInfo() << "push" << name;
    m_current_client = m_clients.value(name);
    // m_current_client.data()
    // m_client_stack.insert());
  }
  // Fixme: else
}

void
QcMapEventRouter::pop_client()
{
  if (m_client_stack.size()) {
    m_client_stack.removeLast();
    // qInfo() << "pop" << name;
    if (m_client_stack.size())
      m_current_client = m_client_stack.last();
    else
      m_current_client = ClientSharedPointer(nullptr);
  }
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
