#version 440

uniform sampler2D frame;
uniform vec2 res;

#define RADIUS 0.75
#define SOFTNESS 0.60

layout(location = 0) in vec2 tex_coord;

layout(location = 0) out vec4 colour;

void main() {
  //Sample texture
  vec4 tex_color = texture(frame, tex_coord);
  //determine center origin
  vec2 position = (gl_FragCoord.xy / res.xy) - vec2(0.5);
  //aspect ratio correction
  position.x *= res.x / res.y;
  //Vector mangitude of center
  float len = length(position);
  //vingette factor
  float vignette = smoothstep(RADIUS, RADIUS - SOFTNESS, len);
  tex_color.xyz = mix(tex_color.xyz, tex_color.xyz * vignette, 0.575);

  colour = tex_color;
  colour.w = 1;
}