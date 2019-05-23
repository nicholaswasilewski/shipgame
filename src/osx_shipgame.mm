#define OSX

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <mach/mach_init.h>
#import <mach/vm_map.h>

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
- (BOOL)acceptsFirstResponder {
    return YES;
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end
@implementation AppDelegate

-(void)windowWillClose:(id)sender
{
    State.Running = false;
}
@end

void HandleKeyEvent(unsigned short keyCode, bool down) {
    switch (keyCode) {
	case kVK_Escape: {
	    State.Running = false;
	} break;
    }
}

void HandleEvent(NSEvent *event) {
    switch(event.type)
    {
	case NSEventTypeKeyDown: {
	    NSLog(@"keydown");
	    //HandleKeyEvent(event.keyCode, false);
	} break;
	case NSEventTypeKeyUp: {
	    //HandleKeyEvent(event.keyCode, true);
	} break;
	default: {

	    [NSApp sendEvent:event];
	} break;
    }
}

int main(int argc, char** argv)
{
  @autoreleasepool
  {
    NSApplication *app = [NSApplication sharedApplication];
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
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

    platform_data PlatformData = {0};
    PlatformData.MainMemorySize = MEGABYTES(32);
    PlatformData.TempMemorySize = MEGABYTES(32);
    PlatformData.TotalMemorySize = PlatformData.MainMemorySize + 
	PlatformData.TempMemorySize;
    vm_address_t address = 0;
    kern_return_t result = vm_allocate((vm_map_t)mach_task_self(),
				       &address,
				       PlatformData.TotalMemorySize,
				       true /* allocate anywhere */);
    if (result == KERN_SUCCESS) {
	NSLog(@"Successful allocation at 0x%lx.", address);
    } else if (result == KERN_INVALID_ADDRESS) {
	NSLog(@"Tried to allocate invalid address 0x%lx.", address);
	return 0;
    }

    PlatformData.MainMemory = (uint8 *)address;
    PlatformData.TempMemory = (uint8 *)PlatformData.MainMemory + PlatformData.MainMemorySize; 
    
    input Inputs[2];
    Inputs[0] = {0};
    Inputs[1] = {0};
    input *NewInput = &Inputs[0];
    input *LastInput = &Inputs[1];
    PlatformData.LastInput = LastInput;
    PlatformData.NewInput = NewInput;
    NewInput->dT = 0.0f;
    State.Running = true;

    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    while (true)
    {
	NSEvent* event;
	while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
			       untilDate:nil
			       inMode:NSDefaultRunLoopMode
			       dequeue:YES]))
	{
	    HandleEvent(event);
	}
	if (!State.Running)
	{
	    break;
	}

	UpdateAndRender(&PlatformData);
	[context flushBuffer];
    }
  }
  return 0;
}
