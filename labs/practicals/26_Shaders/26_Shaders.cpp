#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

mesh m;
effect eff;
target_camera cam;
#define NapierRed 0.765f, 0.082f, 0.196f

bool load_content() {
  // Construct geometry object
  geometry geom;
  // Create triangle data
  // Positions
  vector<vec3> positions{vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f)};
  // Add to the geometry
  geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);

  // Create mesh object
  m = mesh(geom);

  // Load in colour shaders
  eff.add_shader("26_Shaders/colour.vert", GL_VERTEX_SHADER);
  eff.add_shader("26_Shaders/colour.frag", GL_FRAGMENT_SHADER);
  eff.build();

  // Set camera properties
  cam.set_position(vec3(10.0f, 10.0f, 10.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
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
  // Bind effect
  renderer::bind(eff);
  // Create MVP matrix
  auto M = m.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), // Location of uniform
                     1,                               // Number of values - 1 mat4
                     GL_FALSE,                        // Transpose the matrix?
                     value_ptr(MVP));                 // Pointer to matrix data
  glUniform4fv(eff.get_uniform_location("colour"), 1, value_ptr(vec4(NapierRed, 1.0f)));

  // Render the mesh
  renderer::render(m);

  return true;
}

void main() {
  // Create application
  app application("26_Shaders");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}