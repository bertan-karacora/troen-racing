#version 450 core

const vec3 tron_color = vec3(0.094, 0.792, 0.902);

in vec2 grid_coord;
in vec3 interp_normal;

layout (location = 0) out vec4 normal_depth;
layout (location = 1) out vec4 diffuse_specular;
layout (location = 2) out vec4 emitting_shininess;

void main() {
	normal_depth = vec4(normalize(interp_normal) * 0.5 + 0.5, gl_FragCoord.z);

	// anti-aliased grid hack
	vec2 grid = abs(fract(grid_coord - 0.5) - 0.5) / fwidth(grid_coord);
	float line = min(grid.x, grid.y);

	// Alternative
	// const float edge_width = 1.0 / 32.0;
	// vec2 uv = mod(grid_coord + edge_width, 1.0) - mod(grid_coord - edge_width, 1.0) - 2.0 * edge_width;
	// diffuse = vec4(grid_color * clamp(length(uv), 0.0, 1.0), 1.0);

	diffuse_specular.rgb = vec3((1.0 - min(line, 1.0)) * tron_color);
	diffuse_specular.a = 0.75;
	emitting_shininess.rgb = diffuse_specular.rgb * 3.0;
	emitting_shininess.a = 10.0;
}
