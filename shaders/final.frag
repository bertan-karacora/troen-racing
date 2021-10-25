#version 450 core

// Custom Reinhard tone mapping param
const float lum_max = 1.7;

// Gamma correction param
const float gamma = 2.2;

// Fog params
const float fog_density = 0.03;
const vec3 fog_color = vec3(0.0);

// Screenspace Reflections params
const float stp = 0.1;
const int max_steps = 90;
const int search_steps = 10;
const float reflection_falloff = 3.0;


in vec2 interp_uv;
in vec3 eye_direction;

out vec4 frag_color;

uniform sampler2D normal_depth_tex;
uniform sampler2D diffuse_specular_tex;
uniform sampler2D emitting_shininess_tex;
uniform sampler2D final_tex;

uniform mat4 proj_mat;
uniform mat4 inv_view_mat;

vec3 get_eye_position(float z) {
	// Compute the position in eye space using z and proj_mat
	float eye_z = proj_mat[3][2] / (proj_mat[2][3] * z - proj_mat[2][2]);
	return eye_direction * -eye_z;
}

vec2 binarySearch(vec3 dir, vec3 hit) {
	float depth;
	vec4 proj_coords;
 
	for (int i = 0; i < search_steps; i++) {
		proj_coords = proj_mat * vec4(hit, 1.0);
		proj_coords.xy /= proj_coords.w;
		proj_coords.xy = proj_coords.xy * 0.5 + 0.5;

		depth = get_eye_position(2.0 * texture(normal_depth_tex, proj_coords.xy).w - 1.0).z;

		dir *= 0.5;
		if (hit.z - depth > 0.0) hit += dir;
		else hit -= dir;
	}

	proj_coords = proj_mat * vec4(hit, 1.0);
	proj_coords.xy /= proj_coords.w;
	proj_coords.xy = proj_coords.xy * 0.5 + 0.5;
 
	return proj_coords.xy;
}

vec2 rayMarch(vec3 dir, vec3 hit) {
	dir *= stp;

	float depth;
	float dist;
	vec4 proj_coords;

	for (int i = 0; i < max_steps; i++) {
		hit += dir;
 
		proj_coords = proj_mat * vec4(hit, 1.0);
		proj_coords.xy /= proj_coords.w;
		proj_coords.xy = proj_coords.xy * 0.5 + 0.5;
 
		depth = get_eye_position(2.0 * texture(normal_depth_tex, proj_coords.xy).w - 1.0).z;

		if (depth > 100.0) continue;
		dist = hit.z - depth;

		if (dir.z - dist < 1.2) {
			if (dist <= 0.0) {
				return binarySearch(dir, hit);
			}
		}
	}
	return proj_coords.xy;
}

vec3 fresnelSchlick(float cos_theta, vec3 f0) {
	return f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0);
}

vec3 hash(vec3 a) {
	a = fract(a * 0.8);
	a += dot(a, a.yxz + 19.19);
	return fract((a.xxy + a.yxx) * a.zyx);
}

vec3 postprocess(vec3 hdr_color, vec3 eye_pos) {
	// Custom Reinhard tone mapping
	hdr_color = (hdr_color * (1.0 + hdr_color / (lum_max * lum_max))) / (1.0 + hdr_color);

	// Fog (actually should be before SSR apllied)
	float fog_factor = 1.0 - clamp(exp(-pow(fog_density * length(eye_pos), 2.0)), 0.0, 1.0);
	hdr_color = mix(hdr_color, fog_color, fog_factor);

	// Gamma correction
	return pow(hdr_color, vec3(1.0 / gamma));
}

void main() {
	vec4 normal = texture(normal_depth_tex, interp_uv);
	normal = length(normal) < 0.01 ? vec4(0.0) : 2.0 * normal - 1.0;
	vec3 world_normal = normal.xyz;
	float ndc_depth = normal.w;
	vec3 eye_normal = vec3(vec4(world_normal, 1.0) * inv_view_mat);
	vec3 eye_pos = get_eye_position(normal.w);
	vec3 world_pos = vec3(vec4(eye_pos, 1.0) * inv_view_mat);

	vec3 diffuse = texture(diffuse_specular_tex, interp_uv).rgb;
	float specular = texture(diffuse_specular_tex, interp_uv).a;
	vec3 emitting = texture(emitting_shininess_tex, interp_uv).rgb;
	float shininess = texture(emitting_shininess_tex, interp_uv).a;

	// Get lighting result
	vec3 color = texture(final_tex, interp_uv).rgb;

	// Screenspace Reflections
	if (length(world_normal) > 0.01) {
		float metallicness = shininess / 10.0;
		vec3 reflected = normalize(reflect(normalize(eye_pos), normalize(eye_normal)));
		vec3 jitt = mix(vec3(0.0), vec3(hash(world_pos)), specular / 20.0);
		vec2 hit_coords = rayMarch((jitt + reflected * -eye_pos.z), eye_pos);

		vec2 d_coords = smoothstep(0.2, 0.6, abs(vec2(0.5) - hit_coords));
		float screen_edge_factor = clamp(1.0 - (d_coords.x + d_coords.y), 0.0, 1.0);
		float reflection_mult = pow(metallicness, reflection_falloff) * screen_edge_factor * -reflected.z;

		vec3 f0 = mix(vec3(0.0), diffuse, metallicness);
		vec3 fresnel = fresnelSchlick(max(dot(normalize(eye_normal), normalize(eye_pos)), 0.0), f0);

		vec3 SSR = texture(final_tex, hit_coords).rgb * clamp(reflection_mult, 0.0, 0.9) * fresnel;
		color += SSR;
	}
	
	color = postprocess(color, eye_pos);
	frag_color = vec4(color, 1.0);
}
