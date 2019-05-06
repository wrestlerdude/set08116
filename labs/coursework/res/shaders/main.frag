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
uniform float height_scale;

uniform sampler2D tex;
uniform sampler2D dissolve;
uniform sampler2D shadow_map[5];
uniform sampler2D normal_map;
uniform sampler2D depth_map;
uniform samplerCube cubemap;

uniform bool env_map;
uniform bool normal_b;
uniform bool parallax;

uniform spot_light spots[5];
uniform point_light points[6];
uniform material mat;

uniform vec3 eye_pos;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tex_coord;
layout(location = 3) in vec4 light_space_pos[5];
layout(location = 8) in vec3 tangent;
layout(location = 9) in vec3 binormal;

layout(location = 0) out vec4 frag_colour;

vec4 calculate_point(in point_light point, in material mat, in vec3 position,
                     in vec3 normal, in vec3 view_dir, in vec4 tex_colour);

vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position,
                in vec3 normal, in vec3 view_dir, in vec4 tex_colour, in float ambient_intensity);

float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);

void parallax_occulsion(inout vec3 tex_coord, in vec3 view_dir, sampler2D depth_map, float height_scale);

vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord);

void main() {
  vec3 view_dir = normalize(eye_pos - position);
  vec3 new_tex_coord;
  new_tex_coord = tex_coord;

  if (parallax)
    parallax_occulsion(new_tex_coord, view_dir, depth_map, height_scale);

  vec4 tex_colour;
  if (env_map)
    tex_colour = texture(cubemap, new_tex_coord);
  else
    tex_colour = texture(tex, new_tex_coord.xy);

  vec3 mapped_normal;
  if (normal_b || parallax)
    mapped_normal = calc_normal(normal, tangent, binormal, normal_map, new_tex_coord.xy);
  else
    mapped_normal = normal;

  //Phong point lights
  for (int i = 0; i < points.length(); i++)
    frag_colour += calculate_point(points[i], mat, position, mapped_normal, view_dir, tex_colour);
  
  //Phong spot lights
  for (int i = 0; i < spots.length(); i++)
    frag_colour += calculate_spot(spots[i], mat, position, mapped_normal, view_dir, tex_colour, ambient_intensity);
  
  //Must be a more efficient way?
  float shade;
  for (int i = 0; i < shadow_map.length(); i++) {
    shade = calculate_shadow(shadow_map[i], light_space_pos[i]);
    if (shade < 1.0)
      break;
  }

  frag_colour *= shade;
  frag_colour.w = 1;
}