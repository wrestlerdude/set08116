#version 440

struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

uniform float ambient_intensity;
uniform float dissolve_factor;

uniform sampler2D tex;
uniform sampler2D dissolve;

uniform bool texture_exists;
uniform bool dissolve_enabled;

uniform spot_light spots[4];
uniform point_light points[5];
uniform material mat;

uniform vec3 eye_pos;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;

layout(location = 0) out vec4 frag_colour;

vec4 calculate_point(in point_light point, in material mat, in vec3 position,
                     in vec3 normal, in vec3 view_dir, in vec4 tex_colour) {
  float d = distance(point.position, position);
  float c = 1 / (point.constant + (point.linear * d) + (point.quadratic * (d * d)));
  vec4 light_colour = c * point.light_colour;
  vec3 light_dir = normalize(point.position - position);
  vec4 diffuse = max(dot(normal, light_dir), 0) * (mat.diffuse_reflection * light_colour);
  vec3 half_vector = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(normal, half_vector), 0), mat.shininess) * (mat.specular_reflection * light_colour);
  vec4 primary = mat.emissive + diffuse;
  primary.w = 1;
  specular.w = 1;

  return (primary * tex_colour) +  specular;
}

vec4 phong_spot(in spot_light spot, in material mat, in vec3 position,
                in vec3 normal, in vec3 view_dir, in vec4 tex_colour) {
  vec3 light_dir = normalize(spot.position - position);
  float d = distance(spot.position, position);
  //Calculate attentuation factor
  float c = 1 / (spot.constant + (spot.linear * d) + (spot.quadratic * (d * d)));
  float intensity = pow(max(dot(-spot.direction, light_dir), 0), spot.power);
  vec4 light_colour = (c * intensity) * spot.light_colour;
  //Calculate diffuse component
  vec4 diffuse = max(dot(normal, light_dir), 0) * (mat.diffuse_reflection * light_colour);
  vec3 half_vector = normalize(light_dir + view_dir);
  //Calculate specular component
  vec4 specular = pow(max(dot(normal, half_vector), 0), mat.shininess) * (mat.specular_reflection * light_colour);
  vec4 primary = mat.emissive + diffuse + ambient_intensity;
  //Make sure the alphas are = 1
  primary.w = 1;
  specular.w = 1;

  return (primary * tex_colour) + specular;
}

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
    frag_colour += phong_spot(spots[i], mat, position, normal, view_dir, tex_colour);

  frag_colour.w = 1;
}