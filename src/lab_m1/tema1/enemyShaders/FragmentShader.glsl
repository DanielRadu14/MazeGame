#version 330
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;

layout(location = 0) out vec4 out_color;

varying vec2 vUv;
varying float noise;
uniform sampler2D tExplosion;

float random(vec3 scale, float seed) {
	return fract(sin(dot(gl_FragCoord.xyz + seed, scale)) * 43758.5453 + seed);
}

void main()
{
	float r = .01 * random(vec3(12.9898, 78.233, 151.7182), 0.0);
	vec2 tPos = vec2(0, 1.3 * noise + r);
	vec4 color = texture2D(tExplosion, tPos);

	out_color = vec4(0.45f, 0.29f, 0.9f, 1);
}
