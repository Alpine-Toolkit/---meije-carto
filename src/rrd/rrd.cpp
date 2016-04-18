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

#include "rrd.h"

#include <QtDebug>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

constexpr quint8 VERSION = 1;
constexpr size_t POSITION_SEEK = 1;
QLatin1Literal HEADER_FORMAT("BH?HHHB");

size_t
QcRoundRobinDatabase::compute_slot_size(const QString & slot_format)
{
  size_t slot_size = 0;
  for(const QChar c : slot_format)
    switch (c.toLatin1()) {
    case '?':
      slot_size += 1;
      break;
    case 'b':
    case 'B':
      slot_size += 1;
      break;
    case 'h':
    case 'H':
      slot_size += 2;
      break;
    case 'i':
    case 'I':
      slot_size += 4;
      break;
    case 'q':
    case 'Q':
      slot_size += 8;
      break;
    case 'f':
      slot_size += 4;
      break;
    case 'd':
      slot_size += 8;
      break;
      // default; // fixme:
    }
  return slot_size;
}

QcRoundRobinDatabase::QcRoundRobinDatabase(const QString & path)
  : m_file(path),
    m_buffer(),
    m_created(),
    m_version(),
    m_position(),
    m_header_size(),
    m_number_of_slots(),
    m_slot_size(),
    m_empty(),
    m_slot_format()
{
  open();
  if (!m_created) {
    read_header();
    seek_to(m_position); // go to next position
  } else {
    // ...
  }
}

QcRoundRobinDatabase::QcRoundRobinDatabase(const QString & path, size_t number_of_slots, const QString & slot_format)
  : m_file(path),
    m_buffer(),
    m_created(),
    m_version(),
    m_position(),
    m_header_size(),
    m_number_of_slots(),
    m_slot_size(),
    m_empty(),
    m_slot_format()
{
  if (number_of_slots >= 1<<16)
    qCritical() << "QcRoundRobinDatabase" << "Wrong number of slots";
  // Fixme:

  open();
  if (m_created) {
    init(number_of_slots, slot_format, true);
  } else {
    init(number_of_slots, slot_format, true);
    // ...
    // read_header();
    // if (number_of_slots != m_number_of_slots || slot_size != m_slot_size) {
    //   init(number_of_slots, slot_size, true)
  }
}

QcRoundRobinDatabase::~QcRoundRobinDatabase()
{}

void
QcRoundRobinDatabase::open()
{
  m_created = !m_file.exists();
  if (! m_file.open(QIODevice::ReadWrite))
    ; // Fixme:
  m_buffer.setDevice(&m_file);
}

void
QcRoundRobinDatabase::init(size_t number_of_slots, const QString & slot_format, bool allocate)
{
  size_t header_size = compute_slot_size(HEADER_FORMAT) + slot_format.length();
  // Round size to 8-byte
  m_header_size = (header_size/8)*8;
  if (m_header_size < header_size)
    m_header_size += 8;
  qInfo() << "QcRoundRobinDatabase::init" << header_size << m_header_size;

  m_number_of_slots = number_of_slots;
  m_slot_size = compute_slot_size(slot_format);
  m_position = 0;
  m_empty = true;
  m_slot_format = slot_format;

  size_t slot_format_length = slot_format.length();
  m_buffer << static_cast<quint8>(VERSION)
           << static_cast<quint16>(m_position)
           << static_cast<quint8>(m_empty)
           << static_cast<quint16>(m_header_size)
           << static_cast<quint16>(m_number_of_slots)
           << static_cast<quint16>(m_slot_size)
           << static_cast<quint8>(slot_format_length);
  m_buffer.writeRawData(slot_format.toStdString().c_str(), slot_format_length); // don't include terminal \0
  // clear padding
  for (size_t i = 0; i < (m_header_size - header_size); i++)
    m_buffer << static_cast<quint8>(0);

  // if (allocate)
  //   set_length();
  clear();

  // seek_to(0);
  // m_buffer << static_cast<quint8>('Z');
  // seek_to(1);
  // m_buffer << static_cast<quint8>('a') << static_cast<quint8>('b');
  // seek_to(2);
  // m_buffer << static_cast<quint8>('e') << static_cast<quint8>('f');
}

void
QcRoundRobinDatabase::set_length()
{
  if (! m_file.resize(m_header_size + m_number_of_slots * m_slot_size))
    ; // Fixme
}

void
QcRoundRobinDatabase::clear()
{
  seek_to(0);
  for (size_t i = 0; i < m_number_of_slots * m_slot_size; i++)
    m_buffer << static_cast<quint8>(0);
}

void
QcRoundRobinDatabase::read_header()
{
  quint8 version, empty, slot_format_length;
  quint16 position, header_size, number_of_slots, slot_size;
  m_file.seek(0);
  m_buffer >> version
           >> position
           >> empty
           >> header_size
           >> number_of_slots
           >> slot_size
           >> slot_format_length;
  char slot_format[slot_format_length +1];
  slot_format[slot_format_length] = 0;
  m_buffer.readRawData(slot_format, slot_format_length);
  m_version = version;
  m_position = position;
  m_empty = empty;
  if (m_empty)
    m_position = 0; // implicit
  m_header_size = header_size;
  m_number_of_slots = number_of_slots;
  m_slot_size = slot_size;
  m_slot_format = slot_format;
  qInfo() << "QcRoundRobinDatabase::read_header"
          << QStringLiteral("Header: v%1 %2/%3 @%4").arg(m_version).arg(m_number_of_slots).arg(m_slot_size).arg(m_position) << slot_format_length << m_slot_format;
}

void
QcRoundRobinDatabase::seek_to(size_t position)
{
  size_t device_position = m_header_size + position * m_slot_size;
  qInfo() << "QcRoundRobinDatabase::seek_to" << device_position;
  if (! m_file.seek(device_position))
    ; // Fixme:
}

void
QcRoundRobinDatabase::write(const QByteArray & data)
{
  if (static_cast<size_t>(data.size()) == m_slot_size) { // Fixme: use int ?
    seek_to(m_position);
    qInfo() << "QcRoundRobinDatabase::write @" << m_position; // << data;
     m_buffer.writeRawData(data.data(), m_slot_size);
    m_empty = false;
    m_position++;
    if (m_position == m_number_of_slots)
      m_position = 0; // loop
    update_position();
  }
  // else
}

void
QcRoundRobinDatabase::read(size_t position, QByteArray & data)
{
  if (static_cast<size_t>(data.size()) == m_slot_size) { // Fixme: use int ?
    seek_to(position);
    m_buffer.readRawData(data.data(), m_slot_size);
    // qInfo() << "QcRoundRobinDatabase::read" << data;
  }
  // else
}

void
QcRoundRobinDatabase::update_position()
{
  m_file.seek(POSITION_SEEK);
  m_buffer << static_cast<quint16>(m_position) << static_cast<quint8>(m_empty);
  // seek_to(m_position);
}

void
QcRoundRobinDatabase::flush()
{
  m_file.flush();
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
