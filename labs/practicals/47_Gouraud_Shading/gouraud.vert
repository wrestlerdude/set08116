#version 440

// A directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;
// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Incoming texture coordinate
layout(location = 10) in vec2 tex_coord_in;

// Outgoing primary colour
layout(location = 0) out vec4 primary;
// Outgoing secondary colour
layout(location = 1) out vec4 secondary;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord_out;

void main() {
  // Calculate position
  gl_Position = MVP * vec4(position, 1);
  // Calculate ambient component
  vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;
  // Transform the normal
  vec3 transformed_normal = N * normal;
  // Calculate diffuse
  vec4 diffuse = max(dot(transformed_normal, light.light_dir), 0) * (mat.diffuse_reflection * light.light_colour);
  // Calculate world position of vertex
  vec4 world_pos = M * vec4(position, 1);
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - world_pos.xyz);
  // Calculate half vector between view_dir and light_dir
  vec3 half_vector = normalize(light.light_dir + view_dir);
  // Calculate specular component
  vec4 specular = pow(max(dot(transformed_normal, half_vector), 0.0f), mat.shininess) * (mat.specular_reflection * light.light_colour);
  // Set primary
  primary = mat.emissive + ambient + diffuse;
  // Set secondary
  secondary = specular;
  // Ensure primary and secondary alphas are 1
  primary.w = 1;
  secondary.w = 1;
  tex_coord_out = tex_coord_in;
}