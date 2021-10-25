#version 450 core

// Phong params
const float emission = 25.0;


in vec2 interp_uv;
in vec3 eye_direction;
in vec3 local_light_pos;

out vec4 frag_color;

uniform sampler2D normal_depth_tex;
uniform sampler2D diffuse_specular_tex;
uniform sampler2D emitting_shininess_tex;
uniform sampler2D bloom_tex;

uniform mat4 proj_mat;
uniform mat4 inv_view_mat;

vec3 get_eye_position(float z) {
	// Compute the position in eye space using z and proj_mat
	float eye_z = proj_mat[3][2] / (proj_mat[2][3] * z - proj_mat[2][2]);
	return eye_direction * -eye_z;
}

vec3 phong(vec3 l, vec3 n, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
	float d = length(l);
	vec3 ln = l / d;
	vec3 r = reflect(-ln, n);
	float intensity = 1.0 / (d * d);
	float ka = 0.05;
	float kd = dot(ln, n);
	float ks = pow(clamp(r.z, 0.0, 1.0), shininess);

	return ka * ambient + (kd * diffuse + ks * specular) * intensity * emission;
}

void main() {
	vec4 normal = texture(normal_depth_tex, interp_uv);
	normal = length(normal) < 0.01 ? vec4(0.0) : 2.0 * normal - 1.0;
	vec3 world_normal = normal.xyz;
	float ndc_depth = normal.w;
	vec3 eye_normal = vec3(vec4(world_normal, 1.0) * inv_view_mat);
	vec3 eye_pos = get_eye_position(normal.w);

	vec3 diffuse = texture(diffuse_specular_tex, interp_uv).rgb;
	float specular = texture(diffuse_specular_tex, interp_uv).a;
	vec3 emitting = texture(emitting_shininess_tex, interp_uv).rgb;
	float shininess = texture(emitting_shininess_tex, interp_uv).a;
	vec3 bloom = texture(bloom_tex, interp_uv).rgb;

	// Additive blending for emitting objects
	vec3 color = emitting + bloom;
	// Lighting
	if (length(world_normal) > 0.01) {
		color += phong(local_light_pos - eye_pos, eye_normal, diffuse, diffuse, vec3(specular), shininess);
	}
	
	frag_color = vec4(color, 1.0);
}
