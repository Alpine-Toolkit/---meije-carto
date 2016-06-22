/* *********************************************************************************************** */

uniform highp mat4 qt_Matrix;

/* *********************************************************************************************** */

attribute highp vec4 a_vertex;
attribute highp vec2 a_tex_coord;
attribute highp vec3 a_radius;
attribute highp float a_angle;

/* *********************************************************************************************** */

varying highp vec2 tex_coord;
varying highp vec3 radius;
varying highp float angle;

/* *********************************************************************************************** */

void main() {
  tex_coord = a_tex_coord;
  radius = a_radius;
  angle = a_angle;
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
