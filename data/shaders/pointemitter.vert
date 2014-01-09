#version 130
uniform int dt;
uniform mat4 sourcematrix;
uniform int level;
uniform float size_increase_factor;

in vec3 particle_position_initial;
in float lifetime_initial;
in vec3 particle_velocity_initial;
in float size_initial;

in vec3 particle_position;
in float lifetime;
in vec3 particle_velocity;
in float size;

out vec3 new_particle_position;
out float new_lifetime;
out vec3 new_particle_velocity;
out float new_size;

void main(void)
{
  vec4 initialposition = sourcematrix * vec4(particle_position_initial, 1.0);
  vec4 adjusted_initial_velocity = sourcematrix * vec4(particle_position_initial + particle_velocity_initial, 1.0) - initialposition;
  float adjusted_lifetime = lifetime  + (float(dt)/lifetime_initial);
  bool reset = (adjusted_lifetime > 1.) && (gl_VertexID <= level);
  reset = reset || (lifetime < 0.);
  new_particle_position = !reset ? particle_position + particle_velocity.xyz * float(dt) : initialposition.xyz;
  new_lifetime = !reset ? adjusted_lifetime : 0.;
  new_particle_velocity = !reset ? particle_velocity : adjusted_initial_velocity.xyz;
  new_size = !reset ? mix(size_initial, size_initial * size_increase_factor, adjusted_lifetime) : size_initial;
  gl_Position = vec4(0.);
}