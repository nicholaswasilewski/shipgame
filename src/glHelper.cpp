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

#elif defined(WINDOWS)

#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506

#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1

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

#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41

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

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042

#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001


#define GLDECL WINAPI
#include <GL\gl.h>
#include <GL\glu.h>

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
    GLE(void, RenderbufferStorageMultisample, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) \
    GLE(void, FramebufferRenderbuffer, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)

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
#endif

#define GLE(retType, procName, ...) typedef retType GLDECL procName##GLProc(__VA_ARGS__); static procName##GLProc * gl##procName;
GLExtensionList

GLE(GLubyte*, GetStringi, GLenum name, GLuint index)
#undef GLE

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

struct FramebufferDesc
{
    GLuint DepthBufferId;
    GLuint RenderTextureId;
    GLuint RenderFramebufferId;
    GLuint ResolveTextureId;
    GLuint ResolveFramebufferId;
};

void glUniformVec3f(GLuint location, v3 vec)
{
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void glUniformVec4f(GLuint location, v4 vec)
{
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void GLErrorShow()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
	char* msg;
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
	DebugLog("OpenGL error: %d - %s\n", error, msg);
    }
}

#endif
