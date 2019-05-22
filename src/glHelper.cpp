#ifndef GLHELPER_CPP__
#define GLHELPER_CPP__

#include "matrixMath.cpp"

#if defined(LINUX)
#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>

void* GetGLFuncAddress(const char* name)
{
    return (void*)0;
}

#elif defined(OSX)
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#define GL_COMPRESSED_RGBA_BPTC_UNORM     0x8E8C

#elif defined(WINDOWS)
#include <GL\gl.h>
#include <GL\glu.h>

#include "wglext.h"

// For some reason glext.h includes khrplatform.h.
// I've chosen to just take the things I need out of glext instead.
// Someday this is going to become a problem.
// #include "glext.h"

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;

#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506

#define GL_MULTISAMPLE                    0x809D

#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
#define GL_PROGRAM_POINT_SIZE             0x8642
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#define GL_COMPRESSED_RGBA_BPTC_UNORM     0x8E8C

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84

#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_GEOMETRY_VERTICES_OUT          0x8916
#define GL_GEOMETRY_INPUT_TYPE            0x8917
#define GL_GEOMETRY_OUTPUT_TYPE           0x8918

#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88

#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

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
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C

#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT            0x8218
#define GL_FRAMEBUFFER_UNDEFINED          0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_MAX_RENDERBUFFER_SIZE          0x84E8
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_TEXTURE_STENCIL_SIZE           0x88F1
#define GL_TEXTURE_RED_TYPE               0x8C10
#define GL_TEXTURE_GREEN_TYPE             0x8C11
#define GL_TEXTURE_BLUE_TYPE              0x8C12
#define GL_TEXTURE_ALPHA_TYPE             0x8C13
#define GL_TEXTURE_DEPTH_TYPE             0x8C16
#define GL_UNSIGNED_NORMALIZED            0x8C17
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING       0x8CA6
#define GL_RENDERBUFFER_BINDING           0x8CA7
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING       0x8CAA
#define GL_RENDERBUFFER_SAMPLES           0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_FRAMEBUFFER_SRGB               0x8DB9

#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_NUM_EXTENSIONS                 0x821D
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100

#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

#define GLDECL WINAPI

#define GLExtensionList \
GLE(GLint, GetUniformLocation, GLuint program, const GLchar *name) \
GLE(void, Uniform1i, GLint location, GLint v0) \
GLE(void, Uniform1f, GLint location, GLfloat v0) \
GLE(void, Uniform1fv, GLint location, GLsizei count, const GLfloat* value) \
GLE(void, Uniform2fv, GLint location, GLsizei count, const GLfloat* value) \
GLE(void, Uniform3f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) \
GLE(void, Uniform3fv, GLint location, GLsizei count, const GLfloat* value) \
GLE(void, Uniform4f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
GLE(void, UniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
GLE(void, CompressedTexImage2D, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data) \
GLE(void, TexImage2DMultisample, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) \
GLE(void, TextureParameteri, GLuint texture, GLenum pname, GLint param) \
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
GLE(void, NamedBufferSubData, GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data) \
GLE(void, VertexAttribPointer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) \
GLE(void, ActiveTexture, GLenum texture) \
GLE(void, GenFramebuffers, GLsizei n, GLuint *framebuffers) \
GLE(void, FramebufferTexture2D, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
GLE(GLenum, CheckFramebufferStatus, GLenum target) \
GLE(void, BindFramebuffer, GLenum target, GLuint framebuffer) \
GLE(void, DeleteFramebuffers, GLsizei n, const GLuint *framebuffers) \
GLE(void, GenRenderbuffers, GLsizei n, GLuint *renderbuffers) \
GLE(void, BindRenderbuffer, GLenum target, GLuint renderbuffer) \
GLE(void, DeleteRenderbuffers, GLsizei n, const GLuint *renderbuffers) \
GLE(void, RenderbufferStorage, GLenum target, GLenum internalformat, GLsizei width, GLsizei height) \
GLE(void, RenderbufferStorageMultisample, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) \
GLE(void, FramebufferRenderbuffer, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) \
GLE(void, BlitFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) \
GLE(void, BlitNamedFramebuffer, GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)

void *GetGLFuncAddress(const char *name)
{
    void *p = (void *)wglGetProcAddress(name);
    if (p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1))
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void *)GetProcAddress(module, name);
    }
    
    return p;
}

