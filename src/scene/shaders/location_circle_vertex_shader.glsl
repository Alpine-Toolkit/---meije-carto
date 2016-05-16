attribute highp vec4 a_vertex;
attribute highp vec2 a_tex_coord;

uniform highp mat4 qt_Matrix;

varying highp vec2 tex_coord;

void main() {
  gl_Position = qt_Matrix * a_vertex;
  tex_coord = a_tex_coord;
}

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
