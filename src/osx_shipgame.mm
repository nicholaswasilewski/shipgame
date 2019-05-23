#define OSX

#import <Cocoa/Cocoa.h>

#define GL_SILENCE_DEPRECATION

#include "platform.h"
#include "glHelper.cpp"
#include "game.cpp"

struct osx_state
{
    bool Running;
};

osx_state State;

@interface OpenGLView : NSOpenGLView {}
@end
@implementation OpenGLView
@end

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end
@implementation AppDelegate

-(void)windowWillClose:(id)sender
{
    State.Running = false;
}
@end

int main(int argc, char** argv)
{
  @autoreleasepool
  {
    NSApplication *app = [NSApplication sharedApplication];
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    State.Running = true;
    app.delegate = appDelegate;
    [app finishLaunching];

    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect frame = NSMakeRect(0, 0, 1280, 720);
    NSRect windowRect = NSMakeRect(NSMidX(screenRect) - NSMidX(frame),
				   NSMidY(screenRect) - NSMidY(frame),
				   frame.size.width, frame.size.height);
    NSUInteger styleMask =
	NSWindowStyleMaskTitled |
	NSWindowStyleMaskResizable |
	NSWindowStyleMaskClosable |
	NSWindowStyleMaskMiniaturizable;
    NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
					 styleMask:styleMask
					 backing:NSBackingStoreBuffered
					 //TODO: find out what this does.
					 defer:NO];
    window.delegate = appDelegate;

    NSOpenGLPixelFormatAttribute openGLAttributes[] =
    {
      NSOpenGLPFAAccelerated,
      NSOpenGLPFADoubleBuffer,
      NSOpenGLPFAColorSize, 24,
      NSOpenGLPFAAlphaSize, 8,
      NSOpenGLPFADepthSize, 24,
      NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
      0
    };
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:openGLAttributes];
    OpenGLView *view = [[OpenGLView alloc] initWithFrame:frame pixelFormat:pixelFormat];
    [pixelFormat release];
    
    NSOpenGLContext* context = [view openGLContext];
    [context makeCurrentContext];
    
    [window.contentView addSubview:view];
    [window setTitle:@"OpenGL"];
    [window makeKeyAndOrderFront:nil];
    while (true)
    {
	NSEvent* event;
	while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
			       untilDate:nil
			       inMode:NSDefaultRunLoopMode
			       dequeue:YES]))
	{
	    [NSApp sendEvent:event];
	}
	if (!State.Running)
	{
	    break;
	}
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	[context flushBuffer];
    }
  }
  return 0;
}
