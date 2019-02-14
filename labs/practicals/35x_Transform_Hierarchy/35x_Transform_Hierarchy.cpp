#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

std::array<mesh, 3> meshes;
std::array<texture, 3> textures;
effect eff;
mesh plane_mesh;
texture plane_tex;
target_camera cam;

bool load_content() {
  // Create plane mesh
  plane_mesh = mesh(geometry_builder::create_plane());
	plane_mesh.get_transform().position -= vec3(0, 2, 0);
  // Create Three Identical Box Meshes
	for (auto &m : meshes)
		m = mesh(geometry_builder::create_box());
	meshes[0].get_transform().position = vec3(1, 0, 0);
	meshes[1].get_transform().position = vec3(0, 0, 1);
	meshes[2].get_transform().position = vec3(0, 1, 0);

  // Load texture
  plane_tex = texture("textures/snow.jpg");
  textures[0] = texture("textures/check_2.png");
  textures[1] = texture("textures/check_4.png");
  textures[2] = texture("textures/check_5.png");

  // Load in shaders
  eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
  eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(8.0f, 8.0f, 8.0f));
  cam.set_target(vec3(0, 1.0f, 0));
  auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
  cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
  return true;
}

bool update(float delta_time) {
	meshes[0].get_transform().rotate(vec3(0, delta_time, 0));
	meshes[1].get_transform().rotate(vec3(0, 0, delta_time));
	meshes[2].get_transform().rotate(vec3(0, delta_time, 0));
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Super effecient render loop, notice the things we only have to do once, rather than in a loop
  // Bind effect
  renderer::bind(eff);
  // Get PV
  const auto PV = cam.get_projection() * cam.get_view();
  // Set the texture value for the shader here
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // Find the lcoation for the MVP uniform
  const auto loc = eff.get_uniform_location("MVP");

  // Render meshes
  for (size_t i = 0; i < meshes.size(); i++) {
		auto M = meshes[i].get_transform().get_transform_matrix();
    // Apply the heirarchy chain
    for (size_t j = i; j > 0; j--) {
      M = meshes[j - 1].get_transform().get_transform_matrix() * M;
    }

    // Set MVP matrix uniform
    glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(PV * M));
    // Bind texture to renderer
    renderer::bind(textures[i], 0);
    // Render mesh
    renderer::render(meshes[i]);
  }

  // Render floor
  glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(PV * plane_mesh.get_transform().get_transform_matrix()));
  // Bind floor texture
  renderer::bind(plane_tex, 0);
  // Render floor
  renderer::render(plane_mesh);
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