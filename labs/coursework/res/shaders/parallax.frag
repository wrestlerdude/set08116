void steep_parallax_depth(inout vec3 tex_coord, in vec3 view_dir, sampler2D depth_map, float height_scale) {
  float height = texture(depth_map, tex_coord.xy).x;
  vec2 p = view_dir.xy / view_dir.z * (height * height_scale);
  tex_coord.xy - p;
}