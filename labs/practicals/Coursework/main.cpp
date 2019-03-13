#include <string>
#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
array<texture, 4> textures;
vector<spot_light> spots(4);
effect eff;
free_camera free_cam;
target_camera target_cam;
vec2 uv_scroll;
double run_time = 0.0;
double cursor_x = 0.0;
double cursor_y = 0.0;
bool is_free = true;

bool initialise() {
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
  glEnable(GL_MULTISAMPLE);
  glClearColor(0, 0, 0.141, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  return true;
}

bool load_content() {
  meshes["warp_stone"] = mesh(geometry_builder::create_box());
  meshes["dissolve_stone"] = mesh(geometry_builder::create_box());
  meshes["pedestal"] = mesh(geometry("D:/cwk-cache/pedestal.obj"));
  
  meshes["warp_stone"].get_material().set_shininess(50);
  meshes["dissolve_stone"].get_material().set_shininess(25);
  meshes["pedestal"].get_material().set_shininess(20);

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

  seperation = 0;
  for (size_t i = 0; i < 4; i++) {
    spots[i].set_position(vec3(seperation, 30, 0));
    spots[i].set_light_colour(vec4(1, 1, 0.7, 1));
    spots[i].set_direction(vec3(0, -1, 0));
    spots[i].set_range(50);
    spots[i].set_power(40);
    seperation -= 24;
  }

  // Load in shaders
  eff.add_shader("D:/Src/C/set08116-coursework/labs/practicals/Coursework/interp.vert", GL_VERTEX_SHADER);
  eff.add_shader("D:/Src/C/set08116-coursework/labs/practicals/Coursework/scene.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();

  //Apply textures with Anisotropic filtering and generate mipmaps
  //Wood texture
  textures[0] = texture("D:/Textures/Wood/wood-squares.jpg", true, true);
  //Marble texture
  textures[1] = texture("D:/Textures/Marble/stone.jpg", true, true);
  //Blue stone texture
  textures[2] = texture("D:/Textures/Blue/blue-stone.jpg", true, true);
  //Blend map for linen red
  textures[3] = texture("D:/Textures/Blend/passive-blend.png");

  // Setfree_camera properties
  free_cam.set_position(vec3(-35.0f, 10.0f, 40.0f));
  target_cam.set_position(vec3(0, 2.5, 15));
  target_cam.set_target(vec3(0, 0, 0));
  // ~ around 70 degrees fov
  free_cam.set_projection(1.222f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
  target_cam.set_projection(1.222f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  // The ratio of pixels to rotation - remember the fov
  static double ratio_width = 1.222f / static_cast<float>(renderer::get_screen_width());
  static double ratio_height = (1.222f * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

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

  //Target camera positioning
  if (glfwGetKey(renderer::get_window(), '1')) {
    target_cam.set_position(vec3(0, 2.5, 15));
    target_cam.set_target(vec3(0, 0, 0));
  } if (glfwGetKey(renderer::get_window(), '2')) {
    target_cam.set_position(vec3(-24, 2.5, 15));
    target_cam.set_target(vec3(-24, 0, 0));
  } if (glfwGetKey(renderer::get_window(), '3')) {
    target_cam.set_position(vec3(-48, 2.5, 15));
    target_cam.set_target(vec3(-48, 0, 0));
  } if (glfwGetKey(renderer::get_window(), '4')) {
    target_cam.set_position(vec3(-72, 2.5, 15));
    target_cam.set_target(vec3(-72, 0, 0));
  }

  static float old_run_time;
  if (glfwGetKey(renderer::get_window(), 'P') && (((run_time - old_run_time) >= 0.5) || old_run_time == 0)) {
    is_free = !is_free;
    old_run_time = run_time;
  }
  //Manipulate transform of the warp_stone
  float factor = (1.0 + cosf(run_time)) * 2;
  meshes["warp_stone"].get_transform().scale = vec3(pow(factor, 1.5), sqrtf(factor), 2);
  meshes["warp_stone"].get_transform().rotate(vec3(quarter_pi<float>(), quarter_pi<float>(), 0.0f) * delta_time);
  
  // Update the main camera
  if (is_free) {
    // Move free_cam
    free_cam.move(movement);
    free_cam.update(delta_time);
    // Update cursor pos
    cursor_x = new_x;
    cursor_y = new_y;
  } else
    target_cam.update(delta_time);

  run_time += delta_time;
  uv_scroll += vec2(0, delta_time * 0.05);
  return true;
}

bool render() {
  //Optimization if using target cam
  mat4 V, P;
  if (!is_free) {
    V = target_cam.get_view();
    P = target_cam.get_projection();
  }
  // Render meshes
  for (auto &e : meshes) {
    auto m = e.second;

    // Bind effect
    renderer::bind(eff);

    // Create MVP matrix
    auto M = m.get_transform().get_transform_matrix();
    //Have to update VP every frame due to camera being movable
    if (is_free) {
      V = free_cam.get_view();
      P = free_cam.get_projection();
    }
    auto MVP = P * V * M;

    // Set MVP matrix uniform
    glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
    // Set Normal matrix uniform - get vertex normal transform
    glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
    // Set M matrix uniform - convert vertices to world space
    glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

    //Bind texture to renderer and pass to shader
    bool dissolve_enabled = false;
    bool texture_exists = true;
    if (e.first == "warp_stone")
      renderer::bind(textures[0], 0);
    else if (e.first == "dissolve_stone") {
      dissolve_enabled = true;
      renderer::bind(textures[2], 0);
      renderer::bind(textures[3], 1);
      glUniform1i(eff.get_uniform_location("dissolve"), 1);
      glUniform1f(eff.get_uniform_location("dissolve_factor"), (0.75 * sinf(2 * run_time - 0.75)) + 0.5);
      glUniform2fv(eff.get_uniform_location("UV_SCROLL"), 1, value_ptr(uv_scroll));
    } else if (e.first.find("pedestal") != string::npos)
      renderer::bind(textures[1], 0);
    else
      texture_exists = false;

    renderer::bind(m.get_material(), "mat");
    renderer::bind(spots, "spots");

    glUniform1i(eff.get_uniform_location("dissolve_enabled"), dissolve_enabled);
    glUniform1i(eff.get_uniform_location("texture_exists"), texture_exists);
    glUniform1i(eff.get_uniform_location("tex"), 0);
    glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(free_cam.get_position()));
    glUniform1f(eff.get_uniform_location("ambient_intensity"), 0.1);

    renderer::render(m);
  }

  return true;
}

void main() {
  glfwWindowHint(GLFW_SAMPLES, 8);
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