#version 440

#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#endif

#ifndef POINT_LIGHT
#define POINT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

uniform float ambient_intensity;
uniform float dissolve_factor;

uniform sampler2D tex;
uniform sampler2D dissolve;
uniform sampler2D shadow_map;

uniform bool texture_exists;
uniform bool dissolve_enabled;

uniform spot_light spots[4];
uniform point_light points[5];
uniform material mat;

uniform vec3 eye_pos;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 light_space_pos;

layout(location = 0) out vec4 frag_colour;

vec4 calculate_point(in point_light point, in material mat, in vec3 position,
                     in vec3 normal, in vec3 view_dir, in vec4 tex_colour);

vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position,
                in vec3 normal, in vec3 view_dir, in vec4 tex_colour, in float ambient_intensity);

float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);

void main() {
  //Dissolve calculation
  if (dissolve_enabled) {
    vec4 dissolve_v = texture(dissolve, tex_coord);
    if (dissolve_v[0] > dissolve_factor)
      discard;
  }

  vec4 tex_colour;
  if (texture_exists)
    tex_colour = texture(tex, tex_coord);
  else
    //Normally full white but in current scene only untextured thing is the amethyst
    tex_colour = vec4(1, 0, 1, 1);

  vec3 view_dir = normalize(eye_pos - position);
  
  //Phong point lights
  for (int i = 0; i < 5; i++)
    frag_colour += calculate_point(points[i], mat, position, normal, view_dir, tex_colour);
  
  //Phong spot lights
  for (int i = 0; i < 4; i++)
    frag_colour += calculate_spot(spots[i], mat, position, normal, view_dir, tex_colour, ambient_intensity);

    
  float shade = calculate_shadow(shadow_map, light_space_pos);

  frag_colour *= shade;
  frag_colour.w = 1;
}