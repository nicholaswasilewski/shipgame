#version 330 core

in vec4 vColor;
in vec2 vTexUv;
in float normalizedTime;

out vec4 Color;

void main()
{
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 black = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specColor = mix(white, black, normalizedTime);
	
	// Fake specular glow
	float distanceBlendFactor = length(vTexUv);
	vec4 GlowingColor = mix(specColor, vColor, smoothstep(0.2, 0.4, distanceBlendFactor));
	// Fade color
	Color = mix(GlowingColor, black, distanceBlendFactor);
}