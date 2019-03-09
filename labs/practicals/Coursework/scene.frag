#version 440

// Sampler used to get texture colour
uniform sampler2D tex;
uniform bool texture_exists;
// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing colour
layout(location = 0) out vec4 out_colour;

void main() {
  vec4 tex_colour;
  if (texture_exists)
    tex_colour = texture(tex, tex_coord);
  else
    tex_colour = vec4(1, 1, 1, 1);

  out_colour = tex_colour;
}