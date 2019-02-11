#version 330 core

in vec3 UV;

vec4 Up = vec4(0.1f, 0.0f, 0.0f, 1.0f);
vec4 Down = vec4(0.0, 0.0f, 0.0f, 1.0f);

out vec4 Color;

vec3 cubeToSphere(vec3 cube)
{
	vec3 cubecube = cube*cube;
	vec3 halfcubecube = cubecube/2.0;
	return cube * 
		sqrt(1 - 
			 halfcubecube.yzx - //(yy/2.0, zz/2.0, xx/2.0)
		 	halfcubecube.zxy + //(zz/2.0, xx/2.0, yy/2.0)
			 (cubecube.yzx*cubecube.zxy)/3.0); //(yy*zz/3.0, zz*xx/3.0, xx*yy/3.0)
}

float componentToSphere(vec3 v)
{
	float bb = v.y*v.y;
	float cc = v.z*v.z;
	return v.x*sqrt(1 - (bb/2.0) - (cc/2.0) + (bb*cc/3.0));
}

void main()
{
	float x = componentToSphere(vec3(UV.x, UV.y, UV.z));
	float y = componentToSphere(vec3(UV.y, UV.z, UV.x));
	float z = componentToSphere(vec3(UV.z, UV.x, UV.y));

	float mixValue = (y + 1.0)/2.0;
	
	Color = mix(Down, Up, mixValue);
}