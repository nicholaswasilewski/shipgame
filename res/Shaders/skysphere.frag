#version 330 core

in vec3 UV;

vec4 Up = { 0.2f, 0.5f, 1.0f, 1.0f };
vec4 Down = { 0.2f, 0.2f, 0.2f, 1.0f };

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
	float bb = v.b*v.b;
	float cc = v.c*v.c;
	return v.a*sqrt(1 - (bb/2.0) - (cc/2.0) + (bb*cc/3.0));
}

void main()
{
	float x = componentToSphere(UV.x, UV.y, UV.z);
	float y = componentToSphere(UV.y, UV.z, UV.x);
	float z = componentToSphere(UV.z, UV.x, UV.y);

	float mixValue = (y + 1.0)/2.0;
	
	Color = vec4(mix(Down, Up, mixValue), 1.0);
	
//	Color = vec4(y, 0.0, 0.0, 1.0);	
}