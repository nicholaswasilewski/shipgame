#include "game.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

typedef unsigned int uint;

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

float ElapsedMilliseconds(timeval *start, timeval *stop)
{
    return ((stop->tv_sec - start->tv_sec) * 1000.0f +
	    (stop->tv_usec - start->tv_usec) / 1000.0f);
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

void KeyboardCB(KeySym sym, unsigned char key, bool32 Press, controller* Keyboard)
{
    switch(tolower(key))
    {
    //esc key
    case 27:
    {
	exit(EXIT_SUCCESS);
    } break;
    case 'k':
    {
	break;
    }
    case 'w':
    {
	Keyboard->Up.Down = Press;
    } break;
    case 'a':
    {
	Keyboard->Left.Down = Press;
    } break;
    case 's':
    {
	Keyboard->Down.Down = Press;
    } break;
    case 'd':
    {
	Keyboard->Right.Down = Press;
    } break;
    case 0:
    {
	switch (sym)
	{
	case XK_Left:
	{
	    Keyboard->Left.Down = Press;
	} break;
	case XK_Right:
	{
	    Keyboard->Right.Down = Press;
	} break;
	case XK_Up:
	{
	    Keyboard->Up.Down = Press;
	} break;
        case XK_Down:
	{
	    Keyboard->Down.Down = Press;
        } break;
        }
    } break;
    default:
    {
    } break;
    }
}

void ReshapeCB(int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessXEvents(input* Input, Atom wm_protocols, Atom wm_delete_window, int* displayed, Display* display, Window window, int WindowWidth, int WindowHeight)
{
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
	    KeyboardCB(symbol, chr, 1, &Input->Keyboard);
	} break;
	case KeyRelease:
	{
	    char chr;
	    KeySym symbol;
	    XComposeStatus status;
	    XLookupString(&event.xkey, &chr, 1, &symbol, &status);
	    KeyboardCB(symbol, chr, 0, &Input->Keyboard);
	} break;
	case MotionNotify:
	{
	    XMotionEvent motionEvent = event.xmotion;
	    
	    Window focus_return;
	    int revert_to_return;
	    XGetInputFocus(display, &focus_return, &revert_to_return);
	    if (focus_return == window)
	    {
		if (motionEvent.x != WindowWidth/2 || motionEvent.y != WindowHeight/2)
		{
		    Input->Keyboard.RStick.X = motionEvent.x - WindowWidth/2;
		    Input->Keyboard.RStick.Y = motionEvent.y - WindowHeight/2;
		    XWarpPointer(display, 0, window, 0, 0, 0, 0,
				 WindowWidth/2, WindowHeight/2);
		}
	    }
	} break;
	case FocusIn:
	{
	    
	} break;
	case FocusOut:
	{
	    
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

    int WindowWidth = 800;
    int WindowHeight = 600;

    display = XOpenDisplay(0);
    if (!display) 
    {
	fprintf(stderr, "Cannot open display\n");
	exit(1);
    }
    XkbSetDetectableAutoRepeat(display, 0, 0);

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
    winAttr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask
	| ButtonPressMask | ButtonRelease | PointerMotionMask
	| FocusChangeMask;
    winAttr.background_pixmap = None;
    winAttr.background_pixel = 0;
    winAttr.border_pixel = 0;
    winAttr.colormap = XCreateColormap(display, root, visinfo->visual, AllocNone);
    unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
    Window window = XCreateWindow(display, root,
				  0, 0,
				  WindowWidth, WindowHeight, 0,
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


    input Input0 = {0};
    input Input1 = {0};
    
    platform_data PlatformData = {0};
    PlatformData.LastInput = &Input0;
    PlatformData.NewInput = &Input1;

    PlatformData.MainMemorySize = GIGABYTES(1);
    PlatformData.MainMemory = calloc(1, PlatformData.MainMemorySize);
    PlatformData.TempMemorySize = MEGABYTES(512);
    PlatformData.TempMemory = malloc(PlatformData.TempMemorySize);

    XWarpPointer(display, 0, window, 0, 0, 0, 0,
		 WindowWidth/2, WindowHeight/2);
    while(1)
    {
	PlatformData.NewInput->Keyboard.RStick.X = 0.0f;
	PlatformData.NewInput->Keyboard.RStick.Y = 0.0f;
	ProcessXEvents(PlatformData.NewInput,
		       wm_protocols,
		       wm_delete_window,
		       &displayed,
		       display,
		       window,
		       WindowWidth,
		       WindowHeight);
	
	UpdateAndRender(&PlatformData);
	glXSwapBuffers(display, window);
	
	float elapsedTime = 0;

	while(elapsedTime < msPerFrame)
	{
	    gettimeofday(&now, 0);
	    elapsedTime = ElapsedMilliseconds(&last_xcheck, &now);
	    if (elapsedTime <= msPerFrame)
	    {
		sleep((msPerFrame - elapsedTime)/1000.0f);
	    }
	}

	
	*PlatformData.LastInput = *PlatformData.NewInput;
	PlatformData.NewInput->dT = elapsedTime/1000.0f;
	last_xcheck = now;
    }

    return EXIT_SUCCESS;
}