/*The Pain*/
typedef HGLRC GLDECL CreateContextAttribsARBGLProc (HDC hDC, HGLRC hShareContext, const int *attribList);
static CreateContextAttribsARBGLProc *wglCreateContextAttribsARB;

typedef BOOL GLDECL ChoosePixelFormatARBGLProc (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
static ChoosePixelFormatARBGLProc *wglChoosePixelFormatARB;

void LoadWGLBullshit()
{
    wglCreateContextAttribsARB = (CreateContextAttribsARBGLProc*)GetGLFuncAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (ChoosePixelFormatARBGLProc*)GetGLFuncAddress("wglChoosePixelFormatARB");
}

// A macro that defines a type of a Gl function and creates a variable that is a function pointer to a function with that signature.
#define GLE(retType, procName, ...) typedef retType GLDECL procName##GLProc(__VA_ARGS__); static procName##GLProc * gl##procName;
GLExtensionList

// Make typedefs/function pointer vars that we will only be used sometimes.
GLE(GLubyte*, GetStringi, GLenum name, GLuint index)
#undef GLE

// A macro that gets the address of a Gl function and assigns it to the variable declared by the previously defined macro.
#define GLE(retType, procName, ...) gl##procName = (procName##GLProc*)GetGLFuncAddress( "gl" #procName );

static void LoadGLExtensions()
{
    GLExtensionList
#undef GLExtensionList
}

static void PrintGLVersion()
{
    int majorV, minorV, numGLExtensions;
    glGetIntegerv(GL_MAJOR_VERSION, &majorV);
    glGetIntegerv(GL_MINOR_VERSION, &minorV);
    printf("GL VERSION NUMBER: %d.%d\n", majorV, minorV);   
}
static void PrintAvailableGLExtensions()
{   
    GLE(Glubyte*, GetStringi, GLenum name, GLuint index);
    int numGLExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numGLExtensions);
    printf("%d\n", numGLExtensions);
    for(int i = 0; i < numGLExtensions; i++)
    {
        printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
    }
}
#undef GLE

#endif //defined(WINDOWS)

void glUniformVec3f(GLuint location, v3 vec)
{
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void glUniformVec4f(GLuint location, v4 vec)
{
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void GLErrorShowOne(const char* file, int line, GLenum error) {
    const char* msg;
    if (error == GL_INVALID_OPERATION)
    {
        msg = "Invalid Operation";
    }
    else if (error == GL_INVALID_ENUM)
    {
        msg = "Invalid enum";
    }
    else if (error == GL_INVALID_VALUE)
    {
        msg = "Invalid value";
    }
    else if (error == GL_OUT_OF_MEMORY)
    {
        msg = "Out of memory";
    }
    else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
    {
        msg = "Invalid framebuffer operation";
    }
    else
    {
        msg = "Unknown GL error";
    }
    printf("OpenGL error: %s:%d - %d - %s\n", file, line, error, msg);
}

#define GLErrorShow() GLErrorShowInternal(__FILE__, __LINE__)

void GLErrorShowInternal(const char* file, int line)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        GLErrorShowOne(file, line, error);
    }
}

#define GL(stmt) stmt; \
{ \
    GLenum error; \
    while ((error = glGetError()) != GL_NO_ERROR) \
    { \
        const char* file = __FILE__; \
        int line = __LINE__; \
        printf("-----------\n"); \
        printf("GL Error: "#stmt "\n"); \
        GLErrorShowOne(file, line, error); \
        printf("-----------\n"); \
    } \
}



void PrintGlFBOError()
{
    const char* msg;
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status == GL_FRAMEBUFFER_COMPLETE)
    {
        msg = "GL_FRAMEBUFFER_COMPLETE";
    }
    else if(status == GL_FRAMEBUFFER_UNDEFINED)
    {
        msg = "GL_FRAMEBUFFER_UNDEFINED";
    }
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    }
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    }
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER )
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    }
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER )
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    }
    else if(status == GL_FRAMEBUFFER_UNSUPPORTED )
    {
        msg = "GL_FRAMEBUFFER_UNSUPPORTED";
    }
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE )
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    }
    else if(status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    }
    else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
    {
        msg = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
    }
    printf("OpenGL FBO status: %s\n", msg);
}

#endif
