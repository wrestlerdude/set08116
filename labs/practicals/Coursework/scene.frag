#version 440

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

uniform sampler2D tex;
uniform bool texture_exists;
uniform spot_light spots[4];
uniform material mat;
uniform vec3 eye_pos;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;

layout(location = 0) out vec4 frag_colour;

vec4 phong_spot(in spot_light spot, in material mat, in vec3 position,
                in vec3 normal, in vec3 view_dir, in vec4 tex_colour) {
  vec3 light_dir = normalize(spot.position - position);
  float d = distance(spot.position, position);
  float c = 1 / (spot.constant + (spot.linear * d) + (spot.quadratic * (d * d)));
  float intensity = pow(max(dot(-spot.direction, light_dir),0), spot.power);
  vec4 light_colour = (c * intensity) * spot.light_colour;
  vec4 diffuse = max(dot(normal, light_dir), 0) * (max.diffuse_reflection * light_colour);
  vec3 half_vector = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(normal, half_vector), 0), mat.shininess) * (mat.specular_reflection * light_colour);
  vec4 primary = mat.emissive + diffuse;
  primary.w = 1;
  specular.w = 1;
  return (primary * tex_colour) + specular;
}

void main() {
  vec4 tex_colour;
  vec4 frag_colour = vec4(0, 0, 0, 0);

  if (texture_exists)
    tex_colour = texture(tex, tex_coord);
  else
    tex_colour = vec4(1, 1, 1, 1);

  vec3 view_dir = normalize(eye_pos - position);

  for (int i = 0; i < 4; i++)
    frag_colour += phong_spot(spots[i], mat, position, normal, view_dir, tex_colour);
  frag_colour.w = 1;
}