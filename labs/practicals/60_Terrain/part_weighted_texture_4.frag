#version 440

vec4 weighted_texture(in sampler2D tex[4], in vec2 tex_coord, in vec4 weights) {
  vec4 tex_colour = vec4(0, 0, 0, 1);
  // *********************************
  // Sample all Four textures based on weight
  for (int i = 0; i < 4; i++) {
    tex_colour += texture(tex[i], tex_coord) * weights.x;  
  }

  return tex_colour;
}