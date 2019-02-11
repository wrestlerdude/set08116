#version 440

// Main textures
uniform sampler2D tex[2];
// Blend map
uniform sampler2D blend;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing fragment colour
layout(location = 0) out vec4 colour;

void main() {
  vec4 s1 = texture(tex[0], tex_coord);
  vec4 s2 = texture(tex[1], tex_coord);
  vec4 sb = texture(blend, tex_coord);
  colour = mix(s1, s2, sb[0]);
}