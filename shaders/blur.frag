#version 450 core

// Gaussian weights
const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 interp_uv;

out vec4 frag_color;

uniform uint horizontal; // Bool
uniform sampler2D tex;

void main() {
	vec2 tex_offset = 1.0 / textureSize(tex, 0);
	vec3 result = texture(tex, interp_uv).rgb * weight[0];

	if (bool(horizontal)) {
		for (int i = 1; i < weight.length(); ++i) {
			result += texture(tex, interp_uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(tex, interp_uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	} else {
		for (int i = 1; i < weight.length(); ++i) {
			result += texture(tex, interp_uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(tex, interp_uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}

	frag_color = vec4(result, 1.0);
}
