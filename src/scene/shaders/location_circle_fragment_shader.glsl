/**************************************************************************************************/

// #version 120

const float antialias = 1.0;
const float linewidth = 1.5;
const float M_PI = 3.14159265358979323846;
const float SQRT_2 = 1.4142135623730951;

/**************************************************************************************************/

uniform lowp float qt_Opacity;

uniform lowp vec4 cone_colour;
uniform lowp vec4 accuracy_colour;

/**************************************************************************************************/

varying highp vec2 tex_coord;
varying highp vec3 radius;
varying highp float angle;

/**************************************************************************************************/

float
planar_rotation(vec2 P, float angle)
{
  angle *= M_PI / 180.;
  vec2 cos_sin = vec2(cos(angle), sin(angle));
  return dot(cos_sin, P);
}

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

/**************************************************************************************************/

float
marker_ring(vec2 P, float radius)
{
  return length(P) - radius;
}

float
location_marker(vec2 P, vec3 radius, float angle)
{
  float p1 = planar_rotation(P,  30. + angle);
  float p2 = planar_rotation(P, -30. + angle);
  float r_cone = marker_ring(P, radius.x);
  float r_dot = marker_ring(P, radius.y);
  float cone = max(p1, p2);
  return min(max(cone, r_cone), r_dot);
}

/**************************************************************************************************/

void
main() {
  // float d = marker_ring(tex_coord, radius);
  float d_accuray = marker_ring(tex_coord, radius.z);
  float d_location = location_marker(tex_coord, radius, angle);
  vec4 colour;
  if (d_accuray < linewidth)
    colour = accuracy_colour;
  else
    colour = cone_colour;
  vec4 frag_colour = filled(d_location, linewidth, antialias, colour);
  gl_FragColor = frag_colour * qt_Opacity;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
