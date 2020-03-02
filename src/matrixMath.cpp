#ifndef MATRIXMATH_CPP__
#define MATRIXMATH_CPP__

#include "math.cpp"

#include <math.h>
#include <stdio.h>

struct mat4
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
};

struct mat3
{
    union
    {
        struct {
            float x0, y0, z0,
            x1, y1, z1,
            x2, y2, z2;
        };
        float E[3][3];
    };
};

struct v2
{
    float x,y;
};

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

quaternion Quaternion(float x, float y, float z, float w)
{
    quaternion Result = {x, y, z, w};
    return Result;
}

v2 V2(float x, float y)
{
    v2 Result = {x, y};
    return Result;
}

v3 V3(float x, float y, float z)
{
    v3 Result;
    Result.x = x;
    Result.y = y;
    Result.z = z;
    return Result;
}

v3 V3(v4 vec)
{
    v3 Result;
    Result.x = vec.x;
    Result.y = vec.y;
    Result.z = vec.z;
    return Result;
} 

void PrintVector(v3 v)
{
    printf("X:%f, Y:%f, Z:%f\n", v.x, v.y, v.z); 
}

void PrintMatrix(mat4 m)
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

void PrintMatrix3x3(mat3 m) {
    printf("%.4f %.4f %.4f\n"
           "%.4f %.4f %.4f\n"
           "%.4f %.4f %.4f\n",
           (double)m.x0, (double)m.y0, (double)m.z0,
           (double)m.x1, (double)m.y1, (double)m.z1,
           (double)m.x2, (double)m.y2, (double)m.z2);
}

const v3 Vec3UnitX = V3(1.0f,0.0f,0.0f);
const v3 Vec3UnitY = V3(0.0f,1.0f,0.0f);
const v3 Vec3UnitZ = V3(0.0f,0.0f,1.0f);

