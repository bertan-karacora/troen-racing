#version 450 core

// Colors
const vec3 tron_blue = vec3(0.094, 0.792, 0.902);
const vec3 tron_red = vec3(1.0, 0.031, 0.015);

// Materials (a, d, s, e)
const vec4 body = vec4(0.994286, 0.2598, 0.288571, 0.0);
const vec4 body2 = vec4(1.0, 0.0, 0.5, 0.0);
const vec4 contour = vec4(1.0, 0.8, 0.5, 0.0);
const vec4 fer = vec4(0.657143, 0.393114, 0.5, 0.0);
const vec4 materials[] = {body, contour, body2, fer};


in vec3 interp_normal;

layout (location = 0) out vec4 normal_depth;
layout (location = 1) out vec4 diffuse_specular;
layout (location = 2) out vec4 emitting_shininess;

uniform uint material;
uniform uint is_blue;

void main() {
	normal_depth = vec4(0.5 * (normalize(interp_normal) + 1.0), gl_FragCoord.z);

	if (material == 1) {
		diffuse_specular.rgb = bool(is_blue) ? tron_blue : tron_red;
		diffuse_specular.a = materials[material].z;
		emitting_shininess.rgb = diffuse_specular.rgb * 4.0;
		emitting_shininess.a = 10.0;
	} else {
		diffuse_specular.rgb = vec3(materials[material].y);
		diffuse_specular.a = materials[material].z;
		emitting_shininess.rgb = vec3(0.0);
		emitting_shininess.a = 10.0;
	}
}
