#ifndef CAMERA_CPP__
#define CAMERA_CPP__

#include "math.cpp"
#include "matrixMath.cpp"

struct camera
{
    //Projection Stuff
    float FOV;
    float Aspect;
    float Near;
    float Far;
    
    //View stuff
    v3 Position;
    v3 Forward;
    v3 Up;
};

mat4 GenerateCameraOrthographic(camera Camera)
{
    mat4 Projection = MakeOrthographicProjection(6.0f, Camera.Aspect, Camera.Near, Camera.Far);
    return Projection;
}

mat4 GenerateCameraPerspective(camera Camera)
{
    mat4 Projection = MakePerspectiveProjection(Camera.FOV, Camera.Aspect, Camera.Near, Camera.Far);
    return Projection;
}

mat4 GenerateCameraView(camera Camera)
{
    mat4 View = LookAtView(Camera.Position, Camera.Position+Camera.Forward, Camera.Up);
    return View;
}

void CameraStrafe(camera *Camera, float dT, float speed)
{
    Camera->Position = Camera->Position + speed*dT*Normalize(Cross(Camera->Forward, Camera->Up));   
}

void CameraMoveForward(camera *Camera, float dT, float speed)
{
    Camera->Position = Camera->Position + speed*dT*Camera->Forward;
}

void CameraMoveUp(camera *Camera, float dT, float speed)
{
    Camera->Position = Camera->Position + speed*dT*Camera->Up;
}

void TurnCamera(camera* Camera, float dX, float dY, float CameraSpeed)
{
    {
	mat3 XRot = MakeRotation3x3(Camera->Up, PI*dX*CameraSpeed);
	v3 NewForward = XRot*Camera->Forward;
	Camera->Forward = NewForward;
    }

    {
	mat3 YRot = MakeRotation3x3(Cross(Camera->Forward, Camera->Up), PI*dY*CameraSpeed);
	v3 NewForward = YRot*Camera->Forward;
	v3 NewUp = YRot*Camera->Up;
	Camera->Forward = NewForward;
	Camera->Up = NewUp;
    }
}

void RollCamera(camera *Camera, float dT, float speed)
{
    mat3 ZRot = MakeRotation3x3(Camera->Forward, PI*dT*speed);
    v3 NewUp = ZRot*Camera->Up;
    Camera->Up = NewUp;
}

#endif
