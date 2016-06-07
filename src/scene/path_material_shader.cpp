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

#include "path_material_shader.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

#include "shaders/path_shader.h"

const char *
QcPathMaterialShader::vertexShader() const
{
  return vertex_shader_path;
}

const char *
QcPathMaterialShader::fragmentShader() const
{
  return fragment_shader_path;
}

QList<QByteArray>
QcPathMaterialShader::attributes() const
{
  return QList<QByteArray>() << "a_vertex" << "a_tex_coord"
                             << "a_line_length" << "a_line_width"
                             << "a_cap";
}

/*
void
QcPathMaterialShader::updateState(const RenderState & state,
                                  QSGMaterial * new_material,
                                  QSGMaterial * old_material)
{
  qInfo() << "==================================================\n"
          << "\n combined_matrix" << state.combinedMatrix()
          << "\n determinant" << state.determinant()
          << "\n device_pixel_ratio" << state.devicePixelRatio()
          << "\n device_rect" << state.deviceRect()
          << "\n model_view_matrix" << state.modelViewMatrix()
          << "\n opacity" << state.opacity()
          << "\n projection_matrix" << state.projectionMatrix()
          << "\n viewport_rect" << state.viewportRect();

 // RootNode 0x1fba6b0  "" )
 //   TransformNode( 0x4e43aa0 identity "QQuickItem(QQuickRootItem:)"  )
 //     TransformNode( 0x5278780 translate 0 48 0 "QQuickItem(QQuickItem:)"  )
 //       TransformNode( 0x526b840 identity "QQuickItem(MapComponent_QMLTYPE_0:)"  )
 //         ClipNode( 0x526e530 children= 6 is rect? yes ) "" 
 //           GeometryNode( 0x526ecc0 strip #V: 4 #I: 0 x1= 0 y1= 0 x2= 800 y2= 752 materialtype= 0x7ff638f84ed2 ) ""
 //             ClipNode( 0x526ee10 children= 1 is rect? yes ) "" 
 //               TransformNode( 0x526ef90 identity ""  )
 //                 OpacityNode( 0x526f750 opacity= 1 combined= 1  "" )
 //                   TransformNode( 0x526f7f0 identity ""  )
 //                   TransformNode( 0x526f8e0 identity ""  )
 //                     GeometryNode( 0x52714e0 strip #V: 4 #I: 0 x1= 412.47 y1= 253.08 x2= 668.47 y2= 509.08 materialtype= 0x7ff638f84ed3 ) "simpletexture"
 //                     GeometryNode( 0x52717c0 strip #V: 4 #I: 0 x1= -99.53 y1= 509.08 x2= 156.47 y2= 765.08 materialtype= 0x7ff638f84ed3 ) "simpletexture"
 //                     GeometryNode( 0x5271a10 strip #V: 4 #I: 0 x1= 412.47 y1= -2.9199 x2= 668.47 y2= 253.08 materialtype= 0x7ff638f84ed3 ) "simpletexture"
 //                     GeometryNode( 0x5272ee0 strip #V: 4 #I: 0 x1= 668.47 y1= 509.08 x2= 924.47 y2= 765.08 materialtype= 0x7ff638f84ed3 ) "simpletexture"
 //                   TransformNode( 0x526f9d0 translate 800 0 0 ""  )
 //                 OpacityNode( 0x5269ad0 opacity= 0.25 combined= 0.25  "" )
 //                   GeometryNode( 0x526f0b0 strip #V: 4 #I: 0 x1= 290 y1= 266 x2= 510 y2= 486 materialtype= 0x7ff639994728 ) ""
 //                 OpacityNode( 0x526f200 opacity= 1 combined= 1  "" )
 //                   GeometryNode( 0x526f320 strip #V: 12 #I: 0 x1= 91.5397 y1= 87.0941 x2= 807.623 y2= 617.623 materialtype= 0x7ff639994790 ) ""

  //  combined_matrix QMatrix4x4(type:Translation,Scale
  //     0.0025         0         0        -1         
  //          0   -0.0025         0      0.88         
  //          0         0         1         0         
  //          0         0         0         1         
  // ) 
  //  determinant 1 
  //  device_pixel_ratio 1 
  //  device_rect QRect(0,0 800x800) 
  //  model_view_matrix QMatrix4x4(type:Translation
  //          1         0         0         0         
  //          0         1         0        48         
  //          0         0         1         0         
  //          0         0         0         1         
  // ) 
  //  opacity 1 
  //  projection_matrix QMatrix4x4(type:Translation,Scale
  //     0.0025         0         0        -1         
  //          0   -0.0025         0         1         
  //          0         0         1         0         
  //          0         0         0         1         
  // ) 
  //  viewport_rect QRect(0,0 800x800)

  // 2/800 = .0025



  // RootNode 0x43c1950  "" )
  // TransformNode( 0x254d5d0 identity "QQuickItem(QQuickRootItem:)"  )
  //   TransformNode( 0x4800e80 translate 0 48 0 "QQuickItem(QQuickItem:)"  )
  //     TransformNode( 0x47eec20 identity "QQuickItem(MapComponent_QMLTYPE_0:)"  )
  //       ClipNode( 0x47f1900 children= 6 is rect? yes ) "" 
  //         GeometryNode( 0x47f2080 strip #V: 4 #I: 0 x1= 0 y1= 0 x2= 800 y2= 458 materialtype= 0x7fe3aa82aed2 ) ""
  //           ClipNode( 0x47f21d0 children= 1 is rect? yes ) "" 
  //             TransformNode( 0x47f2330 identity ""  )
  //               OpacityNode( 0x47f2ae0 opacity= 1 combined= 1  "" )
  //                 TransformNode( 0x47f2b80 identity ""  )
  //                 TransformNode( 0x47f2c70 identity ""  )
  //                   GeometryNode( 0x47f4860 strip #V: 4 #I: 0 x1= -99.53 y1= 106.08 x2= 156.47 y2= 362.08 materialtype= 0x7fe3aa82aed3 ) "simpletexture"
  //                   GeometryNode( 0x47f4fe0 strip #V: 4 #I: 0 x1= 668.47 y1= 362.08 x2= 924.47 y2= 618.08 materialtype= 0x7fe3aa82aed3 ) "simpletexture"
  //                   GeometryNode( 0x47f6260 strip #V: 4 #I: 0 x1= -99.53 y1= 362.08 x2= 156.47 y2= 618.08 materialtype= 0x7fe3aa82aed3 ) "simpletexture"
  //                 TransformNode( 0x47f2d60 translate 800 0 0 ""  )
  //               OpacityNode( 0x47ee280 opacity= 0.25 combined= 0.25  "" )
  //                 GeometryNode( 0x47f2410 strip #V: 4 #I: 0 x1= 290 y1= 266 x2= 510 y2= 486 materialtype= 0x7fe3ab23a728 ) ""
  //               OpacityNode( 0x47f2560 opacity= 1 combined= 1  "" )
  //                 GeometryNode( 0x47f26b0 strip #V: 12 #I: 0 x1= 91.5397 y1= 87.0941 x2= 807.623 y2= 617.623 materialtype= 0x7fe3ab23a790 ) ""

  //  combined_matrix QMatrix4x4(type:Translation,Scale
  //     0.0025         0         0        -1         
  //          0-0.00395257         0  0.810277         
  //          0         0         1         0         
  //          0         0         0         1         
  // ) 
  //  determinant 1 
  //  device_pixel_ratio 2 
  //  device_rect QRect(0,0 1600x1012) 
  //  model_view_matrix QMatrix4x4(type:Translation
  //          1         0         0         0         
  //          0         1         0        48         
  //          0         0         1         0         
  //          0         0         0         1         
  // ) 
  //  opacity 1 
  //  projection_matrix QMatrix4x4(type:Translation,Scale
  //     0.0025         0         0        -1         
  //          0-0.00395257         0         1         
  //          0         0         1         0         
  //          0         0         0         1         
  // ) 
  //  viewport_rect QRect(0,0 1600x1012)

  // 1600 =  800 * 2
  // 1012 = (458 + 48) * 2
}
*/

void
QcPathMaterialShader::updateState(const QcPathMaterialShaderState * state,
                                  const QcPathMaterialShaderState *)
{
  program()->setUniformValue("colour", state->r, state->g, state->b, state->a);
  program()->setUniformValue("cap_type", state->cap_type);
  program()->setUniformValue("line_join", state->line_join);
  program()->setUniformValue("antialias_diameter", state->antialias_diameter);
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
