#version 440

uniform mat4 MVP;
uniform mat4 lightMVP[5];
uniform mat4 M;
uniform mat3 N;
uniform vec3 eye_pos;
uniform bool env_map;
uniform bool parallax;
uniform bool normal_b;

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 binormal;
layout (location = 4) in vec3 tangent;
layout (location = 10) in vec2 tex_coord_in;

layout(location = 0) out vec3 vertex_position;
layout(location = 1) out vec3 transformed_normal;
layout(location = 2) out vec3 tex_coord_out;
layout(location = 3) out vec4 vertex_light[5];
layout(location = 8) out vec3 transformed_tangent;
layout(location = 9) out vec3 transformed_binormal;

void main()
{
  //Set position of vertex
  gl_Position = MVP * vec4(position, 1);
  vertex_position = (M * vec4(position, 1)).xyz;
  //Multiply normal by transform
  transformed_normal = N * normal;

  if (env_map) {
    vec3 transformed_position = (M * vec4(position, 1.0)).xyz;
    tex_coord_out = normalize(reflect(transformed_position - eye_pos, transformed_normal));
  }
  else
    tex_coord_out = vec3(tex_coord_in, 0);
  
  for (int i = 0; i < lightMVP.length(); i++)
    vertex_light[i] = lightMVP[i] * vec4(position, 1.0);

    //Transform binormal and tangent
    if (normal_b || parallax) {
      transformed_binormal = N * binormal;
      transformed_tangent = N * tangent;
    }
}