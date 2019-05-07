void parallax_occulsion(inout vec3 tex_coord, in vec3 view_dir, sampler2D depth_map, float height_scale) {
  //Layers vary depending on if the fragment is being rendered head on, minimum 64, max 256.
  float layers = mix(256.00, 64.00, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));
  //Calculates the size of each layer
  float layer_depth = 1.0 / layers;
  float current_depth = 0.0;
  //texture coordinate shift per layer
  vec2 shift = view_dir.xy * height_scale;
  vec2 delta = shift / layers;

  vec2 current_coord = vec2(tex_coord.xy);
  float current_depth_value = texture(depth_map, current_coord).x;

  while (current_depth < current_depth_value) {
    //shifts coordinates in direction
    current_coord -= delta;
    current_depth_value = texture(depth_map, current_coord).x;
    //get depth for next layer
    current_depth += layer_depth;
  }

  //coordinates before view collision
  vec2 prev_coord = current_coord + delta;
  //depth after and before
  float after = current_depth_value - current_depth;
  float before = texture(depth_map, prev_coord).x - current_depth + layer_depth;
  //linear interpolation of coordinates
  float weight = after / (after - before);
  //output modified texture coordinates
  tex_coord = vec3((prev_coord * weight + current_coord * (1.0 - weight)), tex_coord.z);
}