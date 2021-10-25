#version 450 core
layout (location = 0) in vec2 position;

out vec2 interp_uv;
out vec3 eye_direction;

uniform vec2 half_size_near;

void main() {
	gl_Position  = vec4(position, 0.0, 1.0);
	interp_uv = clamp(position * 0.5 + 0.5, 0.0, 1.0);

	// Compute eye_direction using half_size_near
	eye_direction = vec3(half_size_near * (2.0 * interp_uv - vec2(1.0, 1.0)), -1.0);
}