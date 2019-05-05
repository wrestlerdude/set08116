#version 440

uniform mat4 MVP;
uniform mat4 M;
uniform mat3 N;
uniform mat4 lightMVP[4];

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 10) in vec2 tex_coord_in;

layout(location = 0) out vec3 vertex_position;
layout(location = 1) out vec3 transformed_normal;
layout(location = 2) out vec2 tex_coord_out;
layout(location = 3) out vec4 vertex_light[4];

void main()
{
  gl_Position = MVP * vec4(position, 1);
  vertex_position = (M * vec4(position, 1)).xyz;
  transformed_normal = N * normal;
  tex_coord_out = tex_coord_in;
  
  for (int i = 0; i < 4; i++)
    vertex_light[i] = lightMVP[i] * vec4(position, 1.0);
}