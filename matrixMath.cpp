#include <math.h>
#include <stdio.h>

typedef struct Matrix4x4
{
    union
    {
	struct {
	    float x0, y0, z0, w0,
		  x1, y1, z1, w1,
		  x2, y2, z2, w2,
		  x3, y3, z3, w3;
	};
	float E[4][4];
    };
} matrix4x4, m4x4, mat4;

typedef struct
{
    union
    {
	struct
	{
	    float x,y,z;
	};
	float E[3];
    };
} vector3, v3;

typedef struct
{
    union
    {
	struct
	{
	    float x, y, z, w;
	};
	float E[4];
    };
} vector4, v4;

typedef struct
{
    float x,y,z,w;
} quaternion;

v3 V3(float x, float y, float z)
{
    v3 Result;
    Result.x = x;
    Result.y = y;
    Result.z = z;
    return Result;
}

float Magnitude(v3 v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

v3 operator+(v3 v1, v3 v2)
{
    v3 Result = {
	v1.x + v2.x,
	v1.y + v2.y,
	v1.z + v2.z
    };

    return Result;
}

v3 operator-(v3 v1, v3 v2)
{
    v3 Result = {
	v1.x - v2.x,
	v1.y - v2.y,
	v1.z - v2.z
    };
    return Result;
}

v3 operator/(v3 v, float s)
{
    v3 Result = {
	v.x / s,
	v.y / s,
	v.z / s
    };
    return Result;
}

quaternion operator/(quaternion q, float s)
{
    quaternion Result = {
	q.x / s,
	q.y / s,
	q.z / s,
	q.w / s
    };
}

v3 Length(v3 v)
{
    return v / Magnitude(v);
}

float Length(quaternion q)
{
    return sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

quaternion Normalize(quaternion q)
{
    return q / Length(q);
}

float Dot(v3 v1, v3 v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

v3 Cross(v3 u, v3 v)
{
    v3 s;
    s.x = u.y*v.z - u.z*v.y;
    s.y = u.z*v.x - u.x*v.z;
    s.z = u.x*v.y - u.y*v.x;

    return s;
}

v4 V4(float x, float y, float z, float w)
{
    v4 Result = {x, y, z, w};
    return Result;
}

v4 V4(v3 v, float w)
{
    return V4(v.x, v.y, v.z, w);
}

mat4 operator*(mat4 m1, mat4 m2)
{
    float x0, x1, x2, x3,
	y0, y1, y2, y3,
	z0, z1, z2, z3,
	w0, w1, w2, w3;
    
    x0 = m1.x0*m2.x0 + m1.x1*m2.y0 + m1.x2*m2.z0 + m1.x3*m2.w0;
    x1 = m1.x0*m2.x1 + m1.x1*m2.y1 + m1.x2*m2.z1 + m1.x3*m2.w1;
    x2 = m1.x0*m2.x2 + m1.x1*m2.y2 + m1.x2*m2.z2 + m1.x3*m2.w2;
    x3 = m1.x0*m2.x3 + m1.x1*m2.y3 + m1.x2*m2.z3 + m1.x3*m2.w3;

    y0 = m1.y0*m2.x0 + m1.y1*m2.y0 + m1.y2*m2.z0 + m1.y3*m2.w0;
    y1 = m1.y0*m2.x1 + m1.y1*m2.y1 + m1.y2*m2.z1 + m1.y3*m2.w1;
    y2 = m1.y0*m2.x2 + m1.y1*m2.y2 + m1.y2*m2.z2 + m1.y3*m2.w2;
    y3 = m1.y0*m2.x3 + m1.y1*m2.y3 + m1.y2*m2.z3 + m1.y3*m2.w3;

    z0 = m1.z0*m2.x0 + m1.z1*m2.y0 + m1.z2*m2.z0 + m1.z3*m2.w0;
    z1 = m1.z0*m2.x1 + m1.z1*m2.y1 + m1.z2*m2.z1 + m1.z3*m2.w1;
    z2 = m1.z0*m2.x2 + m1.z1*m2.y2 + m1.z2*m2.z2 + m1.z3*m2.w2;
    z3 = m1.z0*m2.x3 + m1.z1*m2.y3 + m1.z2*m2.z3 + m1.z3*m2.w3;

    w0 = m1.w0*m2.x0 + m1.w1*m2.y0 + m1.w2*m2.z0 + m1.w3*m2.w0;
    w1 = m1.w0*m2.x1 + m1.w1*m2.y1 + m1.w2*m2.z1 + m1.w3*m2.w1;
    w2 = m1.w0*m2.x2 + m1.w1*m2.y2 + m1.w2*m2.z2 + m1.w3*m2.w2;
    w3 = m1.w0*m2.w3 + m1.w1*m2.y3 + m1.w2*m2.z3 + m1.w3*m2.w3;
    
    mat4 Result = {
	x0, y0, z0, w0,
	x1, y1, z1, w1,
	x2, y2, z2, w2,
	x3, y3, z3, w3
    };

    return Result;
}

v4 operator*(mat4 m, v4 v)
{
    v4 Result;
    Result.x = m.x0*v.x + m.y0*v.y + m.z0*v.z + m.w0*v.w;
    Result.y = m.x1*v.x + m.y1*v.y + m.z1*v.z + m.w1*v.w;
    Result.z = m.x2*v.x + m.y2*v.y + m.z2*v.z + m.z3*v.w;
    Result.w = m.x3*v.x + m.y3*v.y + m.z3*v.z + m.w3*v.w;

    return Result;
}

quaternion QuaternionFromAxisAngle(v3 Axis, float Angle)
{
    float halfAngle = Angle/2.0f;
    float a = sin(halfAngle);
    float b = cos(halfAngle);
    quaternion Result = {
	Axis.x * a,
	Axis.y * a,
	Axis.z * a,
	b
    };
    
    return Result;
}

mat4 MakeRotation(quaternion q)
{
    float xx2 = q.x*q.x*2;
    float xy2 = q.x*q.y*2;
    float xz2 = q.x*q.z*2;
    float xw2 = q.x*q.y*2;
 
    float yy2 = q.y*q.y*2;
    float yz2 = q.y*q.z*2;
    float yw2 = q.y*q.w*2;

    float zz2 = q.z*q.z*2;
    float zw2 = q.z*q.w*2;

    float ww2 = q.w*q.w*2;
    
    mat4 Result = {
	1-yy2-zz2, xy2 - zw2, xz2 + yw2, 0,
	xy2 + zw2, 1-xx2-zz2, yz2 - xw2, 0,
        xz2 - yw2, yz2 + xw2, 1-xx2-yy2, 0,
	        0,         0,         0, 1
    };
    return Result;
}

mat4 MakeRotation(v3 u, float theta)
{
    float cost = cos(theta);
    float sint = sin(theta);

    float x0,x1,x2,
	y0, y1, y2,
	z0, z1, z2;
    x0 = cost + u.x*u.x*(1.0f-cost);
    x1 = u.x*u.y*(1.0f-cost)-u.z*sint;
    x2 = u.x*u.z*(1.0f-cost)+u.y*sint;
    
    y0 = u.y*u.x*(1.0f-cost)+u.z*sint;
    y1 = cost + u.y*u.y*(1.0f-cost);
    y2 = u.y*u.z*(1.0f-cost)-u.x*sint;

    z0 = u.z*u.x*(1.0f-cost)-u.y*sint;
    z1 = u.z*u.y*(1.0f-cost)+u.x*sint;
    z2 = cost + u.z*u.z*(1-cost);
    

    mat4 Result = {
	x0, y0, z0, 0,
	x1, y1, z1, 0,
	x2, y2, z2, 0,
	0,   0,  0, 1
    };

    return Result;
}

mat4 MakeRotation2(v3 Axis, float Angle)
{
    quaternion q = QuaternionFromAxisAngle(Axis, Angle);
    return MakeRotation(q);
}

mat4 MakeTranslation(v3 v)
{
    mat4 Result = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	v.x, v.y, v.z, 1.0f
    };
    return Result;
}

mat4 MakeScale(v3 v)
{
    mat4 Result = {
	v.x, 0.0f, 0.0f, 0.0f,
	0.0f, v.y, 0.0f, 0.0f,
	0.0f, 0.0f, v.z, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
    };
    return Result;
}

mat4 Identity4x4() {
    mat4 Result = MakeScale(V3(1.0f, 1.0f, 1.0f));
    return Result;
}

mat4 LookAtView(v3 Position, v3 Target, v3 Up)
{
    v3 Z = Length(Position - Target);
    v3 X = Length(Cross(Up, Z));
    v3 Y = Cross(Z, X);

    mat4 View = {
	X.x, Y.x, Z.x, 0,
	X.y, Y.y, Z.y, 0,
	X.z, Y.z, Z.z, 0,
	-Dot(X, Position),   -Dot(Y, Position),   -Dot(Z, Position), 1
    };

    return View;
}

mat4 MakeOrthoProjection(float HalfWidth, float HalfHeight, float Near, float Far)
{
    mat4 Result = {
	1.0f/HalfWidth, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f/HalfHeight, 0.0f, 0.0f,
	0.0f, 0.0f, -2.0f/(Far-Near), 0.0f,
	0.0f, 0.0f, -(Far+Near)/(Far-Near), 1.0f
    };
    return Result;
}

mat4 MakePerspectiveProjection(float Fov, float Aspect, float Near, float Far)
{
    
    mat4 Result = {
	1 / tan(Fov*Aspect/2.0f), 0.0f, 0.0f, 0.0f,
	0.0f, 1 / tan(Fov/2.0f), 0.0f, 0.0f,
	0.0f, 0.0f, -(Far+Near)/(Far-Near), -1.0f,
	0.0f, 0.0f, -2*(Near*Far)/(Far-Near), 1.0f
    };
    return Result;
}

void PrintMatrix(Matrix4x4 m)
{
    printf("%.4f %.4f %.4f %.4f\n"
           "%.4f %.4f %.4f %.4f\n"
           "%.4f %.4f %.4f %.4f\n"
           "%.4f %.4f %.4f %.4f\n",
	   (double)m.x0, (double)m.y0, (double)m.z0, (double)m.w0,
	   (double)m.x1, (double)m.y1, (double)m.z1, (double)m.w1,
	   (double)m.x2, (double)m.y2, (double)m.z2, (double)m.w2,
	   (double)m.x3, (double)m.y3, (double)m.z3, (double)m.w3);
}
