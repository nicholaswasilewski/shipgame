#include "matrixMath.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <GL/glx.h>

typedef unsigned int uint;

GLuint LoadShaders(char* vertexShaderFilePath, char* fragmentShaderFilePath)
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    FILE* vertexShaderFile = fopen(vertexShaderFilePath, "r");
    fseek(vertexShaderFile, 0L, SEEK_END);
    int vertexShaderFileLength = ftell(vertexShaderFile);
    rewind(vertexShaderFile);
 
    char* vertexShaderCode = (char*)malloc(vertexShaderFileLength+1);
    fread(vertexShaderCode, 1, vertexShaderFileLength+1, vertexShaderFile);
    vertexShaderCode[vertexShaderFileLength] = '\0';
    fclose(vertexShaderFile);

    GLint result = GL_FALSE;
    int infoLogLength;
    
    glShaderSource(vertexShaderID, 1, &vertexShaderCode, 0);
    glCompileShader(vertexShaderID);
    free(vertexShaderCode);

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
	char* error = (char*)malloc(infoLogLength);
	glGetShaderInfoLog(vertexShaderID, infoLogLength, 0, error);
	printf("%s\n", error);
	free(error);
    }
    char* fragmentShaderCode = 0;
    FILE* fragmentShaderFile = fopen(fragmentShaderFilePath, "r");
    fseek(fragmentShaderFile, 0L, SEEK_END);
    int fragmentShaderFileLength = ftell(fragmentShaderFile);
    rewind(fragmentShaderFile);

    fragmentShaderCode = (char*)malloc(fragmentShaderFileLength+1);
    fread(fragmentShaderCode, 1, fragmentShaderFileLength+1, fragmentShaderFile);
    fragmentShaderCode[fragmentShaderFileLength] = '\0';
    fclose(fragmentShaderFile);

    glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, 0);
    glCompileShader(fragmentShaderID);
    free(fragmentShaderCode);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
	char* error = (char*)malloc(infoLogLength+1);
	glGetShaderInfoLog(fragmentShaderID, infoLogLength, 0, error);
	printf("%s\n", error);
    }

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
	char* error = (char*)malloc(infoLogLength+1);
	glGetProgramInfoLog(programID, infoLogLength, 0, error);
	printf("%s\n", error);
	free(error);
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

float ElapsedMsec(timeval *start, timeval *stop)
{
    return ((stop->tv_sec - start->tv_sec) * 1000.0f +
	    (stop->tv_usec - start->tv_usec) / 1000.0f);
}

GLXFBConfig ChooseFBConfig(Display *display, int screen)
{   
    static const int visualAttributes[] =
    {
	GLX_X_RENDERABLE, True,
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE, GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_STENCIL_SIZE, 8,
	GLX_DOUBLEBUFFER, True,
	GLX_SAMPLE_BUFFERS, 1,
	GLX_SAMPLES, 4,
	None
    };
    int attributes[100];
    memcpy(attributes, visualAttributes, sizeof(visualAttributes));
    GLXFBConfig ret = 0;
    int fbCount;
    GLXFBConfig *fbc = glXChooseFBConfig(display, screen, attributes, &fbCount);
    if (fbc)
    {
	if (fbCount >= 1)
	{
	    ret = fbc[0];
	}
	XFree(fbc);
    }
    return ret;
}

GLXContext CreateContext(Display *display, int screen,
			 GLXFBConfig fbConfig, XVisualInfo *visinfo, Window window)
{
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

    typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    GLXContext ctx_old = glXCreateContext(display, visinfo, 0, True);
    if (!ctx_old)
    {
	printf("Could not allocate an old-style GL context!\n");
	exit(EXIT_FAILURE);
    }

    glXMakeCurrent(display, window, ctx_old);
    if (strstr(glXQueryExtensionsString(display, screen), "GLX_ARB_create_context") != 0)
    {
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
	glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
	if (!glXCreateContextAttribsARB)
	{
	    printf("Can't create new-style GL context\n");
	    exit(EXIT_FAILURE);
	}
    }
    glXMakeCurrent(display, None, 0);
    glXDestroyContext(display, ctx_old);

    static int contextAttributes[] =
    {
	GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
	GLX_CONTEXT_MINOR_VERSION_ARB, 2,
	GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	None
    };

    GLXContext context = glXCreateContextAttribsARB(display, fbConfig, 0, True, contextAttributes);
    XSync(display, False);
    if (!context)
    {
	printf("Failed to allocate a GL context.\n");
	exit(EXIT_FAILURE);
    }
    return context;
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
	else if (error = GL_INVALID_VALUE)
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
	printf("OpenGL error: %d - %s\n", error, msg);
    }
}

void KeyboardCB(KeySym sym, unsigned char key, int x, int y, int* setting_change)
{
    switch(tolower(key))
    {
        case 27:
	{
	    exit(EXIT_SUCCESS);
	} break;
        case 'k':
        {
	    break;
        }
        case 0:
        {
	    switch (sym)
	    {
	        case XK_Left:
	        {
	        } break;
	        case XK_Right:
	        {
	        } break;
	    }
	} break;
    }
}

