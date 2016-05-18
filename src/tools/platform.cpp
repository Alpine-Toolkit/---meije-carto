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

#include "platform.h"

#include <QSysInfo>
#include <QGuiApplication>
#include <QScreen>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QcPlatform::QcPlatform()
{
#ifdef ON_LINUX
  m_os_type = LINUX;
#endif
#ifdef ON_ANDROID
  m_os_type = ANDROID;
#endif
#ifdef ON_WINDOWS
  m_os_type = WINDOWS;
#endif
}

QcPlatform::~QcPlatform()
{}

QString
QcPlatform::os_name() const
{
  switch (m_os_type) {
  case LINUX:
    return QLatin1Literal("linux");
  case ANDROID:
    return QLatin1Literal("android");
  case WINDOWS:
    return QLatin1Literal("windows");
  case OSX:
    return QLatin1Literal("osx");
  }
}

QString
QcPlatform::architecture_name() const
{
  switch (m_architecture_type) {
  case X86:
    return QLatin1Literal("x86");
  case X86_64:
    return QLatin1Literal("x86_64");
  case ARM:
    return QLatin1Literal("arm");
  case ARM_64:
    return QLatin1Literal("arm_64");
  }
}

QJsonObject
QcPlatform::to_json() const
{
  QJsonObject platform_object;
  platform_object[QLatin1Literal("os")] = os_name();
  platform_object[QLatin1Literal("architecture")] = architecture_name();
  platform_object[QLatin1Literal("build_abi")] = QSysInfo::buildAbi();
  platform_object[QLatin1Literal("build_cpu_architecture")] = QSysInfo::buildCpuArchitecture();
  platform_object[QLatin1Literal("current_cpu_architecture")] = QSysInfo::currentCpuArchitecture();
  platform_object[QLatin1Literal("product_type")] = QSysInfo::productType();
  platform_object[QLatin1Literal("product_version")] = QSysInfo::productVersion();
  platform_object[QLatin1Literal("pretty_product_name")] = QSysInfo::prettyProductName();
  if (on_windows())
    platform_object[QLatin1Literal("windows_version")] = QSysInfo::windowsVersion();
  if (on_osx())
    platform_object[QLatin1Literal("osx_version")] = QSysInfo::macVersion();

  QGuiApplication * application = static_cast<QGuiApplication *>(QGuiApplication::instance());

  QJsonObject screens_object;
  for (const auto * screen : application->screens()) {
    QJsonObject screen_object;
    screen_object[QLatin1Literal("device_pixel_ratio")] = screen->devicePixelRatio();
    QSize resolution = screen->size();
    QSizeF physical_size = screen->physicalSize();
    screen_object[QLatin1Literal("resolution")] =
      QString::number(resolution.width()) + 'x' + QString::number(resolution.height()) + QLatin1Literal(" px");
    screen_object[QLatin1Literal("screen_format_px")] = resolution.width() / float(resolution.height());
    screen_object[QLatin1Literal("physical_size")] =
      QString::number(physical_size.width()) + 'x' + QString::number(physical_size.height()) + QLatin1Literal(" mm");
    screen_object[QLatin1Literal("screen_format_mm")] = physical_size.width() / physical_size.height();
    screen_object[QLatin1Literal("physical_dots_per_inch_x")] = screen->physicalDotsPerInchX();
    screen_object[QLatin1Literal("physical_dots_per_inch_y")] = screen->physicalDotsPerInchY();
    screen_object[QLatin1Literal("logical_dots_per_inch_x")] = screen->logicalDotsPerInchX();
    screen_object[QLatin1Literal("logical_dots_per_inch_y")] = screen->logicalDotsPerInchY();
    screens_object[screen->name()] = screen_object;
  }
  platform_object[QLatin1Literal("screens")] = screens_object;

  return platform_object;
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
