#version 440

uniform sampler2D tex;
uniform sampler2D dissolve;

uniform float factor;

// Incoming texture coordinates
layout(location = 0) in vec2 tex_coord;
// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  vec4 dissolve_v = texture(dissolve, tex_coord);
  if (dissolve_v[0] > factor)
    discard;
  // Get texture colour
  colour = texture(tex, tex_coord);
}