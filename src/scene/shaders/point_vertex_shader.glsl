/* *********************************************************************************************** */

uniform highp mat4 qt_Matrix;

/* *********************************************************************************************** */

attribute highp vec4 a_vertex;
attribute highp vec2 a_tex_coord;
attribute highp float a_radius;
attribute lowp vec4 a_colour;

/* *********************************************************************************************** */

varying highp vec2 tex_coord;
varying highp float radius;
varying lowp vec4 colour;

/* *********************************************************************************************** */

void main() {
  tex_coord = a_tex_coord;
  radius = a_radius;
  colour = a_colour;
  gl_Position = qt_Matrix * a_vertex;
}

/* *********************************************************************************************** */

/*
attribute highp float _qt_order;

uniform highp float _qt_zRange;

void main() {
  gl_Position.z = (gl_Position.z * _qt_zRange + _qt_order) * gl_Position.w;
}
*/

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
