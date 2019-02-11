#version 330 core

in vec4 vColor;
in vec2 vTexUv;
in float normalizedTime;

out vec4 Color;

void main()
{
	float blendFactor = length(vTexUv);
	blendFactor = blendFactor;
	vec4 black = vec4(0.0, 0.0, 0.0, 0.0);
	Color = mix(vColor, black, blendFactor);
}