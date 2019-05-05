#include <string>
#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
mesh skybox;
cubemap cube_map;
array<texture, 9> textures;
vector<spot_light> spots(5);
vector<point_light> points(6);
vector<shadow_map> shadows(5);
effect eff, sky_eff, vignette_eff, shadow_eff;
free_camera free_cam;
target_camera target_cam;
frame_buffer frame;
geometry screen_quad;
double run_time = 0.0;
double cursor_x = 0.0;
double cursor_y = 0.0;
bool is_free = true;

bool initialise() {
  //Set resolution to 1600x900
  renderer::set_screen_dimensions(1600, 900);
  //Capture cursor
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
  //Enable MSAA
  glEnable(GL_MULTISAMPLE);
  //Background colour: Navy Blue
  glClearColor(0, 0, 0.141, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  return true;
}

bool load_content() {
  // Create frame buffer - use screen width and height
  frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  // Create screen quad
  vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f),
                         vec3(1.0f, 1.0f, 0.0f) };
  vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
  screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
  screen_quad.set_type(GL_TRIANGLE_STRIP);

  skybox = mesh(geometry_builder::create_box());
  skybox.get_transform().scale = vec3(100, 100, 100);
  
  meshes["warp_stone"] = mesh(geometry_builder::create_box());
  meshes["dissolve_stone"] = mesh(geometry_builder::create_box());
  meshes["pedestal"] = mesh(geometry("res/models/pedestal.obj"));
  meshes["lamp"] = mesh(geometry("res/models/spotlight.obj"));
  meshes["skeleton"] = mesh(geometry("res/models/skeleton.obj"));
  meshes["amethyst"] = mesh(geometry("res/models/amethyst.obj"));
  meshes["parallax_stone"] = mesh(geometry_builder::create_box());

  meshes["lamp"].get_transform().position = vec3(0, 15, 0);
  meshes["lamp"].get_transform().scale = vec3(2, 2, 2);
  meshes["amethyst"].get_transform().position = vec3(-48, 0, 0);
  meshes["amethyst"].get_transform().scale = vec3(2.5, 2.5, 2.5);
  meshes["dissolve_stone"].get_transform().position = vec3(-24, 0, 0);
  meshes["dissolve_stone"].get_transform().scale = vec3(4, 4, 4);
  meshes["skeleton"].get_transform().position = vec3(-72, -5, 0);
  meshes["skeleton"].get_transform().scale = vec3(1.4, 1.4, 1.4);
  meshes["pedestal"].get_transform().position = vec3(0, -5, 0);
  meshes["pedestal"].get_transform().scale = vec3(0.3, 0.3, 0.3);
  meshes["parallax_stone"].get_transform().scale = vec3(4, 4, 4);
  meshes["parallax_stone"].get_transform().position = vec3(-96, 0, 0);

  meshes["warp_stone"].get_material().set_shininess(3);
  meshes["dissolve_stone"].get_material().set_shininess(1.5);
  meshes["pedestal"].get_material().set_shininess(10);
  meshes["skeleton"].get_material().set_shininess(5);
  meshes["amethyst"].get_material().set_shininess(2);
  meshes["parallax_stone"].get_material().set_shininess(5);

  meshes["pedestal2"] = mesh(meshes["pedestal"]);
  meshes["pedestal3"] = mesh(meshes["pedestal"]);
  meshes["pedestal4"] = mesh(meshes["pedestal"]);
  meshes["pedestal5"] = mesh(meshes["pedestal"]);
  meshes["lamp2"] = mesh(meshes["lamp"]);
  meshes["lamp3"] = mesh(meshes["lamp"]);
  meshes["lamp4"] = mesh(meshes["lamp"]);
  meshes["lamp5"] = mesh(meshes["lamp"]);

  //Placement of pedastels and lamp models
  int seperation[2] = { 0 };
  for (auto &e : meshes) {
    material mat = e.second.get_material();
    if (e.first.find("pedestal") != string::npos) {
      e.second.get_transform().position -= vec3(seperation[0], 0, 0);
      seperation[0] += 24;
    }
    else if (e.first.find("lamp") != string::npos) {
      e.second.get_transform().position -= vec3(seperation[1], 0, 0);
      seperation[1] += 24;
    }

    mat.set_emissive(vec4(0, 0, 0, 1));
    mat.set_specular(vec4(1, 1, 1, 1));
  }

  //Placement and initial values of lights
  seperation[0] = 0;
  for (size_t i = 0; i < 5; i++) {
    spots[i].set_position(vec3(seperation[0], 13.5, 0));
    spots[i].set_light_colour(vec4(1, 1, 0.85, 1));
    spots[i].set_direction(vec3(0, -1, 0));
    spots[i].set_range(20);
    spots[i].set_power(10);
    points[i].set_position(vec3(seperation[0], 13.5, 0.5));
    points[i].set_light_colour(vec4(1, 1, 0.7, 1));
    points[i].set_range(4);
    seperation[0] -= 24;

    shadows[i] = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
    shadows[i].light_position = spots[i].get_position();
    shadows[i].light_dir = spots[i].get_direction();
  }

  //6th point is amethyst sparkle
  points[5].set_light_colour(vec4(1, 0, 1, 1));
  points[5].set_range(5);
  points[5].set_position(vec3(-48, 2, 0));

  // Load in shaders
  eff.add_shader("res/shaders/main.vert", GL_VERTEX_SHADER);
  eff.add_shader(vector<string>{"res/shaders/main.frag", "res/shaders/point.frag", "res/shaders/spot.frag", "res/shaders/shadow.frag", "res/shaders/normal.frag"}, GL_FRAGMENT_SHADER);
  sky_eff.add_shader("res/shaders/skybox.vert", GL_VERTEX_SHADER);
  sky_eff.add_shader("res/shaders/skybox.frag", GL_FRAGMENT_SHADER);
  vignette_eff.add_shader("res/shaders/basic_textured.vert", GL_VERTEX_SHADER);
  vignette_eff.add_shader("res/shaders/vignette.frag", GL_FRAGMENT_SHADER);
  //Possibly link more basic renders?
  shadow_eff.add_shader("res/shaders/basic_colour.vert", GL_VERTEX_SHADER);
  shadow_eff.add_shader("res/shaders/basic_colour.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();
  sky_eff.build();
  vignette_eff.build();
  shadow_eff.build();

  //Apply textures with Anisotropic filtering and generate mipmaps
  //Wood texture
  textures[0] = texture("res/textures/wood-squares.jpg", true, true);
  //Marble texture
  textures[1] = texture("res/textures/stone.jpg", true, true);
  //Metal floor texture
  textures[2] = texture("res/textures/158.jpg", true, true);
  textures[3] = texture("res/textures/158_norm.jpg", true, true);
  //Spotlight model texture
  textures[4] = texture("res/textures/rusty-light.jpg", true, true);
  //Bone texture
  textures[5] = texture("res/textures/bone.png");
  //Brick textures
  textures[6] = texture("res/textures/bricks2.jpg");
  textures[7] = texture("res/textures/bricks2_normal.jpg");
  textures[8] = texture("res/textures/bricks2_disp.jpg");

  array<string, 6> filenames = { "res/textures/miramar_ft.png", "res/textures/miramar_bk.png", "res/textures/miramar_up.png",
                                "res/textures/miramar_dn.png", "res/textures/miramar_rt.png", "res/textures/miramar_lf.png" };
  cube_map = cubemap(filenames);

  // Set camera properties
  free_cam.set_position(vec3(-35, 10, 40));
  target_cam.set_position(vec3(-72, 2.5, 15));
  target_cam.set_target(vec3(-72, 0, 0));
  // 1.222 ~ around 70 degrees fov
  free_cam.set_projection(1.222, renderer::get_screen_aspect(), 0.1, 1000);
  target_cam.set_projection(1.222, renderer::get_screen_aspect(), 0.1, 1000);

  return true;
}

