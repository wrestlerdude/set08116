#include <string>
#include <glm\glm.hpp>
#include <graphics_framework.h>


using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
array<texture, 2> textures;
vector<spot_light> spots(4);
directional_light sun;
effect eff;
free_camera free_cam;
//target_camera;
double run_time = 0.0;
double cursor_x = 0.0;
double cursor_y = 0.0;

bool initialise() {
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

  return true;
}

bool load_content() {

  meshes["warp_stone"] = mesh(geometry_builder::create_box());
  meshes["dissolve_stone"] = mesh(geometry_builder::create_box());
  meshes["pedestal"] = mesh(geometry("Coursework/pedestal.obj"));
  
  meshes["warp_stone"].get_material().set_shininess(50);
  meshes["dissolve_stone"].get_material().set_shininess(200);
  meshes["pedestal"].get_material().set_shininess(1000);

  meshes["dissolve_stone"].get_transform().position = vec3(-24, 0, 0);
  meshes["dissolve_stone"].get_transform().scale = vec3(2.5, 2.5, 2.5);

  meshes["pedestal"].get_transform().position = vec3(0, -5, 0);
  meshes["pedestal"].get_transform().scale = vec3(0.3, 0.3, 0.3);

  meshes["pedestal2"] = mesh(meshes["pedestal"]);
  meshes["pedestal3"] = mesh(meshes["pedestal"]);
  meshes["pedestal4"] = mesh(meshes["pedestal"]);

  int seperation = 0;
  for (auto &e : meshes) {
    material mat = e.second.get_material();
    if (e.first.find("pedestal") != string::npos) {
      e.second.get_transform().position -= vec3(seperation, 0, 0);
      seperation += 24;
    }
    mat.set_emissive(vec4(0, 0, 0, 1));
    mat.set_specular(vec4(1, 1, 1, 1));
  }

  sun.set_ambient_intensity(vec4(0.1, 0.1, 0.1, 1));
  sun.set_light_colour(vec4(1, 1, 1, 1));
  sun.set_direction(normalize(vec3(0, -1, 1)));

  // Load in shaders
  eff.add_shader("D:/Src/C/set08116/labs/practicals/Coursework/interp.vert", GL_VERTEX_SHADER);
  eff.add_shader("D:/Src/C/set08116/labs/practicals/Coursework/scene.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();

  //Apply textures with Anisotropic filtering and generate mipmaps
  //Gem texture
  textures[0] = texture("Coursework/wood-squares.jpg", true, true);
  //Marble texture
  textures[1] = texture("D:/Textures/Marble/black-stone.jpg", true, true);

  seperation = 0;
  for (size_t i = 0; i < 4; i++) {
    spots[i].set_position(vec3(seperation, 10, 0));
    spots[i].set_light_colour(vec4(1, 1, 0.5, 1));
    spots[i].set_direction(vec3(0, -1, 0));
    spots[i].set_range(8);
    spots[i].set_power(1);
    seperation -= 24;
  }

  // Setfree_camera properties
  free_cam.set_position(vec3(-35.0f, 10.0f, 40.0f));
  free_cam.set_target(vec3(-30.0f, -5.0f, 0.0f));
  // ~ around 70 degrees fov
  free_cam.set_projection(1.222f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
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

  // Rotate free_cam by delta
  free_cam.rotate(delta_x, -delta_y);

  // Use keyboard to move thefree_camera - WSAD
  vec3 movement = vec3(0, 0, 0);
  if (glfwGetKey(renderer::get_window(), 'W'))
    movement.z += 0.25f;
  if (glfwGetKey(renderer::get_window(), 'S'))
    movement.z -= 0.25f;
  if (glfwGetKey(renderer::get_window(), 'A'))
    movement.x -= 0.25f;
  if (glfwGetKey(renderer::get_window(), 'D'))
    movement.x += 0.25f;

  //Targetfree_camera positioning
  if (glfwGetKey(renderer::get_window(), '4'))
   free_cam.set_position(vec3(0, 0, 12.5));
  if (glfwGetKey(renderer::get_window(), '3'))
   free_cam.set_position(vec3(-24, 0, 12.5));
  if (glfwGetKey(renderer::get_window(), '2'))
   free_cam.set_position(vec3(-48, 0, 12.5));
  if (glfwGetKey(renderer::get_window(), '1'))
   free_cam.set_position(vec3(-72, 0, 12.5));

  // Move free_cam
  free_cam.move(movement);
  // Update the free_cam
  free_cam.update(delta_time);
  // Update cursor pos
  cursor_x = new_x;
  cursor_y = new_y;

  run_time += delta_time;
  //Manipulate transform of the warp_stone
  float factor = (1.0 + cosf(run_time)) * 2;
  meshes["warp_stone"].get_transform().scale = vec3(pow(factor, 1.5), sqrtf(factor), 2);
  meshes["warp_stone"].get_transform().rotate(vec3(quarter_pi<float>(), quarter_pi<float>(), 0.0f) * delta_time);

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
    auto V = free_cam.get_view();
    auto P = free_cam.get_projection();
    auto MVP = P * V * M;

    // Set MVP matrix uniform
    glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
    // Set Normal matrix uniform - get vertex normal transform
    glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
    // Set M matrix uniform - convert vertices to world space
    glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

    //Bind texture to renderer and pass to shader
    if (e.first == "warp_stone")
      renderer::bind(textures[0], 0);
    else  
      renderer::bind(textures[1], 0);

    renderer::bind(m.get_material(), "mat");
    renderer::bind(spots, "spots");
    renderer::bind(sun, "sun");

    glUniform1i(eff.get_uniform_location("tex"), 0);
    glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(free_cam.get_position()));
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