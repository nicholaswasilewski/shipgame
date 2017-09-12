#ifndef _VRHELPER_CPP__

#include "glHelper.cpp"

#include <openvr.h>

char* TrackedDeviceClassToEnglishDescription(vr::ETrackedDeviceClass trackedDeviceClass)
{
    char* Result;
    switch(trackedDeviceClass)
    {
    case vr::TrackedDeviceClass_HMD:
	Result = "TrackedDeviceClass_HMD";
	break;
    case vr::TrackedDeviceClass_Controller:
	Result = "TrackedDeviceClass_Controller";
	break;
    case vr::TrackedDeviceClass_TrackingReference:
	Result = "TrackedDeviceClass_TrackingReference";
	break;
    case vr::TrackedDeviceClass_DisplayRedirect:
	Result = "TrackedDeviceClass_DisplayRedirect";
	break;
    case vr::TrackedDeviceClass_Invalid:
    default:
	Result = "TrackedDeviceClass_Invalid";
	break;
    }

    return Result;
}

bool CreateFramebuffer(int Width, int Height, FramebufferDesc *BufferDesc)
{
    glGenFramebuffers(1, &BufferDesc->RenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, BufferDesc->RenderFramebufferId);
    glGenRenderbuffers(1, &BufferDesc->DepthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, BufferDesc->DepthBufferId);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, Width, Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, BufferDesc->DepthBufferId);
    
    glGenTextures(1, &BufferDesc->RenderTextureId);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, BufferDesc->RenderTextureId);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, Width, Height, 1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, BufferDesc->RenderTextureId, 0);

    glGenFramebuffers(1, &BufferDesc->ResolveFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, BufferDesc->ResolveFramebufferId);
    glGenTextures(1, &BufferDesc->ResolveTextureId);
    glBindTexture(GL_TEXTURE_2D, BufferDesc->ResolveTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BufferDesc->ResolveTextureId, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
	return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return true;
}

bool SetupStereoRenderTargets(vr::IVRSystem *VRSystem, uint32* RenderWidth, uint32* RenderHeight, FramebufferDesc *LeftEyeDesc, FramebufferDesc *RightEyeDesc)
{
    if (!VRSystem)
	return false;
    VRSystem->GetRecommendedRenderTargetSize(RenderWidth, RenderHeight);
    return CreateFramebuffer(*RenderWidth, *RenderHeight, LeftEyeDesc) &&
	CreateFramebuffer(*RenderWidth, *RenderHeight, RightEyeDesc);
}


#define _VRHELPER_CPP__
#endif
