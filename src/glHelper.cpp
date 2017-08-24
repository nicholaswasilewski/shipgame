#ifndef GLHELPER_CPP__
#define GLHELPER_CPP__

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include "matrixMath.cpp"


void glUniformVec3f(GLuint location, v3 vec)
{
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void glUniformVec4f(GLuint location, v4 vec)
{
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

#endif