void ReshapeCB(int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessXEvents(Atom wm_protocols, Atom wm_delete_window, int* displayed, Display* display, Window window)
{
    int setting_change = 0;
    while (XEventsQueued(display, QueuedAfterFlush))
    {
	XEvent event;
	XNextEvent(display, &event);
	if(event.xany.window != window)
	{
	    continue;
	}

	switch(event.type)
	{
	   case MapNotify:
	   {
	       *displayed = 1;
	   } break;
	   case ConfigureNotify:
	   {
	       XConfigureEvent cevent = event.xconfigure;
	       ReshapeCB(cevent.width, cevent.height);
	   } break;
	   case KeyPress:
	   {
	       char chr;
	       KeySym symbol;
	       XComposeStatus status;
	       XLookupString(&event.xkey, &chr, 1, &symbol, &status);
	       KeyboardCB(symbol, chr, event.xkey.x, event.xkey.y, &setting_change);
	   } break;
	   case ClientMessage:
	   {
	       if (event.xclient.message_type == wm_protocols && (Atom)event.xclient.data.l[0] == wm_delete_window)
	       {
		   exit(EXIT_SUCCESS);
	       }
	   } break;
	}
    }
}

int main(int argc, char *argv[])
{
    Display *display;
    XEvent e;

    int WindowWidth = 500;
    int WindowHeight = 500;

    display = XOpenDisplay(0);
    if (!display) 
    {
	fprintf(stderr, "Cannot open display\n");
	exit(1);
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    if (glXQueryExtension(display, 0, 0))
    {
//	printf("X Server doesn't support GLX extension\n");
    }

    GLXFBConfig fbconfig = ChooseFBConfig(display, screen);
    if (!fbconfig)
    {
	printf("Failed to get GLXFBConfig\n");
	exit(EXIT_FAILURE);
    }

    XVisualInfo *visinfo = glXGetVisualFromFBConfig(display, fbconfig);
    if (!visinfo)
    {
	printf("failed to get XVisualInfo\n");
	exit(EXIT_FAILURE);
    }
    XSetWindowAttributes winAttr;
    winAttr.event_mask = StructureNotifyMask | KeyPressMask | ButtonPressMask;
    winAttr.background_pixmap = None;
    winAttr.background_pixel = 0;
    winAttr.border_pixel = 0;
    winAttr.colormap = XCreateColormap(display, root, visinfo->visual, AllocNone);
    unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
    Window window = XCreateWindow(display, root,
				  0, 0,
				  800, 800, 0,
				  visinfo->depth, InputOutput,
				  visinfo->visual, mask, &winAttr);
    XStoreName(display, window, "GLX");
    GLXContext context = CreateContext(display, screen, fbconfig, visinfo, window);
    if (!glXMakeCurrent(display, window, context))
    {
	printf("glxMakeCurrent failed.\n");
    }
    if (!glXIsDirect(display, glXGetCurrentContext()))
    {
	printf("Indirect GLX rendering context obtained.\n");
    }
    XMapWindow(display, window);
    if (!glXMakeCurrent(display, window, context))
    {
	printf("glXMakeCurrent 2 failed.\n");
    }

    Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, True);

    int displayed = 0;

    float targetFPS = 60.0f;
    float msPerFrame = 1000.0f/targetFPS;

    timeval last_xcheck;
    gettimeofday(&last_xcheck, 0);
    timeval now;
    int frameCount = 0;


    glClearColor(0.0, 0.0, 0.4, 0.0);
    glFrontFace(GL_CCW); 
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat vertexBufferData[] =
    {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
    };

    GLfloat colorBufferData[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	
    };
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);
    
    GLuint programID = LoadShaders("vertexShader.vert", "fragmentShader.frag");
    GLuint matrixID = glGetUniformLocation(programID, "MVP");

//    mat4 Projection = MakeOrthoProjection(2.0f, 2.0f, 0.1f, 1000.0f);
    mat4 Projection = MakePerspectiveProjection(3.14f*0.25f, 1.0f, 0.1f, 1000.0f);

    v3 CameraPosition = V3(0.0f,0.0f,5.0f);
    v3 CameraTarget = V3(0,0,-1);
    v3 CameraUp = V3(0,1,0);
    mat4 View = LookAtView(CameraPosition, CameraTarget, CameraUp);

    mat4 Model = Identity4x4();
    v3 ModelAxis = V3(0.0f, 1.0f, 0.0f);
    float Angle = 3.14159 * .25f;
    mat4 Rotation = MakeRotation(ModelAxis, Angle);
    mat4 Scale = MakeScale(V3(1.0f, 1.0f, 1.0f));
    mat4  Translation = MakeTranslation(V3(0.0f, 0.0f, 0.0f));
    Model = Translation * Rotation * Scale;
    mat4 MyMVP = Projection * View * Model;
	
    while(1)
    {
	if (displayed)
	{
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    
	    glUseProgram(programID);
#define ME 1
#if !ME
	    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
#else
	    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MyMVP.E[0][0]);
#endif
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableVertexAttribArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	    glVertexPointer(3, GL_FLOAT, 0, 0);
	    glVertexAttribPointer(0,
				  3,
				  GL_FLOAT,
				  GL_FALSE,
				  0,
				  (void*)0
		);

	    glEnableVertexAttribArray(1);
	    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	    glVertexAttribPointer(1,
				  3,
				  GL_FLOAT,
				  GL_FALSE,
				  0,
				  (void*)0);

	    glDrawArrays(GL_TRIANGLES, 0, 3);
	    glDisableVertexAttribArray(0);
	    glXSwapBuffers(display, window);
	}

	ProcessXEvents(wm_protocols, wm_delete_window, &displayed, display, window);
	float elapsedTime = 0;

	while(elapsedTime < msPerFrame)
	{
	    gettimeofday(&now, 0);
	    elapsedTime = ElapsedMsec(&last_xcheck, &now);
	    if (elapsedTime <= msPerFrame)
	    {
		sleep((msPerFrame - elapsedTime)/1000.0f);
	    }
	}

	last_xcheck = now;
    }

    return EXIT_SUCCESS;
}
