#version 330 core

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform mat4 MVP;
uniform float uTime;
uniform float uTimePeriod;
uniform float uParticleSize;

layout(location = 5) in vec3 particleCenter;
layout(location = 0) in vec3 vertexOffset;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexUv;
layout(location = 3) in float vertexTimeOffset;
layout(location = 4) in vec4 vertexVelocity;

out vec4 vColor;
out vec2 vTexUv;
out float normalizedTime;

void main()
{
	const float tau = 2.0*3.14159;
	float time = mod((uTime + vertexTimeOffset), uTimePeriod);
	normalizedTime = time/uTimePeriod;

	float particleSizeTime = mod(time + (vertexTimeOffset * (gl_VertexID/4)), uTimePeriod);
	float particleSizeFactor = sin(tau*particleSizeTime);

	
	// shift (-1.0,1.0) particleSizeFactor range to (0.5, 1.0)
	particleSizeFactor = (particleSizeFactor/2.0) + 1.0;
	float particleSize = uParticleSize*particleSizeFactor;

	vec3 linearVelocity = vertexVelocity.xyz;
	float angularVelocity = vertexVelocity.w;
	vec3 angularPosition = vec3(sin(angularVelocity*time), 0.0, cos(angularVelocity*time));

	vec3 position = particleCenter + time*linearVelocity + angularPosition;
	vec3 cameraRight = vec3(uViewMatrix[0][0], uViewMatrix[1][0], uViewMatrix[2][0]);
	vec3 cameraUp = vec3(uViewMatrix[0][1], uViewMatrix[1][1], uViewMatrix[2][1]);

	position = (uModelMatrix*vec4(position,1.0)).xyz;
	vec3 offset = vertexOffset*(particleSize/2.0);
	position += cameraRight*offset.x + cameraUp*offset.y;

	vColor = mix(vertexColor, vec4(0.0), normalizedTime);

	// Change UV range from [0.0, 1.0] to [-1.0, 1.0]
	vTexUv = (2*vertexUv) - vec2(1.0, 1.0);
	gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0);
}