bool update(float delta_time) {
  // The ratio of pixels to rotation
  static double ratio_width = 1.222 / static_cast<float>(renderer::get_screen_width());
  static double ratio_height = (1.222 * (static_cast<float>(renderer::get_screen_height())
                               / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

  // Calculate delta of cursor positions from last frame
  double new_x, new_y;
  glfwGetCursorPos(renderer::get_window(), &new_x, &new_y);
  double delta_x = new_x - cursor_x;
  double delta_y = new_y - cursor_y;

  // Multiply deltas by ratios - gets actual change in orientation
  delta_x *= ratio_width;
  delta_y *= ratio_height;

  // Rotate free_cam by delta
  if(is_free)
    free_cam.rotate(delta_x, -delta_y);

  // Use keyboard to move thefree_camera - WSAD
  vec3 movement = vec3(0, 0, 0);
  if (glfwGetKey(renderer::get_window(), 'W'))
    movement.z += 0.25;
  if (glfwGetKey(renderer::get_window(), 'S'))
    movement.z -= 0.25;
  if (glfwGetKey(renderer::get_window(), 'A'))
    movement.x -= 0.25;
  if (glfwGetKey(renderer::get_window(), 'D'))
    movement.x += 0.25;

  //Target camera positioning
  if (glfwGetKey(renderer::get_window(), '1')) {
    target_cam.set_position(vec3(-96, 2.5, 15));
    target_cam.set_target(vec3(-96, 0, 0));
  }
  if (glfwGetKey(renderer::get_window(), '2')) {
    target_cam.set_position(vec3(-72, 2.5, 15));
    target_cam.set_target(vec3(-72, 0, 0));
  }
  if (glfwGetKey(renderer::get_window(), '3')) {
    target_cam.set_position(vec3(-48, 2.5, 15));
    target_cam.set_target(vec3(-48, 0, 0));
  }
  if (glfwGetKey(renderer::get_window(), '4')) {
    target_cam.set_position(vec3(-24, 2.5, 15));
    target_cam.set_target(vec3(-24, 0, 0));
  }
  if (glfwGetKey(renderer::get_window(), '5')) {
    target_cam.set_position(vec3(0, 2.5, 15));
    target_cam.set_target(vec3(0, 0, 0));
  }

  //testing
  if (glfwGetKey(renderer::get_window(), 'L'))
    shadows[3].buffer->save("test.png");
  
  //Used so that there is delay between switches -> so switching every frame doesn't happen
  static float old_run_time;
  //Press P to toggle between free_cam and target_cam
  if (glfwGetKey(renderer::get_window(), 'P') && (((run_time - old_run_time) >= 0.5) || old_run_time == 0)) {
    is_free = !is_free;
    if (is_free)
      free_cam.set_position(vec3(-35.0, 10.0, 40.0));
    old_run_time = run_time;
  }
  //Manipulate transform of the warp_stone
  float factor = (1.0 + cosf(run_time)) * 2;
  meshes["warp_stone"].get_transform().scale = vec3(pow(factor, 1.5), sqrtf(factor), 2);
  meshes["warp_stone"].get_transform().rotate(vec3(quarter_pi<float>(), quarter_pi<float>(), 0.0f) * delta_time);
  //Rotate amethyst stone
  meshes["amethyst"].get_transform().rotate(vec3(0, half_pi<float>(), 0.0f) * delta_time);
  meshes["dissolve_stone"].get_transform().rotate(vec3(0.0, half_pi<float>(), 0.0) * delta_time);
  //Change colour diffuse of skeleton, wave range 0 to 1
  meshes["skeleton"].get_material().set_diffuse(vec4(0.5 * sinf(4 * run_time) + 0.5,
                                                     0.5 * cosf(5 * run_time) + 0.5,
                                                     0.5 * sinf(1.25 * run_time) + 0.5, 1));
  // Update the main camera
  if (is_free) {
    // Move free_cam
    free_cam.move(movement);
    free_cam.update(delta_time);
    // Update cursor pos
    cursor_x = new_x;
    cursor_y = new_y;
    skybox.get_transform().position = free_cam.get_position();
  }
  else {
    target_cam.update(delta_time);
    skybox.get_transform().position = target_cam.get_position();
  }

  run_time += delta_time;

  return true;
}

bool render() {
  camera* cam_ref;
  //Optimization for using target cam
  mat4 M, V, P, MVP;
  if (!is_free)
    cam_ref = &target_cam;
  else
    cam_ref = &free_cam;

  /*
    SHADOW MAP RENDER
  */
  //zNear 4 to increase depth buffer precision, zFar 20 to prevent clipping with no acne, FOV of 30 degrees matches pedastel perfectly.
  mat4 LightProjectionMat = perspective<float>(0.523599f, renderer::get_screen_aspect(), 4.0f, 20.0f);
  glCullFace(GL_FRONT);
  for (size_t i = 0; i < shadows.size(); i++) {
    renderer::set_render_target(shadows[i]);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderer::bind(shadow_eff);

    for (auto &e : meshes) {
      auto m = e.second;
      // Create MVP matrix
      auto M = m.get_transform().get_transform_matrix();
      // View matrix taken from shadow map
      auto V = shadows[i].get_view();
      auto MVP = LightProjectionMat * V * M;
      // Set MVP matrix uniform
      glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), // Location of uniform
        1,                                      // Number of values - 1 mat4
        GL_FALSE,                               // Transpose the matrix?
        value_ptr(MVP));                        // Pointer to matrix data
      // Render mesh
      renderer::render(m);
    }
  }
  /*
    SKYBOX RENDER
  */
  renderer::set_render_target(frame);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  V = cam_ref->get_view();
  P = cam_ref->get_projection();

  glCullFace(GL_FRONT);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  renderer::bind(sky_eff);
  M = skybox.get_transform().get_transform_matrix();
  MVP = P * V * M;
  glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  renderer::bind(cube_map, 0);
  glUniform1i(sky_eff.get_uniform_location("cube_map"), 0);
  renderer::render(skybox);

  /*
    MAIN SCENE RENDER
  */

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glCullFace(GL_BACK);

  // Bind effect
  renderer::bind(eff);

  // Render meshes
  for (auto &e : meshes) {
    auto m = e.second;

    // Create MVP matrix
    M = m.get_transform().get_transform_matrix();
    auto MVP = P * V * M;

    // Set MVP matrix uniform
    glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
    // Set Normal matrix uniform - get vertex normal transform
    glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
    // Set M matrix uniform - convert vertices to world space
    glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

    for (int i = 0; i < 4; i++) {
      mat4 lightMVP = LightProjectionMat * shadows[i].get_view() * M;
      glUniformMatrix4fv(eff.get_uniform_location("lightMVP[" + to_string(i) + "]"), 1, GL_FALSE, value_ptr(lightMVP));
    }

    //Bind texture to renderer and pass to shader
    bool env_map = false;
    bool parallax = false;
    bool normal_b = false;
    if (e.first == "warp_stone")
      renderer::bind(textures[0], 0);
    else if (e.first == "dissolve_stone") {
      renderer::bind(textures[2], 0);
      renderer::bind(textures[3], 6);
      normal_b = true;
    }
    else if (e.first.find("pedestal") != string::npos)
      renderer::bind(textures[1], 0);
    else if (e.first.find("lamp") != string::npos)
      renderer::bind(textures[4], 0);
    else if (e.first == "skeleton")
      renderer::bind(textures[5], 0);
    else if (e.first == "amethyst") {
      renderer::bind(cube_map, 5);
      glUniform1i(eff.get_uniform_location("cubemap"), 5);
      env_map = true;
    }
    else if (e.first == "parallax_stone") {
      renderer::bind(textures[6], 0);
      renderer::bind(textures[7], 6);
      renderer::bind(textures[8], 7);
      parallax = true;
    }

    glUniform1i(eff.get_uniform_location("env_map"), env_map);
    glUniform1i(eff.get_uniform_location("normal_b"), normal_b);
    glUniform1i(eff.get_uniform_location("parallax"), parallax);

    renderer::bind(m.get_material(), "mat");
    renderer::bind(spots, "spots");
    renderer::bind(points, "points");

    glUniform1i(eff.get_uniform_location("tex"), 0);
    glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam_ref->get_position()));
    glUniform1f(eff.get_uniform_location("ambient_intensity"), 0.075);

    for (int i = 0; i < 4; i++) {
      renderer::bind(shadows[i].buffer->get_depth(), i+1);
      glUniform1i(eff.get_uniform_location("shadow_map[" + to_string(i) + "]"), i+1);
    }

    glUniform1i(eff.get_uniform_location("normal_map"), 6);
    glUniform1i(eff.get_uniform_location("depth_map"), 7);
    glUniform1f(eff.get_uniform_location("height_scale"), 1.0f);

    renderer::render(m);
  }

  /*
    POST-PROCESS RENDER
  */
  renderer::set_render_target();
  renderer::bind(vignette_eff);
  MVP = mat4(1.0);
  glUniformMatrix4fv(vignette_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  renderer::bind(frame.get_frame(), 0);
  glUniform1i(vignette_eff.get_uniform_location("frame"), 0);
  glUniform2fv(vignette_eff.get_uniform_location("res"), 1, value_ptr(vec2(1600, 900)));

  renderer::render(screen_quad);
  return true;
}

void main() {
  //MSAA set to 8 samples
  glfwWindowHint(GLFW_SAMPLES, 8);
  // Create application
  app application("Raish Allan Computer Graphics Coursework Part 2");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_initialise(initialise);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}
