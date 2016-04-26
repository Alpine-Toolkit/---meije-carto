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

#include "location_circle_material_shader.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

const char *
QcLocationCircleMaterialShader::vertexShader() const
{
  return
    " attribute highp vec4 vertex;          \n"
    " uniform highp mat4 qt_Matrix;         \n"
    " void main() {                         \n"
    "     gl_Position = qt_Matrix * vertex; \n"
    " } ";
}

const char *
QcLocationCircleMaterialShader::fragmentShader() const
{
  return
    "#version 120 \n"
    "const float antialias = 1.0;                     \n"
    "const float linewidth = 1.5;                     \n"
    "uniform lowp float qt_Opacity;                   \n"
    "uniform lowp vec4 color;                         \n"
    "const float M_PI = 3.14159265358979323846;                                               \n"
    "const float SQRT_2 = 1.4142135623730951;                                                 \n"
    "                                                                                         \n"
    "vec4 stroke(float distance, float linewidth, float antialias, vec4 stroke)               \n"
    "{                                                                                        \n"
    "  vec4 frag_color;                                                                       \n"
    "  float t = linewidth/2.0 - antialias;                                                   \n"
    "  float signed_distance = distance;                                                      \n"
    "  float border_distance = abs(signed_distance) - t;                                      \n"
    "  float alpha = border_distance/antialias;                                               \n"
    "  alpha = exp(-alpha*alpha);                                                             \n"
    "                                                                                         \n"
    "  if (border_distance > (linewidth/2.0 + antialias))                                     \n"
    "    discard;                                                                             \n"
    "  else if (border_distance < 0.0)                                                        \n"
    "    frag_color = stroke;                                                                 \n"
    "  else                                                                                   \n"
    "    frag_color = vec4(stroke.rgb*alpha, 1.);                                             \n"
    "                                                                                         \n"
    "  return frag_color;                                                                     \n"
    "}                                                                                        \n"
    "                                                                                         \n"
    "vec4 filled(float distance, float linewidth, float antialias, vec4 fill)                 \n"
    "{                                                                                        \n"
    "  vec4 frag_color;                                                                       \n"
    "  float t = linewidth/2.0 - antialias;                                                   \n"
    "  float signed_distance = distance;                                                      \n"
    "  float border_distance = abs(signed_distance) - t;                                      \n"
    "  float alpha = border_distance/antialias;                                               \n"
    "  alpha = exp(-alpha*alpha);                                                             \n"
    "  //if (alpha == .0) discard;                                                            \n"
    "  // Within linestroke                                                                   \n"
    "  if (border_distance < 0.0)                                                             \n"
    "    frag_color = fill;                                                                   \n"
    "  // Within shape                                                                        \n"
    "  else if (signed_distance < 0.0)                                                        \n"
    "    frag_color = fill;                                                                   \n"
    "  else                                                                                   \n"
    "    // Outside shape                                                                     \n"
    "    if (border_distance > (linewidth/2.0 + antialias))                                   \n"
    "      frag_color = vec4(0, 0, 0, 0); //discard;                                          \n"
    "    else {// Line stroke exterior border                                                 \n"
    "      // frag_color = vec4(1, 0, 0, 0);                                                  \n"
    "       frag_color = vec4(fill.rgb*alpha, 1.0);                                           \n"
    "       }                                                                                 \n"
    "                                                                                         \n"
    "  return frag_color;                                                                     \n"
    "}                                                                                        \n"
    "                                                                                         \n"
    "vec4 outline(float distance, float linewidth, float antialias, vec4 stroke, vec4 fill)   \n"
    "{                                                                                        \n"
    "  vec4 frag_color;                                                                       \n"
    "  float t = linewidth/2.0 - antialias;                                                   \n"
    "  float signed_distance = distance;                                                      \n"
    "  float border_distance = abs(signed_distance) - t;                                      \n"
    "  float alpha = border_distance/antialias;                                               \n"
    "  alpha = exp(-alpha*alpha);                                                             \n"
    "                                                                                         \n"
    "  // Within linestroke                                                                   \n"
    "  if (border_distance < 0.0)                                                             \n"
    "    frag_color = stroke;                                                                 \n"
    "  else if (signed_distance < 0.0)                                                        \n"
    "    // Inside shape                                                                      \n"
    "    if (border_distance > (linewidth/2.0 + antialias))                                   \n"
    "      frag_color = fill;                                                                 \n"
    "    else // Line stroke interior border                                                  \n"
    "      frag_color = mix(fill, stroke, alpha);                                             \n"
    "  else                                                                                   \n"
    "    // Outside shape                                                                     \n"
    "    if (border_distance > (linewidth/2.0 + antialias))                                   \n"
    "      discard;                                                                           \n"
    "    else // Line stroke exterior border                                                  \n"
    "      frag_color = vec4(stroke.rgb*alpha, 1.0);                                          \n"
    "                                                                                         \n"
    "  return frag_color;                                                                     \n"
    "}                                                                                        \n"
    "float marker_ring(vec2 P)            \n"
    "{                                                \n"
    "  return 100*length(P - vec2(.5, .5)) - 90/2.;                            \n"
    "}                                                \n"
    "void main() {                                    \n"
    "    float d = marker_ring(gl_PointCoord); \n"
    "    vec4 frag_color = filled(d, linewidth, antialias, color);  \n"
    "    gl_FragColor = frag_color * qt_Opacity;     \n"
    "}";
}

QList<QByteArray>
QcLocationCircleMaterialShader::attributes() const
{
  return QList<QByteArray>() << "vertex";
}

void
QcLocationCircleMaterialShader::updateState(const QcLocationCircleMaterialShaderState * state,
                                            const QcLocationCircleMaterialShaderState *)
{
  program()->setUniformValue("color", state->r, state->g, state->b, state->a);
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