float Length(v3 v)
{
    return (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

v3 operator+(v3 u, v3 v)
{
    v3 Result = {
        u.x + v.x,
        u.y + v.y,
        u.z + v.z
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

v3 operator*(float s, v3 v)   
{
    return V3(v.x*s, v.y*s, v.z*s);
}

v3 operator*(v3 v, float s)
{
    return s*v;
}

v3 operator-(v3 v)   
{
    return -1.0f*v;
}

v2 operator/(v2 v, float s)
{
    v2 Result = {
        v.x / s,
        v.y / s
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
    return Result;
}

v3 Normalize(v3 v)
{
    return v / Length(v);
}

float Length(v2 v)
{
    return (float)sqrt(v.x*v.x + v.y*v.y);
}

v2 Normalize(v2 v)
{
    return v/Length(v);
}

float Length(quaternion q)
{
    return (float)sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

quaternion Normalize(quaternion q)
{
    return q / Length(q);
}

float Dot(v3 u, v3 v)
{
    return u.x*v.x + u.y*v.y + u.z*v.z;
}

v3 Cross(v3 u, v3 v)
{
    v3 s;
    s.x = u.y*v.z - u.z*v.y;
    s.y = u.z*v.x - u.x*v.z;
    s.z = u.x*v.y - u.y*v.x;
    
    return s;
}

v3 MakeRotationVector(v2 dM, v3 f, v3 u)
{
    //return Cross(dM.x*Cross(f, u) + dM.y*u, f);
    v3 Result;
    Result.x = 
        dM.x*f.y*f.y*u.x + 
        dM.x*f.z*f.z*u.x - 
        dM.x*f.x*f.y*u.y + 
        dM.y*f.z*u.y - 
        dM.y*f.y*u.z - 
        dM.x*f.x*f.z*u.z;
    
    Result.y = 
        -(dM.x*f.x*f.y*u.x) -
        dM.y*f.z*u.x + 
        dM.x*f.x*f.x*u.y + 
        dM.x*f.z*f.z*u.y + 
        dM.y*f.x*u.z - 
        dM.x*f.y*f.z*u.z;
    
    Result.z = 
        -dM.y*f.y*u.x - 
        dM.x*f.x*f.z*u.x - 
        dM.y*f.x*u.y - 
        dM.x*f.y*f.z*u.y + 
        dM.x*f.x*f.x*u.z + 
        dM.x*f.y*f.y*u.z;
    
    //    dM = Normalize(dM);
    return Result;
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

v4 operator*(float s, v4 v)
{
    return V4(-v.x, -v.y, -v.z, -v.w);
}

v4 operator*(v4 v, float s)   
{
    return s*v;
}

v4 operator-(v4 v)
{
    return -1*v;
}

v3 operator*(mat3 m, v3 v)
{
    v3 Result;
    Result.x = m.x0*v.x + m.y0*v.y + m.z0*v.z;
    Result.y = m.x1*v.x + m.y1*v.y + m.z1*v.z;
    Result.z = m.x2*v.x + m.y2*v.y + m.z2*v.z;
    return Result;
}

mat3 operator*(mat3 m1, mat3 m2)
{
    float x0, x1, x2,
    y0, y1, y2,
    z0, z1, z2;
    
    x0 = m1.x0*m2.x0 + m1.x1*m2.y0 + m1.x2*m2.z0;
    x1 = m1.x0*m2.x1 + m1.x1*m2.y1 + m1.x2*m2.z1;
    x2 = m1.x0*m2.x2 + m1.x1*m2.y2 + m1.x2*m2.z2;
    
    y0 = m1.y0*m2.x0 + m1.y1*m2.y0 + m1.y2*m2.z0;
    y1 = m1.y0*m2.x1 + m1.y1*m2.y1 + m1.y2*m2.z1;
    y2 = m1.y0*m2.x2 + m1.y1*m2.y2 + m1.y2*m2.z2;
    
    z0 = m1.z0*m2.x0 + m1.z1*m2.y0 + m1.z2*m2.z0;
    z1 = m1.z0*m2.x1 + m1.z1*m2.y1 + m1.z2*m2.z1;
    z2 = m1.z0*m2.x2 + m1.z1*m2.y2 + m1.z2*m2.z2;
    
    mat3 Result = {
        x0, y0, z0,
        x1, y1, z1,
        x2, y2, z2
    };
    return Result;}


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
    w3 = m1.w0*m2.x3 + m1.w1*m2.y3 + m1.w2*m2.z3 + m1.w3*m2.w3;
    
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
    Axis = Normalize(Axis);
    float halfAngle = Angle/2.0f;
    float a = (float)sin(halfAngle);
    float b = (float)cos(halfAngle);
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
    q = Normalize(q);
    float xx2 = q.x*q.x*2;
    float xy2 = q.x*q.y*2;
    float xz2 = q.x*q.z*2;
    float xw2 = q.x*q.y*2;
    
    float yy2 = q.y*q.y*2;
    float yz2 = q.y*q.z*2;
    float yw2 = q.y*q.w*2;
    
    float zz2 = q.z*q.z*2;
    float zw2 = q.z*q.w*2;
    
    mat4 Result = {
        1-yy2-zz2, xy2 - zw2, xz2 + yw2, 0,
        xy2 + zw2, 1-xx2-zz2, yz2 - xw2, 0,
        xz2 - yw2, yz2 + xw2, 1-xx2-yy2, 0,
        0,         0,         0, 1
    };
    return Result;
}

mat3 MakeRotation3x3(v3 u, float theta)
{
    u = Normalize(u);
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
    
    
    mat3 Result = {
        x0, y0, z0,
        x1, y1, z1,
        x2, y2, z2,
    };
    return Result;
}

mat4 MakeRotation(v3 u, float theta)
{
    u = Normalize(u);
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

mat3 Mat3(mat4 mat)
{
    mat3 Result = {
        mat.E[0][0], mat.E[0][1], mat.E[0][2], 
        mat.E[1][0], mat.E[1][1], mat.E[1][2], 
        mat.E[2][0], mat.E[2][1], mat.E[2][2]
    };
    return Result;
}

mat4 Mat4(mat3 mat)
{
    mat4 Result = {
        mat.E[0][0], mat.E[0][1], mat.E[0][2], 0, 
        mat.E[1][0], mat.E[1][1], mat.E[1][2], 0, 
        mat.E[2][0], mat.E[2][1], mat.E[2][2], 0, 
        0, 0, 0, 1
    };
    return Result;
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

mat4 DirectionView(v3 Position, v3 Direction, v3 Up)
{
    v3 Z = Normalize(Direction);
    v3 X = Normalize(Cross(Up, Z));
    v3 Y = Cross(Z, X);
    
    mat4 View = {
        X.x, Y.x, Z.x, 0,
        X.y, Y.y, Z.y, 0,
        X.z, Y.z, Z.z, 0,
        -Dot(X, Position), -Dot(Y, Position), -Dot(Z, Position), 1.0f
    };
    return View;
}

mat4 LookAtView(v3 Position, v3 Target, v3 Up)
{
    return DirectionView(Position, Position-Target, Up);
}

mat4 MakeOrthographicProjection(float HalfHeight, float Aspect, float Near, float Far)
{
    mat4 Result = {
        1.0f/HalfHeight/Aspect, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f/HalfHeight, 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f/(Far-Near), 0.0f,
        0.0f, 0.0f, -(Far+Near)/(Far-Near), 1.0f
    };
    return Result;
}

mat4 MakePerspectiveProjection(float Fov, float Aspect, float Near, float Far)
{
    /*
        mat4 Result = {
            Near/400, 0.0f, 0.0f, 0.0f,
            0.0f, Near / 300, 0.0f, 0.0f,
            0.0f, 0.0f, -(Far+Near)/(Far-Near), -1.0f,
            0.0f, 0.0f, -2.0f*(Near*Far)/(Far-Near), 0.0f
        };
        return Result;
    */
    
    float yScale = 1.0f / (float)tan(Fov/2.0f);
    float xScale = yScale / Aspect;
    float nearmfar = Near - Far;
    mat4 Result = {
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, (Far+Near)/nearmfar, -1,
        0, 0, 2*Far*Near/nearmfar, 0
    };
    
    return Result;
    /*
        mat4 Result = {
            1.0f / Aspect*(float)tan(Fov/2.0f), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / (float)tan(Fov/2.0f), 0.0f, 0.0f,
            0.0f, 0.0f, -(Far+Near)/(Far-Near), -1.0f,
            0.0f, 0.0f, -2.0f*(Near*Far)/(Far-Near), 0.0f
        };
        return Result;
    */
}

#endif
