#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect eff;
texture tex;
target_camera cam;

bool load_content() {
  // Create meshes
  meshes["plane"] = mesh(geometry_builder::create_plane());
	meshes["box"] = mesh(geometry_builder::create_box());
	meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["disk"] = mesh(geometry_builder::create_disk(20));
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1, 5));
	// Set mesh transforms
	meshes["box"].get_transform().scale = vec3(5, 5, 5);
	meshes["box"].get_transform().position = vec3(-10, 2.5f, -30);
	meshes["tetra"].get_transform().scale = vec3(4, 4, 4);
	meshes["tetra"].get_transform().position = vec3(-30, 10, -10);
	meshes["pyramid"].get_transform().scale = vec3(5, 5, 5);
	meshes["pyramid"].get_transform().position = vec3(-10, 7.5f, -30);
	meshes["disk"].get_transform().scale = vec3(3, 1, 3);
	meshes["disk"].get_transform().position = vec3(-10, 11.5f, -30);
  meshes["disk"].get_transform().rotate(vec3(pi<float>() / 2.0f, 0.0f, 0.0f));
	meshes["cylinder"].get_transform().scale = vec3(5, 5, 5);
	meshes["cylinder"].get_transform().position = vec3(-25, 2.5f, -25);
	meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	meshes["sphere"].get_transform().position = vec3(-25, 10, -25);
	meshes["torus"].get_transform().position = vec3(-25, 10, -25);
	meshes["torus"].get_transform().rotate(vec3(pi<float>() / 2.0f, 0.0f, 0.0f));


  // Load texture
  tex = texture("textures/checker.png");

  // Load in shaders
  eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
  eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(10.0f, 10.0f, 10.0f));
  cam.set_target(vec3(-100.0f, 0.0f, -100.0f));
  auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
  cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Render meshes
  for (auto &e : meshes) {
    auto m = e.second;
    // Bind effect
    renderer::bind(eff);
    // Create MVP matrix
    auto M = m.get_transform().get_transform_matrix();
    auto V = cam.get_view();
    auto P = cam.get_projection();
    auto MVP = P * V * M;
    // Set MVP matrix uniform
    glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		renderer::bind(tex, 0);
		glUniform1i(eff.get_uniform_location("tex"), 0);
    
		// Render mesh
    renderer::render(m);
  }

  return true;
}

void main() {
  // Create application
  app application("35_Geometry_Builder");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}