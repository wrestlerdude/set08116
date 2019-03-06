#version 440

// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;
// Material colour
uniform vec4 material_colour;
// Light colour
uniform vec4 light_colour;
// Direction of the light
uniform vec3 light_dir;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;

// Outgoing vertex colour
layout(location = 0) out vec4 vertex_colour;

void main() {
  gl_Position = MVP * vec4(position, 1.0f);
  vec3 transformed_normal = N * normal;
  // Calculate k
  float k = max(dot(transformed_normal, light_dir), 0.0f);
  // Calculate diffuse
  vec4 diffuse = k * (material_colour * light_colour);

  // Ensure alpha is 1
  diffuse.w = 1.0f;

  // Output vertex colour - just diffuse
  vertex_colour = diffuse;
}