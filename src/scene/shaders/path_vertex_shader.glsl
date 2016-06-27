/* *********************************************************************************************** */

uniform highp mat4 qt_Matrix;

/* *********************************************************************************************** */

// attribute highp vec2 a_vertex;
attribute highp vec4 a_vertex;
attribute highp vec2 a_tex_coord;
attribute highp float a_line_length;
attribute highp float a_line_width;
attribute lowp float a_cap;
attribute lowp vec4 a_colour;

/* *********************************************************************************************** */

varying highp vec2 uv;
varying highp float line_length;
varying highp float line_width;
varying lowp float cap;
varying lowp vec4 colour;

/* *********************************************************************************************** */

void main() {
  uv = a_tex_coord;
  line_length = a_line_length;
  line_width = a_line_width;
  cap = a_cap;
  colour = a_colour;

  gl_Position = qt_Matrix * a_vertex;
  // gl_Position = qt_Matrix * vec4(a_vertex.xy, 0, 1);
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
