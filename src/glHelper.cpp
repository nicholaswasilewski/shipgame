#ifndef GLHELPER_CPP__
#define GLHELPER_CPP__

#include "matrixMath.cpp"

#if defined(LINUX)
#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#elif defined(WINDOWS)

#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506

#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#define GL_COMPRESSED_RGBA_BPTC_UNORM     0x8E8C

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

#define GL_STATIC_DRAW                    0x88E4

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF

#define GLDECL WINAPI
#include <GL\gl.h>

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;

#define GLExtensionList \
    GLE(GLint, GetUniformLocation, GLuint program, const GLchar *name) \
    
    GLE(void, Uniform1i, GLint location, GLint v0) \
    GLE(void, Uniform1f, GLint location, GLfloat v0) \
    GLE(void, Uniform3f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) \
    GLE(void, Uniform4f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
    GLE(void, UniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    
    GLE(void, CompressedTexImage2D, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data) \
    GLE(void, GenerateMipmap, GLenum target) \
    
    GLE(GLuint, CreateShader, GLenum type) \
    GLE(void, DeleteShader, GLuint shader) \
    GLE(void, ShaderSource, GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) \
    GLE(void, CompileShader, GLuint shader) \
    GLE(void, AttachShader, GLuint program, GLuint shader) \
    GLE(void, DetachShader, GLuint program, GLuint shader) \
    GLE(void, GetShaderiv, GLuint shader, GLenum pname, GLint *params) \
    GLE(void, GetShaderInfoLog, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    
    GLE(GLuint, CreateProgram) \
    GLE(void, DeleteProgram, GLuint program) \
    GLE(void, LinkProgram, GLuint program) \
    GLE(void, GetProgramiv, GLuint program, GLenum pname, GLint *params) \
    GLE(void, GetProgramInfoLog, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    GLE(void, UseProgram, GLuint program) \
    
    GLE(void, GenVertexArrays, GLsizei n, GLuint *arrays) \
    GLE(void, BindVertexArray, GLuint array) \
    GLE(void, EnableVertexAttribArray, GLuint index) \
    GLE(void, DisableVertexAttribArray, GLuint index) \
    
    GLE(void, GenBuffers, GLsizei n, GLuint *buffers) \
    GLE(void, BindBuffer, GLenum target, GLuint buffer) \
    GLE(void, DeleteBuffers, GLsizei n, const GLuint *buffer) \
    GLE(void, BufferData, GLenum target, GLsizeiptr size, const void *data, GLenum usage) \
    
    GLE(void, VertexAttribPointer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) \
    GLE(void, ActiveTexture, GLenum texture)

#define GLE(retType, procName, ...) typedef retType GLDECL procName##GLProc(__VA_ARGS__); static procName##GLProc * gl##procName;
GLExtensionList
    
#endif

static void LoadExtensions()
{
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
