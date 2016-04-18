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

#ifndef __RRD_H__
#define __RRD_H__

/**************************************************************************************************/

#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QString>

/**************************************************************************************************/

class QcRoundRobinDatabaseIter
{
public:
  QcRoundRobinDatabaseIter(size_t number_of_reserved_slots, size_t oldest_position, size_t position)
    : m_number_of_reserved_slots(number_of_reserved_slots),
      m_linear_count(position),
      m_position(position)
  {
    if (m_number_of_reserved_slots) {
      if (position >= oldest_position)
        m_linear_count = position - oldest_position;
      else
        m_linear_count = position + m_number_of_reserved_slots - oldest_position;
    }
  }

  bool operator!=(const QcRoundRobinDatabaseIter & other) const {
    return m_linear_count != other.m_linear_count;
  }

  size_t operator*() const { return m_position; };

  const QcRoundRobinDatabaseIter & operator++() {
    m_linear_count++;
    m_position++;
    if (m_number_of_reserved_slots && m_position == m_number_of_reserved_slots)
      m_position = 0; // loop
    return *this;
  }

private:
  size_t m_number_of_reserved_slots;
  size_t m_linear_count;
  size_t m_position;
};

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/*
 * Header Format
 * B | byte    | version
 * H | short   | position
 * H | short   | number_of_used_slots
 * H | short   | header_size
 * H | short   | number_of_reserved_slots
 * H | short   | slot_size
 * B | byte    | slot_format length 256
 *
 * Slot schema:
 * | boolean | 1 | ?
 * | byte    | 1 | b B (upper for unsigned)
 * | short   | 2 | h H
 * | int     | 4 | i I
 * | long    | 8 | q Q
 * | float   | 4 | f
 * | double  | 8 | d
 *
 */

class QcRoundRobinDatabase
{
public:
  static size_t compute_slot_size(const QString & slot_format);

  QcRoundRobinDatabase(const QString & path);
  QcRoundRobinDatabase(const QString & path, const QString & slot_format, size_t number_of_reserved_slots = 0);
  ~QcRoundRobinDatabase();

  bool is_round_robin() const { return m_number_of_reserved_slots > 0; }
  bool is_unlimited() const { return m_number_of_reserved_slots == 0; }
  bool is_empty() const { return m_number_of_used_slots == 0; }
  bool is_full() const { return is_round_robin() && m_number_of_used_slots == m_number_of_reserved_slots; }

  unsigned int version() const { return m_version; }
  size_t number_of_reserved_slots() const { return m_number_of_reserved_slots; }
  size_t number_of_used_slots() const { return m_number_of_used_slots; }
  size_t slot_size() const { return m_slot_size; }
  size_t position() const { return m_position; } // position for the next writing
  size_t latest_position() const;
  size_t oldest_position() const;

  size_t position_to_linear_position(size_t position) const;
  size_t linear_position_to_position(size_t linear_position) const;

  QcRoundRobinDatabaseIter begin() const {
    size_t _oldest_position = oldest_position();
    return QcRoundRobinDatabaseIter(m_number_of_reserved_slots, _oldest_position, _oldest_position);
  }
  QcRoundRobinDatabaseIter end() const {
    return QcRoundRobinDatabaseIter(m_number_of_reserved_slots, oldest_position(), latest_position()); // Fixme: miss last
  }

  void write(const QByteArray & data);
  void read(size_t position, QByteArray & data);
  void flush();

private:
  void open();
  void init(bool allocate);
  void set_length();
  void clear();
  void read_header();
  void seek_to(size_t position);
  void update_position();

private:
  QFile m_file;
  QDataStream m_buffer;
  QString m_slot_format;
  bool m_created;
  unsigned int m_version;
  size_t m_position;
  size_t m_number_of_reserved_slots;
  size_t m_number_of_used_slots;
  size_t m_header_size;
  size_t m_slot_size;
};

// QC_END_NAMESPACE

#endif /* __RRD_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
