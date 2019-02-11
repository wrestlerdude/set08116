#version 440

uniform sampler1D tex;

// Incoming texture coordinate
layout(location = 0) in float tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  colour = texture(tex, tex_coord);
}