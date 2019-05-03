// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light
{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};
#endif

// Material data
#ifndef MATERIAL
#define MATERIAL
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position,
                in vec3 normal, in vec3 view_dir, in vec4 tex_colour, in float ambient_intensity) {
  vec3 light_dir = normalize(spot.position - position);
  float d = distance(spot.position, position);
  //Calculate attentuation factor
  float c = 1.0 / (spot.constant + (spot.linear * d) + (spot.quadratic * (d * d)));
  float intensity = pow(max(dot(-spot.direction, light_dir), 0.0), spot.power);
  vec4 light_colour = (c * intensity) * spot.light_colour;
  //Calculate diffuse component
  vec4 diffuse = max(dot(normal, light_dir), 0.0) * (mat.diffuse_reflection * light_colour);
  vec3 half_vector = normalize(light_dir + view_dir);
  //Calculate specular component
  vec4 specular = pow(max(dot(normal, half_vector), 0.0), mat.shininess) * (mat.specular_reflection * light_colour);
  vec4 primary = mat.emissive + diffuse + ambient_intensity;
  //Make sure the alphas are = 1
  primary.w = 1.0;
  specular.w = 1.0;

  return (primary * tex_colour) + specular;
}

