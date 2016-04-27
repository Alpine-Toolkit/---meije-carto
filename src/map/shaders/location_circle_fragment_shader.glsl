#version 120

const float antialias = 1.0;
const float linewidth = 1.5;
const float M_PI = 3.14159265358979323846;
const float SQRT_2 = 1.4142135623730951;

uniform lowp float qt_Opacity;
uniform lowp vec4 color;

vec4
stroke(float distance, float linewidth, float antialias, vec4 stroke)
{
  vec4 frag_color;
  float t = linewidth/2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance/antialias;
  alpha = exp(-alpha*alpha);

  if (border_distance > (linewidth/2.0 + antialias))
    discard;
  else if (border_distance < 0.0)
    frag_color = stroke;
  else
    frag_color = vec4(stroke.rgb*alpha, 1.);

  return frag_color;
}

vec4
filled(float distance, float linewidth, float antialias, vec4 fill)
{
  vec4 frag_color;
  float t = linewidth/2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance/antialias;
  alpha = exp(-alpha*alpha);

  // if (alpha == .0) discard;
  // Within linestroke
  if (border_distance < 0.0)
    frag_color = fill;
  // Within shape
  else if (signed_distance < 0.0)
    frag_color = fill;
  else
    // Outside shape
    if (border_distance > (linewidth/2.0 + antialias))
      frag_color = vec4(0, 0, 0, 0); //discard;
    else { // Line stroke exterior border
      // frag_color = vec4(1, 0, 0, 0);
      frag_color = vec4(fill.rgb*alpha, 1.0);
    }

  return frag_color;
}

vec4
outline(float distance, float linewidth, float antialias, vec4 stroke, vec4 fill)
{
  vec4 frag_color;
  float t = linewidth/2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance/antialias;
  alpha = exp(-alpha*alpha);

  // Within linestroke
  if (border_distance < 0.0)
    frag_color = stroke;
  else if (signed_distance < 0.0)
    // Inside shape
    if (border_distance > (linewidth/2.0 + antialias))
      frag_color = fill;
    else // Line stroke interior border
      frag_color = mix(fill, stroke, alpha);
  else
    // Outside shape
    if (border_distance > (linewidth/2.0 + antialias))
      discard;
    else // Line stroke exterior border
      frag_color = vec4(stroke.rgb*alpha, 1.0);

  return frag_color;
}

float
marker_ring(vec2 P)
{
  return 100*length(P - vec2(.5, .5)) - 90/2.;
}

void
main() {
  float d = marker_ring(gl_PointCoord);
  vec4 frag_color = filled(d, linewidth, antialias, color);
  gl_FragColor = frag_color * qt_Opacity;
}
