#version 440

// Model view projection matrix
uniform mat4 MVP;

layout (location = 0) in vec3 position;

layout (location = 0) out vec4 vetex_colour;

void main()
{
	// Calculate screen position of vertex
	gl_Position = MVP * vec4(position, 1.0);
	// Output colour to the fragment shader
	vetex_colour = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}