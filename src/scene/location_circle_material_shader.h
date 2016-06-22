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

#ifndef __LOCATION_CIRCLE_MATERIAL_SHADER_H__
#define __LOCATION_CIRCLE_MATERIAL_SHADER_H__

/**************************************************************************************************/

#include <QSGSimpleMaterialShader>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

struct QcLocationCircleMaterialShaderState
{
  float cone_r, cone_g, cone_b, cone_a;
  float accuracy_r, accuracy_g, accuracy_b, accuracy_a;
};

class QcLocationCircleMaterialShader : public QSGSimpleMaterialShader<QcLocationCircleMaterialShaderState>
{
    QSG_DECLARE_SIMPLE_SHADER(QcLocationCircleMaterialShader, QcLocationCircleMaterialShaderState)

public:
  const char * vertexShader() const Q_DECL_OVERRIDE ;
  const char * fragmentShader() const Q_DECL_OVERRIDE ;
  QList<QByteArray> attributes() const Q_DECL_OVERRIDE ;
  void updateState(const QcLocationCircleMaterialShaderState * state,
                   const QcLocationCircleMaterialShaderState *) Q_DECL_OVERRIDE ;
};

// QC_END_NAMESPACE

#endif /* __LOCATION_CIRCLE_MATERIAL_SHADER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
