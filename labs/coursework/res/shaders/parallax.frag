void parallax_occulsion(inout vec3 tex_coord, in vec3 view_dir, sampler2D depth_map, float height_scale) {
  float layers = mix(256.00, 64.00, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));
  float layer_depth = 1.0 / layers;
  float current_depth = 0.0;
  vec2 shift = view_dir.xy * height_scale;
  vec2 delta = shift / layers;

  vec2 current_coord = vec2(tex_coord.xy);
  float current_depth_value = texture(depth_map, current_coord).x;

  while (current_depth < current_depth_value) {
    current_coord -= delta;
    current_depth_value = texture(depth_map, current_coord).x;
    current_depth += layer_depth;
  }

  
  vec2 prev_coord = current_coord + delta;
  float after = current_depth_value - current_depth;
  float before = texture(depth_map, prev_coord).x - current_depth + layer_depth;
  float weight = after / (after - before);
  
  tex_coord = vec3((prev_coord * weight + current_coord * (1.0 - weight)), tex_coord.z);
}