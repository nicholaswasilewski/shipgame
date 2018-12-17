#version 330 core

uniform float Radius;
uniform float Width;
uniform float FilledAngle;
uniform vec3 FillColor;

in vec2 CircleVal;
out vec3 Color;


void main()
{
	float Distance = length(CircleVal);

	float HalfWidth = Width/2.0;
	float MinRadius = Radius-HalfWidth;
	float MaxRadius = Radius+HalfWidth;
	float Angle = atan(CircleVal.y, CircleVal.x);
//  Use step if it seems worthwhile to avoid the branch, but alpha testing will have to be enabled
//	float DistanceTest = step(MinRadius, Distance) * step(Distance, MaxRadius);
//	Color = FillColor * DistanceTest;

	if (Distance > MinRadius && Distance < MaxRadius && Angle < FilledAngle) {
		Color = FillColor;
	}
	else
	{
		discard;
	}
}