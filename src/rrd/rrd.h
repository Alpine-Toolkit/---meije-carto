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

// QC_BEGIN_NAMESPACE

/*
 * Header Format
 * B | byte    | version
 * H | short   | position
 * ? | boolean | empty
 * H | short   | header_size
 * H | short   | number_of_slots
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
  QcRoundRobinDatabase(const QString & path, size_t number_of_slots, const QString & slot_format);
  ~QcRoundRobinDatabase();

  int version() const { return m_version; }
  int number_of_slots() const { return m_number_of_slots; }
  int slot_size() const { return m_slot_size; }
  int position() const { return m_position; }

  void write(const QByteArray & data);
  void read(size_t position, QByteArray & data);
  void flush();

private:
  void open();
  void init(size_t number_of_slots, const QString & slot_format, bool allocate);
  void set_length();
  void clear();
  void read_header();
  void seek_to(size_t position);
  void update_position();

private:
  QFile m_file;
  QDataStream m_buffer;
  bool m_created;
  unsigned int m_version;
  size_t m_position;
  size_t m_header_size;
  size_t m_number_of_slots;
  size_t m_slot_size;
  bool m_empty;
  QString m_slot_format;
};

// QC_END_NAMESPACE

#endif /* __RRD_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
