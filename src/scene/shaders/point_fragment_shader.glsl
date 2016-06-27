/**************************************************************************************************/

// #version 120

const float antialias = 1.0;
const float linewidth = 1.5;
const float M_PI = 3.14159265358979323846;
const float SQRT_2 = 1.4142135623730951;

/**************************************************************************************************/

uniform lowp float qt_Opacity;


/**************************************************************************************************/

varying highp vec2 tex_coord;
varying highp float radius;
varying lowp vec4 colour;

/**************************************************************************************************/

vec4
stroke(float distance, float linewidth, float antialias, vec4 stroke)
{
  vec4 frag_colour;
  float t = linewidth/2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance/antialias;
  alpha = exp(-alpha*alpha);

  if (border_distance > (linewidth/2.0 + antialias))
    discard;
  else if (border_distance < 0.0)
    frag_colour = stroke;
  else
    frag_colour = vec4(stroke.rgb*alpha, 1.);

  return frag_colour;
}

vec4
filled(float distance, float linewidth, float antialias, vec4 fill)
{
  vec4 frag_colour;
  float t = linewidth/2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance/antialias;
  alpha = exp(-alpha*alpha);

  // if (alpha == .0) discard;
  // Within linestroke
  if (border_distance < 0.0)
    frag_colour = fill;
  // Within shape
  else if (signed_distance < 0.0)
    frag_colour = fill;
  else
    // Outside shape
    if (border_distance > (linewidth/2.0 + antialias))
      discard;
      // frag_colour = vec4(0, 0, 0, 0);
    else { // Line stroke exterior border
      if (alpha == .0)
        discard;
      else
        // frag_colour = vec4(fill.rgb*alpha, 1.);
        frag_colour = vec4(fill.rgb*alpha, alpha);
    }

  return frag_colour;
}

vec4
outline(float distance, float linewidth, float antialias, vec4 stroke, vec4 fill)
{
  vec4 frag_colour;
  float t = linewidth/2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance/antialias;
  alpha = exp(-alpha*alpha);

  // Within linestroke
  if (border_distance < 0.0)
    frag_colour = stroke;
  else if (signed_distance < 0.0)
    // Inside shape
    if (border_distance > (linewidth/2.0 + antialias))
      frag_colour = fill;
    else // Line stroke interior border
      frag_colour = mix(fill, stroke, alpha);
  else
    // Outside shape
    if (border_distance > (linewidth/2.0 + antialias))
      discard;
    else // Line stroke exterior border
      frag_colour = vec4(stroke.rgb*alpha, 1.0);

  return frag_colour;
}

float
marker_ring(vec2 P, float radius)
{
  return length(P) - radius;
}

/**************************************************************************************************/

void
main() {
  float d = marker_ring(tex_coord, radius);
  vec4 frag_colour = filled(d, linewidth, antialias, colour);

  gl_FragColor = frag_colour * qt_Opacity;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
