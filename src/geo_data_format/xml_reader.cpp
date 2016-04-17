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

#include "geo_data_format/xml_reader.h"

#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

bool
QcXmlStreamReader::read_match_start_element(const QString & _name)
{
  return (readNext() == QXmlStreamReader::StartElement
          and name() == _name);
}

bool
QcXmlStreamReader::read_match_end_element(const QString & _name)
{
  return (readNext() == QXmlStreamReader::EndElement
          and name() == _name);
}

bool
QcXmlStreamReader::match_start_element(const QString & _name)
{
  return (tokenType() == QXmlStreamReader::StartElement
          and name() == _name);
}

bool
QcXmlStreamReader::match_end_element(const QString & _name)
{
  return (tokenType() == QXmlStreamReader::EndElement
          and name() == _name);
}

bool
QcXmlStreamReader::read_match_empty()
{
  return (readNext() == QXmlStreamReader::Characters
          and isWhitespace());
}

bool
QcXmlStreamReader::match_empty()
{
  return (tokenType() == QXmlStreamReader::Characters
          and isWhitespace());
}

void
QcXmlStreamReader::read_until_start_of(const QString & _name)
{
  while (! (readNext() == QXmlStreamReader::StartElement
            and name() == _name))
    {}
}

void
QcXmlStreamReader::read_until_end_of(const QString & _name)
{
  while (! (readNext() == QXmlStreamReader::EndElement
            and name() == _name))
    {}
}

void
QcXmlStreamReader::read_until_empty()
{
  while (readNext() == QXmlStreamReader::Characters
         and isWhitespace())
    {}
}

QDateTime
QcXmlStreamReader::read_date()
{
  QString s = readElementText();
  return QDateTime::fromString(s, Qt::ISODate);
}

int
QcXmlStreamReader::read_int()
{
  return readElementText().toInt();
}

double
QcXmlStreamReader::read_double()
{
  return readElementText().toDouble();
}

QString
QcXmlStreamReader::get_attribute(QString attribute)
{
  return attributes().value(attribute).toString();
}

int
QcXmlStreamReader::get_int_attribute(QString attribute)
{
  return get_attribute(attribute).toInt();
}

double
QcXmlStreamReader::get_double_attribute(QString attribute)
{
  return get_attribute(attribute).toDouble();
}


/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
