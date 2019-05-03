#version 430 core

// Incoming frame data
uniform sampler2D tex;

// 1.0f / screen width
uniform float inverse_width;
// 1.0f / screen height
uniform float inverse_height;

// Surrounding pixels to sample and their scale
const vec4 samples[] = vec4[4](vec4(-1.0, 0.0, 0.0, 0.25), vec4(1.0, 0.0, 0.0, 0.25), vec4(0.0, 1.0, 0.0, 0.25),
                                vec4(0.0, -1.0, 0.0, 0.25));
//EDGE DETECTION: vec4[](vec4(-1.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 2.0), vec4(1.0, 1.0, 0.0, 1.0), vec4(1.0, -1.0, 0.0, -1.0), vec4(0.0, -1.0, 0.0, -2.0), vec4(-1.0, -1.0, 0.0, -1.0));
//SHARPENING FILTER: vec4[](vec4(0, 0, 0, 11 / 3), vec4(-1, 0, 0, -2 / 3), vec4(1, 0, 0, -2 / 3), vec4(0, 1, 0, -2 / 3), vec4(0, -1, 0, -2 / 3));

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;


void main() {
  // Start with colour as black
  colour += vec4(0, 0, 0, 1);

  // Loop through each sample vector
  for (int i = 0; i < samples.length(); i++) {
    // Calculate tex coord to sample
    vec2 uv = tex_coord + vec2(samples[i].w * inverse_width, samples[i].y * inverse_height);
    // Sample the texture and scale appropriately
    // - scale factor stored in w component
    vec4 tex_colour = texture(tex, uv);
    tex_colour *= samples[i].w;

    colour += tex_colour;
 }
  
  // Ensure alpha is 1.0
  colour.w = 1;
}