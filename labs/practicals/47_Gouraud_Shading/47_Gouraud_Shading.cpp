#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect eff;
texture tex;
target_camera cam;
directional_light light;

bool load_content() {
  // Create plane mesh
  meshes["plane"] = mesh(geometry_builder::create_plane());

  // Create scene
  meshes["box"] = mesh(geometry_builder::create_box());
  meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
  meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
  meshes["disk"] = mesh(geometry_builder::create_disk(20));
  meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
  meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
  meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

  // Transform objects
  meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));
  meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
  meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));
  meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));
  meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
  meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
  meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
  meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
  meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
  meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
  meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
  meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));

  // *********************************
  // Set materials
  // - all emissive is black
  // - all specular is white
  // - all shininess is 25
  // Red box
  meshes["box"].get_material().set_diffuse(vec4(1, 0, 0, 1));
  // Green tetra
  meshes["tetra"].get_material().set_diffuse(vec4(0, 1, 0, 1));
  // Blue pyramid
  meshes["pyramid"].get_material().set_diffuse(vec4(0, 0, 1, 1));
  // Yellow disk
  meshes["disk"].get_material().set_diffuse(vec4(1, 1, 0, 1));
  // Magenta cylinder
  meshes["cylinder"].get_material().set_diffuse(vec4(1, 0, 1, 1));
  // Cyan sphere
  meshes["sphere"].get_material().set_diffuse(vec4(0, 1, 1, 1));
  // White torus
  meshes["torus"].get_material().set_diffuse(vec4(1, 1, 1, 1));
  
  for (auto &e : meshes)
  {
    material mat = e.second.get_material();
    mat.set_emissive(vec4(0, 0, 0, 1));
    mat.set_specular(vec4(1, 1, 1, 1));
    mat.set_shininess(25);
  }

  // Load texture
  tex = texture("textures/checker.png");

  // Set lighting values
  // ambient intensity (0.3, 0.3, 0.3)
  light.set_ambient_intensity(vec4(0.3, 0.3, 0.3, 1));
  // Light colour white
  light.set_ambient_intensity(vec4(1, 1, 1, 1));
  // Light direction (1.0, 1.0, -1.0)
  light.set_direction(vec3(1, 1, -1));

  // Load in shaders
  eff.add_shader("47_Gouraud_Shading/gouraud.vert", GL_VERTEX_SHADER);
  eff.add_shader("47_Gouraud_Shading/gouraud.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(50.0f, 10.0f, 50.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  if (glfwGetKey(renderer::get_window(), '1')) {
    cam.set_position(vec3(50, 10, 50));
  }
  if (glfwGetKey(renderer::get_window(), '2')) {
    cam.set_position(vec3(-50, 10, 50));
  }
  if (glfwGetKey(renderer::get_window(), '3')) {
    cam.set_position(vec3(-50, 10, -50));
  }
  if (glfwGetKey(renderer::get_window(), '4')) {
    cam.set_position(vec3(50, 10, -50));
  }

  // Rotate the sphere
  meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

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
    // Set M matrix uniform
    glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
    // Set N matrix uniform - remember - 3x3 matrix
    glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
    // Bind material
    renderer::bind(m.get_material(), "mat");
    // Bind light
    renderer::bind(light, "light");
    // Bind texture
    renderer::bind(tex, 0);
    // Set tex uniform
    glUniform1i(eff.get_uniform_location("tex"), 0);
    // Set eye position - Get this from active camera
    glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));
    // Render mesh
    renderer::render(m);
  }

  return true;
}

void main() {
  // Create application
  app application("47_Gouraud_Shading");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}