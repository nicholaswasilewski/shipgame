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
	float E[16];
    };
} matrix4x4, m4x4;

typedef struct Vector3
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

typedef struct Vector4
{
    union
    {
	struct
	{
	    float x, y, z, w;
	};
	float E[4];
    };
} vector4, v4, quaternion;

v3 V3(float x, float y, float z)
{
    v3 Result;
    Result.x = x;
    Result.y = y;
    Result.z = z;
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

Matrix4x4 MakeTranslation(Vector3 v)
{
    Matrix4x4 Result = {
	1.0f, 0.0f, 0.0f, v.x,
	0.0f, 1.0f, 0.0f, v.y,
	0.0f, 0.0f, 1.0f, v.z,
	0.0f, 0.0f, 0.0f, 1.0f
    };
    return Result;
}

Matrix4x4 MakeScale(Vector3 v)
{
    Matrix4x4 Result = {
	v.x, 0.0f, 0.0f, 0.0f,
	0.0f, v.y, 0.0f, 0.0f,
	0.0f, 0.0f, v.z, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
    };
    return Result;
}

Matrix4x4 operator*(Matrix4x4 m1, Matrix4x4 m2)
{
    float x0 = m1.x0*m2.x0 + m1.y0*m2.x1 + m1.z0*m2.x2 + m1.w0*m2.x3;
    float y0 = m1.x0*m2.y0 + m1.y0*m2.y1 + m1.z0*m2.y2 + m1.w0*m2.y3;
    float z0 = m1.x0*m2.z0 + m1.y0*m2.z1 + m1.z0*m2.z2 + m1.w0*m2.z3;
    float w0 = m1.x0*m2.w0 + m1.y0*m2.w1 + m1.z0*m2.w2 + m1.w0*m2.w3;
    
    float x1 = m1.x1*m2.x0 + m1.y1*m2.x1 + m1.z1*m2.x2 + m1.w1*m2.x3;
    float y1 = m1.x1*m2.y0 + m1.y1*m2.y1 + m1.z1*m2.y2 + m1.w1*m2.y3;
    float z1 = m1.x1*m2.z0 + m1.y1*m2.z1 + m1.z1*m2.z2 + m1.w1*m2.z3;
    float w1 = m1.x1*m2.w0 + m1.y1*m2.w1 + m1.z1*m2.w2 + m1.w1*m2.w3;
    
    float x2 = m1.x2*m2.x0 + m1.y2*m2.x1 + m1.z2*m2.x2 + m1.w2*m2.x3;
    float y2 = m1.x2*m2.y0 + m1.y2*m2.y1 + m1.z2*m2.y2 + m1.w2*m2.y3;
    float z2 = m1.x2*m2.z0 + m1.y2*m2.z1 + m1.z2*m2.z2 + m1.w2*m2.z3;
    float w2 = m1.x2*m2.w0 + m1.y2*m2.w1 + m1.z2*m2.w2 + m1.w2*m2.w3;
    
    float x3 = m1.x3*m2.x0 + m1.y3*m2.x1 + m1.z3*m2.x2 + m1.w3*m2.x3;
    float y3 = m1.x3*m2.y0 + m1.y3*m2.y1 + m1.z3*m2.y2 + m1.w3*m2.y3;
    float z3 = m1.x3*m2.z0 + m1.y3*m2.z1 + m1.z3*m2.z2 + m1.w3*m2.z3;
    float w3 = m1.x3*m2.w0 + m1.y3*m2.w1 + m1.z3*m2.w2 + m1.w3*m2.w3;
    Matrix4x4 Result = {
	x0, y0, z0, w0,
	x1, y1, z1, w1,
	x2, y2, z2, w2,
	x3, y3, z3, w3
    };

    return Result;
}

Vector4 operator*(Matrix4x4 m, Vector4 v)
{
    v4 Result;
    Result.x = m.x0*v.x + m.y0*v.y + m.z0*v.z + m.w0*v.w;
    Result.y = m.x1*v.x + m.y1*v.y + m.z1*v.z + m.w1*v.w;
    Result.z = m.x2*v.x + m.y2*v.y + m.z2*v.z + m.z3*v.w;
    Result.w = m.x3*v.x + m.y3*v.y + m.z3*v.z + m.w3*v.w;

    return Result;
}

Matrix4x4 Identity4x4() {
    Matrix4x4 Result = MakeScale(V3(1.0f, 1.0f, 1.0f));
    return Result;
}


