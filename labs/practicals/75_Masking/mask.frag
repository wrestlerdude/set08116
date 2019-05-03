#version 430 core

// Captured render
uniform sampler2D tex;
// Alpha map
uniform sampler2D alpha_map;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // Sample textures
  vec4 tex_col = texture(tex, tex_coord);
  vec4 mask = texture(alpha_map, tex_coord);
  // Final colour is produce of these two colours
  colour = tex_col * mask;
  // Ensure alpha is 1
  colour.w = 1;
}