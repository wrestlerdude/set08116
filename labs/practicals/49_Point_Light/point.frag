#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light point;
// Material for the object
uniform material mat;
// Eye position
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // Get distance between point light and vertex
  float d = distance(point.position, position);
  // Calculate attenuation factor
  float c = 1 / (point.constant + (point.linear * d) + (point.quadratic * (d * d)));
  // Calculate light colour
  vec4 light_colour = c * point.light_colour;
  // Calculate light dir
  vec3 light_dir = normalize(point.position - position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 diffuse = max(dot(normal, light_dir), 0) * (mat.diffuse_reflection * light_colour);
  vec3 view_dir = normalize(eye_pos - position);
  vec3 half_vector = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(normal, half_vector), 0), mat.shininess) * (mat.specular_reflection * light_colour);
  vec4 texture_colour = texture(tex, tex_coord);
  vec4 primary = mat.emissive + diffuse;
  primary.w = 1;
  specular.w = 1;
  
  colour = (primary * texture_colour) + specular;
}