#define OSX

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <mach/mach.h>
#import <mach/mach_init.h>
#import <mach/mach_time.h>
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
-(BOOL)acceptsFirstResponder { return YES; }
-(BOOL)canBecomeKeyView { return YES; }
-(void)keyDown:(NSEvent*)event { NSLog(@"key down"); }
@end

@interface AppWindow : NSWindow
@end
@implementation AppWindow
-(BOOL)canBecomeMainWindow { return YES; }
@end

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end
@implementation AppDelegate

-(void)windowWillClose:(id)sender
{
    State.Running = false;
}
@end

float OSXGetSecondsElapsed(uint64 StartMachTime, uint64 EndMachTime)
{

    uint64 ElapsedMachTime = EndMachTime - StartMachTime;
    struct mach_timebase_info TimebaseInfo;
    mach_timebase_info(&TimebaseInfo);
    uint64 ElapsedNanos = ElapsedMachTime * (TimebaseInfo.numer/ TimebaseInfo.denom);

    return ElapsedNanos/1000000000.0f;
}

void OSXProcessKeyboardMessage(button_state *ButtonState, bool DownState) 
{
	ButtonState->Down = DownState;
}

void HandleKeyEvent(unsigned short keyCode, bool Down, controller *Controller) 
{
    switch (keyCode) 
    {
		case kVK_Escape: {
			State.Running = false;
		} break;
		case kVK_ANSI_W: {
			OSXProcessKeyboardMessage(&Controller->Forward, Down);
		} break;
		case kVK_ANSI_A: {
			OSXProcessKeyboardMessage(&Controller->Left, Down);
		} break;
		case kVK_ANSI_S: {
			OSXProcessKeyboardMessage(&Controller->Back, Down);
		} break;
		case kVK_ANSI_D: {
			OSXProcessKeyboardMessage(&Controller->Right, Down);
		} break;
		case kVK_ANSI_Q: {
			OSXProcessKeyboardMessage(&Controller->UpperLeft, Down);
		} break;
		case kVK_ANSI_E: {
			OSXProcessKeyboardMessage(&Controller->UpperRight, Down);
		} break;
		case kVK_Shift: {
			OSXProcessKeyboardMessage(&Controller->Up, Down);
		} break;
		case kVK_Control: {
			OSXProcessKeyboardMessage(&Controller->Down, Down);
		} break;
    }
}

void HandleEvent(NSEvent *event, controller *KeyboardController) 
{
    switch(event.type)
    {
		case NSEventTypeKeyDown:
		case NSEventTypeKeyUp: {
			HandleKeyEvent(event.keyCode, event.type==NSEventTypeKeyDown, KeyboardController);
		} break;
	    case NSEventTypeFlagsChanged: {
			NSUInteger ModifierFlag;
			switch(event.keyCode)
			{
				case kVK_Shift: {
					ModifierFlag = NSShiftKeyMask;
				} break;
				case kVK_Control: {
					ModifierFlag = NSEventModifierFlagControl;
				} break;
				case kVK_Option: {
					ModifierFlag = NSEventModifierFlagOption;
				} break;
				default: {
					ModifierFlag = 0;
				} break;
			}
			HandleKeyEvent(event.keyCode, event.modifierFlags & ModifierFlag, KeyboardController);
		} break;
		case NSEventTypeLeftMouseDown: {
		} break;
		case NSEventTypeLeftMouseUp: {
		} break;
		case NSEventTypeRightMouseDown: {
		} break;
		case NSEventTypeRightMouseUp: {
		} break;
		case NSEventTypeMouseMoved: {
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
		ProcessSerialNumber psn;
		if (!GetCurrentProcess(&psn)) {
			TransformProcessType(&psn, kProcessTransformToForegroundApplication);
			SetFrontProcess(&psn);
		}
			
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
		NSWindow *window = [[AppWindow alloc] initWithContentRect:frame
											  styleMask:styleMask
											  backing:NSBackingStoreBuffered
											  //TODO: find out what this does.
											  defer:NO];
		[window setAcceptsMouseMovedEvents:YES];
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
		
		PlatformData.WindowWidth = frame.size.width;
		PlatformData.WindowHeight = frame.size.height;
		
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
		uint64 LastTime = mach_absolute_time();
		while (true)
		{
			NSEvent* event;
			while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
								   untilDate:nil
								   inMode:NSDefaultRunLoopMode
								   dequeue:YES]))
			{
				HandleEvent(event, &NewInput->Keyboard);
			}
			if (!State.Running)
			{
				break;
			}
			
			UpdateAndRender(&PlatformData);
			[context flushBuffer];
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			
			uint64 TimeNow;
			float FrameSecondsElapsed = 0;
			float TargetFrameSeconds = 1.0f / 60.0f;
			while(FrameSecondsElapsed < TargetFrameSeconds) 
			{
				TimeNow = mach_absolute_time();
				FrameSecondsElapsed = OSXGetSecondsElapsed(LastTime, TimeNow);
				if (FrameSecondsElapsed < TargetFrameSeconds)
				{
					float TimeToSleep = TargetFrameSeconds - FrameSecondsElapsed;
					usleep(TimeToSleep * 1000000);
				}
		}
			
			*PlatformData.LastInput = *PlatformData.NewInput;
			PlatformData.NewInput->dT = FrameSecondsElapsed;
			LastTime = TimeNow;
		}
	}
	return 0;
}
