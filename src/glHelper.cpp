#ifndef GLHELPER_CPP__
#define GLHELPER_CPP__

#include "matrixMath.cpp"

#if defined(LINUX)
#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#elif defined(WINDOWS)
#define GLDECL WINAPI
#include <GL\gl.h>

typedef char GLchar;

#define GLDefineProc(retType, procName, ...) typedef retType GLDECL procName##GLProc(__VA_ARGS__); static procName##GLProc * gl##procName;
GLDefineProc(void, Uniform3f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLDefineProc(void, Uniform4f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLDefineProc(GLuint, GetUniformLocation, GLuint Program, const char *name);
GLDefineProc(void, Uniform1f, GLint location, GLfloat v0);
    
#endif

static void LoadExtensions()
{
    wglGetProcAddress("wglGetExtensionsStringARB");
}

void glUniformVec3f(GLuint location, v3 vec)
{
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void glUniformVec4f(GLuint location, v4 vec)
{
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

#endif
