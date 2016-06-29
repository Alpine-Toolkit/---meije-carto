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

#include "logger.h"

#include <QDateTime>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// COLOR_SEQ = "\033[1;%dm"
// BOLD_SEQ = "\033[1m"

const QString ANSI_RESET = "\e[0m";

const QString ANSI_BOLD       = "\e[1m";
const QString ANSI_DIM        = "\e[2m";
const QString ANSI_UNDERLINED = "\e[4m";
const QString ANSI_BLINK      = "\e[5m";
const QString ANSI_INVERTED   = "\e[7m";
const QString ANSI_HIDDEN     = "\e[8m";

const QString ANSI_RESET_BOLD       = "\e[21m";
const QString ANSI_RESET_DIM        = "\e[22m";
const QString ANSI_RESET_UNDERLINED = "\e[24m";
const QString ANSI_RESET_BLINK      = "\e[25m";
const QString ANSI_RESET_INVERTED   = "\e[27m";
const QString ANSI_RESET_HIDDEN     = "\e[28m";

const QString ANSI_DEFAULT       = "\e[39m";
const QString ANSI_BLACK         = "\e[30m";
const QString ANSI_RED           = "\e[31m";
const QString ANSI_GREEN         = "\e[32m";
const QString ANSI_YELLOW        = "\e[33m";
const QString ANSI_BLUE          = "\e[34m";
const QString ANSI_MAGENTA       = "\e[35m";
const QString ANSI_CYRAN         = "\e[36m";
const QString ANSI_LIGHT_GRAY    = "\e[37m";

const QString ANSI_DARK_GRAY     = "\e[90m";
const QString ANSI_LIGHT_RED     = "\e[91m";
const QString ANSI_LIGHT_GREEN   = "\e[92m";
const QString ANSI_LIGHT_YELLOW  = "\e[93m";
const QString ANSI_LIGHT_BLUE    = "\e[94m";
const QString ANSI_LIGHT_MAGENTA = "\e[95m";
const QString ANSI_LIGHT_CYAN    = "\e[96m";
const QString ANSI_WHITE         = "\e[97m";

const QString ANSI_BG_DEFAULT       = "\e[49m";
const QString ANSI_BG_BLACK         = "\e[40m";
const QString ANSI_BG_RED           = "\e[41m";
const QString ANSI_BG_GREEN         = "\e[42m";
const QString ANSI_BG_YELLOW        = "\e[43m";
const QString ANSI_BG_BLUE          = "\e[44m";
const QString ANSI_BG_MAGENTA       = "\e[45m";
const QString ANSI_BG_CYRAN         = "\e[46m";
const QString ANSI_BG_LIGHT_GRAY    = "\e[47m";

const QString ANSI_BG_DARK_GRAY     = "\e[100m";
const QString ANSI_BG_LIGHT_RED     = "\e[101m";
const QString ANSI_BG_LIGHT_GREEN   = "\e[102m";
const QString ANSI_BG_LIGHT_YELLOW  = "\e[103m";
const QString ANSI_BG_LIGHT_BLUE    = "\e[104m";
const QString ANSI_BG_LIGHT_MAGENTA = "\e[105m";
const QString ANSI_BG_LIGHT_CYAN    = "\e[106m";
const QString ANSI_BG_WHITE         = "\e[107m";

/*
 * QMessageLogContext
 *   category : const char * = default
 *   file : const char *
 *   function : const char *
 *   line : int
 *   version : int
 */

QString
format_log_with_ansi(const QString & message_type, const QMessageLogContext & context, const QString & message)
{
  QString date = QDateTime::currentDateTime().toString(QLatin1Literal("yyyy-MM-dd HH:mm:ss.zzz")); // Qt::ISODate
  QString full_message = QString("%1   %2   %3\n")
    .arg(ANSI_BOLD + ANSI_RED + message_type + ANSI_RESET)
    .arg(ANSI_BOLD + ANSI_BLUE + date + ANSI_RESET)
    .arg(ANSI_BOLD + ANSI_MAGENTA + context.function + ANSI_RESET);
  // .arg(context.file)
  // .arg(context.line)

  if (!message.isEmpty()) {
    QString indented_messaged = message;
    indented_messaged.replace(QLatin1Literal("\n"), QLatin1Literal("\n  "));
    full_message += "   " + indented_messaged + "\n";
  }

  return full_message;
}

QString
format_log(const QString & message_type, const QMessageLogContext & context, const QString & message)
{
  QString date = QDateTime::currentDateTime().toString(QLatin1Literal("yyyy-MM-dd HH:mm:ss.zzz")); // Qt::ISODate
  QString full_message = QString("%1   %2   %3")
    .arg(message_type)
    .arg(date)
    .arg(context.function);
  // .arg(context.file)
  // .arg(context.line)

  full_message += message;
  full_message += '\n';

  return full_message;
}

void
message_handler(QtMsgType type, const QMessageLogContext & context, const QString & message)
{
  QString formated_message;
  QString (*formater)(const QString & message_type, const QMessageLogContext & context, const QString & message);
  // formater = format_log;
#ifdef ANDROID
  formater = format_log;
#else
  formater = format_log_with_ansi;
#endif

  switch (type) {
  case QtDebugMsg:
    formated_message = formater(QLatin1Literal("Debug"), context, message);
    break;
  case QtInfoMsg:
    formated_message = formater(QLatin1Literal("Info"), context, message);
    break;
  case QtWarningMsg:
    formated_message = formater(QLatin1Literal("Warning"), context, message);
    break;
  case QtCriticalMsg:
    formated_message = formater(QLatin1Literal("Critical"), context, message);
    break;
  case QtFatalMsg:
    formated_message = formater(QLatin1Literal("Fatal"), context, message);
  }

  // stderr
  fprintf(stdout, formated_message.toStdString().c_str()); //  # QByteArray local_message = message.toLocal8Bit();

  if (type == QtFatalMsg)
    abort();
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
