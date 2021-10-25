#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 interp_normal;
out vec4 interp_color;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main() {
	gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0);
	interp_normal = normalize((vec4(normal, 0.0) * inverse(model_mat)).xyz);
}
