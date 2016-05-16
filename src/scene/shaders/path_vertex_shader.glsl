/* *********************************************************************************************** */

uniform highp mat4 qt_Matrix;

/* *********************************************************************************************** */

// attribute highp vec2 a_vertex;
attribute highp vec4 a_vertex;
attribute highp vec2 a_tex_coord;
attribute float a_line_length;
attribute float a_line_width;
attribute float a_cap;

/* *********************************************************************************************** */

varying highp vec2 uv;
varying float line_length;
varying float line_width;
/* varying vec4 colour; */
varying float cap;

/* *********************************************************************************************** */

void main() {
  uv = a_tex_coord;
  line_length = a_line_length;
  line_width = a_line_width;
  cap = a_cap;

  gl_Position = qt_Matrix * a_vertex;
  // gl_Position = qt_Matrix * vec4(a_vertex.xy, 0, 1);
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
