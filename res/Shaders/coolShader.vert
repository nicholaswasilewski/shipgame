#version 330 core

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform mat4 MVP;
uniform float uTime;
uniform float uTimePeriod;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexUv;
layout(location = 3) in float vertexTimeOffset;
layout(location = 4) in vec4 vertexVelocity;
layout(location = 5) in float angularVelocity;
layout(location = 6) in vec3 particleCenter;

out vec4 vColor;
out vec2 vTexUv;
out float normalizedTime;

void main()
{
	float time = mod((uTime + vertexTimeOffset), uTimePeriod);
	normalizedTime = time/uTimePeriod;

	vec3 linearVelocity = vertexVelocity.xyz;
	float angularVelocity = vertexVelocity.w;
	float radius = length(vec2(vertexPosition.x, vertexPosition.z));
	vec3 angularPosition = vec3(sin(angularVelocity*time), 0.0, cos(angularVelocity*time));

	vec3 centerOffset = vertexPosition - particleCenter;
	vec3 position = particleCenter + time*linearVelocity + angularPosition;
	vec3 cameraRight = vec3(uViewMatrix[0][0], uViewMatrix[1][0], uViewMatrix[2][0]);
	vec3 cameraUp = vec3(uViewMatrix[0][1], uViewMatrix[1][1], uViewMatrix[2][1]);

	position = (uModelMatrix*vec4(position,1.0)).xyz;
	position += cameraRight*centerOffset.x + cameraUp*centerOffset.y;

	vColor = mix(vertexColor, vec4(0.0), normalizedTime);
	vTexUv = (2*vertexUv) - vec2(1.0, 1.0);
	gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0);
}