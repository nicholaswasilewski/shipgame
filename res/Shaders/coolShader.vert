#version 330 core

uniform mat4 MVP;
uniform float uTime;
uniform float uTimePeriod;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexUv;
layout(location = 3) in float vertexTimeOffset;
layout(location = 4) in vec4 vertexVelocity;
layout(location = 5) in float angularVelocity;

out vec4 vColor;
out vec2 vTexUv;
out float normalizedTime;

void main()
{
	float time = mod((uTime + vertexTimeOffset), uTimePeriod);
	normalizedTime = time/uTimePeriod;
	vec3 linearVelocity = vertexVelocity.xyz;
	float angularVelocity = vertexVelocity.w;
	vec3 angularPosition = vec3(sin(angularVelocity*time), 0.0, cos(angularVelocity*time));
	vec3 position = vertexPosition + time*linearVelocity + angularPosition;
	vColor = mix(vertexColor, vec4(0.0), normalizedTime);
	vTexUv = (2*vertexUv) - vec2(1.0, 1.0);
	gl_Position = MVP * vec4(position, 1.0);
}