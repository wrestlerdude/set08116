#include <string>
#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
array<texture, 2> textures;
effect eff;
free_camera cam;
double run_time = 0.0;
double cursor_x = 0.0;
double cursor_y = 0.0;

bool initialise() {
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

  return true;
}

bool load_content() {

  meshes["magic_stone"] = mesh(geometry_builder::create_box());
  meshes["pedastel"] = mesh(geometry("Coursework/pedastel.obj"));
  meshes["pedastel"].get_transform().scale = vec3(6, 6, 6);

  meshes["pedastel"].get_transform().position = vec3(-1, -20, -4.5);
  meshes["pedastel2"] = mesh(meshes["pedastel"]);
  meshes["pedastel3"] = mesh(meshes["pedastel"]);
  meshes["pedastel4"] = mesh(meshes["pedastel"]);

  //Space out pedastels
  int seperation = 0;
  for (auto &e : meshes) {
    if (e.first.find("pedastel") != string::npos)
    {
      e.second.get_transform().position -= vec3(seperation, 0, 0);
      seperation += 24;
    }
  }

  // Load in shaders
  eff.add_shader("Coursework/interp.vert", GL_VERTEX_SHADER);
  eff.add_shader("Coursework/scene.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();

  //Apply textures with Anisotropic filtering and generate mipmaps
  //Sandstone texture
  textures[0] = texture("Coursework/purple-stone.jpg", true, true);
  textures[1] = texture("Coursework/light-grey.png");

  // Set camera properties
  cam.set_position(vec3(0.0f, 5.0f, 15.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  // ~ around 70 degrees fov
  cam.set_projection(1.222f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  // The ratio of pixels to rotation - remember the fov
  static double ratio_width = 1.222f / static_cast<float>(renderer::get_screen_width());
  static double ratio_height =
    (1.222f *
    (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
    static_cast<float>(renderer::get_screen_height());

  // Calculate delta of cursor positions from last frame
  double new_x, new_y;
  glfwGetCursorPos(renderer::get_window(), &new_x, &new_y);
  double delta_x = new_x - cursor_x;
  double delta_y = new_y - cursor_y;

  // Multiply deltas by ratios - gets actual change in orientation
  delta_x *= ratio_width;
  delta_y *= ratio_height;

  // Rotate cameras by delta
  cam.rotate(delta_x, -delta_y);

  // Use keyboard to move the camera - WSAD
  vec3 movement = vec3(0, 0, 0);
  if (glfwGetKey(renderer::get_window(), 'W'))
    movement.z += 0.25f;
  if (glfwGetKey(renderer::get_window(), 'S'))
    movement.z -= 0.25f;
  if (glfwGetKey(renderer::get_window(), 'A'))
    movement.x -= 0.25f;
  if (glfwGetKey(renderer::get_window(), 'D'))
    movement.x += 0.25f;

  // Move camera
  cam.move(movement);
  // Update the camera
  cam.update(delta_time);
  // Update cursor pos
  cursor_x = new_x;
  cursor_y = new_y;

  run_time += delta_time;
  //Manipulate transform of the magic_stone
  float factor = (1.0 + cosf(run_time)) * 2;
  meshes["magic_stone"].get_transform().scale = vec3(pow(factor, 1.5), sqrtf(factor), 2);
  meshes["magic_stone"].get_transform().rotate(vec3(quarter_pi<float>(), quarter_pi<float>(), 0.0f) * delta_time);

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

    //Bind texture to renderer and pass to shader
    if (e.first == "magic_stone")
      renderer::bind(textures[0], 0);
    else
      renderer::bind(textures[1], 0);
    
    glUniform1i(eff.get_uniform_location("tex"), 0);

    // Render mesh
    renderer::render(m);
  }

  return true;
}

void main() {
  // Create application
  app application("Coursework");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_initialise(initialise);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}