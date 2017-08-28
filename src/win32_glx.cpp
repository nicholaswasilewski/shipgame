#define WINDOWS
#include <stdio.h>
#include <windows.h>
#include <Windowsx.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

#include "glHelper.cpp"
#include "game.cpp"
#include "platform.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "OpenGL32.lib")

struct stack
{
    int Count;
    void* Base;
};

typedef struct memory_arena
{
    size_t Size;
    uint8 *Base;
    size_t Used;
} memory_arena;

typedef struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
} win32_offscreen_buffer;

typedef struct win32_state
{
    bool Running;
    bool ConsoleVisible;
    win32_offscreen_buffer Backbuffer;
    HWND MainWindow;
    WINDOWPLACEMENT PreviousWindowPlacement;
    HWND Console;

    uint64 TotalSize;
    void* GameMemoryBlock;
} win32_state;

typedef struct win32_window_dimension {
    int Width;
    int Height;
} win32_window_dimension;

#define PushSize(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
void*
PushSize_(memory_arena *Arena, size_t Size)
{
    Assert((Arena->Used + Size) <= (Arena->Size));
    void *NewSpace = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return NewSpace;
}

inline void
InitArena(memory_arena *Arena, size_t Size, uint8 *Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

inline memory_arena
PushArena(memory_arena *Arena, size_t Size)
{
    memory_arena Result = {0};
    Result.Size = Size;
    Result.Used = 0;
    Result.Base = (uint8 *)PushSize_(Arena, Size);
    return Result;
}

size_t ArenaSizeRemaining(memory_arena *Arena)
{
    return Arena->Size - Arena->Used;
}

HWND FindConsole()
{
    return FindWindowA("ConsoleWindowClass", 0);
}

HWND CreateConsole()
{
    HWND Console;
    long StandardOut;
    FILE *fp;

    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    Console = FindConsole();
    SetWindowText(Console, "GLX Console");
    ShowWindow(Console, 0);
    
    return Console;
}

char* ConsoleInputMutexName = "GLXConsoleInputMutex";
bool NewInput;
#define CONSOLE_INPUT_MAX 512
char ConsoleInput[CONSOLE_INPUT_MAX];

bool CheckConsoleInput(LPVOID OutBuffer) {
    if (NewInput)
    {
        HANDLE mut = OpenMutex(MUTEX_ALL_ACCESS, 0, ConsoleInputMutexName);
        memcpy(OutBuffer, ConsoleInput, CONSOLE_INPUT_MAX);
        memset(ConsoleInput, 0, CONSOLE_INPUT_MAX);
        NewInput = false;
        ReleaseMutex(mut);
        return true;
    }
    else
    {
        return false;
    }
}

DWORD WINAPI Win32AsyncReadFromConsole(void* ThreadInput) {

    HANDLE hConHandle = CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    DWORD CharsToRead = CONSOLE_INPUT_MAX;
    DWORD CharsRead;
    while(true) {
        char ConsoleInputTempBuffer[CONSOLE_INPUT_MAX];
        ReadConsole(hConHandle, &ConsoleInputTempBuffer, CharsToRead, &CharsRead, 0);
        HANDLE mut = OpenMutex(MUTEX_ALL_ACCESS, 0, ConsoleInputMutexName);

        //remove endline because that's how I signal I'm done entering things? let's check it out
        ConsoleInputTempBuffer[CharsRead-2] = '\0';
        memcpy(ConsoleInput, ConsoleInputTempBuffer, CharsRead);
        NewInput = true;
        ReleaseMutex(mut);
    }
}

void ShowConsole(win32_state* State, bool Show)
{    
    State->ConsoleVisible = Show;
    ShowWindow(State->Console, Show);

    SetActiveWindow(State->MainWindow);
}

void ToggleConsole(win32_state* State)
{
    ShowConsole(State, !State->ConsoleVisible);
}

void FocusConsole(win32_state* State)
{
    SetActiveWindow(State->Console);
    SetFocus(State->Console);
}

win32_window_dimension GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return Result;
}

int64 PerfCountFrequency;
float Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    float Result = ((float)(End.QuadPart - Start.QuadPart) / (float)PerfCountFrequency);
    return Result;
}

void Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
				HDC DeviceContext,
				int WindowWidth,
				int WindowHeight)
{
    float HorizontalStretch = (float)WindowWidth / (float)Buffer->Width;
    float VerticalStretch = (float)WindowHeight / (float)Buffer->Height;
    float StretchRatio = Min(HorizontalStretch, VerticalStretch);

    int DestinationWidth = Buffer->Width*StretchRatio;
    int DestinationHeight = Buffer->Height*StretchRatio;

    int SideGutter = (WindowWidth - DestinationWidth+1)/2;
    int VertGutter = (WindowHeight - DestinationHeight+1)/2;

    PatBlt(DeviceContext, 0, 0, SideGutter, WindowHeight, BLACKNESS);
    PatBlt(DeviceContext, WindowWidth-SideGutter, 0, SideGutter, WindowHeight, BLACKNESS);
    PatBlt(DeviceContext, 0, 0, WindowWidth, VertGutter, BLACKNESS);
    PatBlt(DeviceContext, 0, WindowHeight-VertGutter, WindowWidth, VertGutter, BLACKNESS);
    
    StretchDIBits(
        DeviceContext,
        (WindowWidth/2)-(DestinationWidth/2),
        (WindowHeight/2)-(DestinationHeight/2),
        DestinationWidth,
        DestinationHeight,
        0,0,Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

inline win32_state* GetAppState(HWND Window)
{
    LONG_PTR Pointer = GetWindowLongPtr(Window, GWLP_USERDATA);
    return (win32_state*)Pointer;
}


void
Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int ScreenWidth, int ScreenHeight)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 8, MEM_RELEASE);
    }
    Buffer->Width = ScreenWidth;
    Buffer->Height = ScreenHeight;
	
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    Buffer->BytesPerPixel = 4;

    int BitmapMemorySize = (ScreenWidth*ScreenHeight)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
}

LRESULT CALLBACK MainWindowCallback(HWND Window,
                                    UINT Message,
                                    WPARAM WParam,
                                    LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_CREATE:
        {
	    PIXELFORMATDESCRIPTOR pfd =
		{
		    sizeof(PIXELFORMATDESCRIPTOR),
		    1,
		    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		    PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
		    32,                        //Colordepth of the framebuffer.
		    0, 0, 0, 0, 0, 0,
		    0,
		    0,
		    0,
		    0, 0, 0, 0,
		    24,                        //Number of bits for the depthbuffer
		    8,                        //Number of bits for the stencilbuffer
		    0,                        //Number of Aux buffers in the framebuffer.
		    PFD_MAIN_PLANE,
		    0,
		    0, 0, 0
		};

            CREATESTRUCT* Create = (CREATESTRUCT*)LParam;
            win32_state* State = (win32_state*)Create->lpCreateParams;
            SetWindowLongPtr(Window, GWLP_USERDATA, (LONG_PTR)State);

	    HDC DeviceContext = GetDC(Window);
	    int ChosenPixelFormat = ChoosePixelFormat(DeviceContext, &pfd);
	    SetPixelFormat(DeviceContext, ChosenPixelFormat, &pfd);
	    HGLRC RenderContext = wglCreateContext(DeviceContext);
	    wglMakeCurrent(DeviceContext, RenderContext);
        } break;
        case WM_CLOSE:
        {
            win32_state* State = GetAppState(Window);
            State->Running = false;
            PostQuitMessage(0);
        } break;	
        case WM_SETCURSOR:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
            /*SetCursor(0)*/
            
        } break;
        case WM_DESTROY:
        {
            win32_state* AppState = GetAppState(Window);
            AppState->Running = false;
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            //Assert(true);
        } break;
        case WM_ACTIVATEAPP:
        {
            if (WParam == TRUE)
            {
                SetLayeredWindowAttributes(Window, RGB(0,0,0), 255, LWA_ALPHA);
            }
            else
            {
                SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 128, LWA_ALPHA);
            }
        } break;
	
        case WM_PAINT:
        {
            win32_state* State = GetAppState(Window);
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32DisplayBufferInWindow(&State->Backbuffer, DeviceContext, Dimension.Width, Dimension.Height);
            EndPaint(Window, &Paint);
        } break;
	
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
            break;
        } break;
    }
    return Result;
}

void
Win32ProcessKeyboardMessage(button_state *ButtonState, bool DownState)
{
    ButtonState->Down = DownState;
}

void
Win32ProcessPendingMessages(HWND Window,
                            win32_state *State,
                            mouse *Mouse,
                            controller *KeyboardController)
{			
	MSG Message;
	while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		switch(Message.message)
		{
            case WM_QUIT:
            {
                State->Running = false;
            } break;
            case WM_MOUSEMOVE:
            {
                win32_window_dimension Dimension = GetWindowDimension(Window);
                float XRatio = (float)(State->Backbuffer.Width) / Dimension.Width;
                float YRatio = (float)(State->Backbuffer.Height) / Dimension.Height;
                
                Mouse->X = Clamp(0, roundf(GET_X_LPARAM(Message.lParam)*XRatio), State->Backbuffer.Width);
                Mouse->Y = Clamp(0, roundf(GET_Y_LPARAM(Message.lParam)*YRatio), State->Backbuffer.Height);
            } break;
            case WM_LBUTTONDOWN:
            {
                Mouse->Mouse1.Down = 1;
                SetCapture(Window);
            } break;
            /*
            case WM_RBUTTONDOWN:
            {
                Mouse->Mouse2.Down = 1;
                SetCapture(Window);
            } break;
            case WM_MBUTTONDOWN:
            {
                Mouse->Mouse3.Down = 1;
                SetCapture(Window);
            } break;
            */
            
            case WM_LBUTTONUP:
            {
                Mouse->Mouse1.Down = 0;
                ReleaseCapture();
            } break;
            /*
            case WM_RBUTTONUP:
            {
                Mouse->Mouse2.Down = 0;
                ReleaseCapture();
            } break;
            case WM_MBUTTONUP:
            {
                Mouse->Mouse3.Down = 0;
                ReleaseCapture();
            } break;
            */
                    
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				uint32 VKCode = (uint32)Message.wParam;
				bool WasDown = ((Message.lParam & (1 << 30)) != 0);
				bool IsDown = ((Message.lParam & (1 << 31)) == 0);

				if(WasDown != IsDown)
				{
					if(VKCode == 'W')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Up, IsDown);
					}
					else if(VKCode == 'A')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Left, IsDown);
					}
					else if(VKCode == 'S')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Down, IsDown);
					}
					else if(VKCode == 'D')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Right, IsDown);
					}
					else if(VKCode == 'Q')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->LeftShoulder, IsDown);
					}
					else if(VKCode == 'E')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->RightShoulder, IsDown);
					}
					else if(VKCode == VK_UP)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionUp, IsDown);
					}
					else if(VKCode == VK_LEFT)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionLeft, IsDown);
					}
					else if(VKCode == VK_DOWN)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionDown, IsDown);
					}
					else if(VKCode == VK_RIGHT)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionRight, IsDown);
					}
					else if(VKCode == VK_ESCAPE)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Back, IsDown);
					}
					else if(VKCode == VK_SPACE)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Start, IsDown);
					}
					if (IsDown)
					{
						bool AltIsDown = (Message.lParam & (1 << 29));
						if(VKCode == VK_F4 && AltIsDown)
						{
                            State->Running = false;
						}
                        if (VKCode == VK_F12)
                        {
                            ToggleConsole(State);
                        }

                        if (VKCode == VK_RETURN && AltIsDown)
                        {
                            //ToggleFullScreen(State);
                        }
					}
				}
			} break;
			default:
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			} break;
		}
	}
}				

int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{
    win32_state State = {};
    State.Console = CreateConsole();
    CreateMutex(0, 0, ConsoleInputMutexName);

    DWORD ConsoleReadThreadID;
    CreateThread(NULL,
                 0,
                 Win32AsyncReadFromConsole,
                 0,
                 0,
                 &ConsoleReadThreadID);

    LARGE_INTEGER PerfCountFreqRes;
    QueryPerformanceFrequency(&PerfCountFreqRes);
    PerfCountFrequency = PerfCountFreqRes.QuadPart;

    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.lpszClassName="GLXMainWindowClass";

    int GameWidth = 480;
    int GameHeight = 320;
    Win32ResizeDIBSection(&State.Backbuffer, GameWidth, GameHeight);

    if (!RegisterClassA(&WindowClass))
    {
	printf("failed to register window class");
	return 1;
    }

    int WindowWidth = 976;
    int WindowHeight = 678;

    HWND WindowHandle = CreateWindowEx(
	0,
	WindowClass.lpszClassName,
	"GLX",
	WS_OVERLAPPEDWINDOW|WS_VISIBLE,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	WindowWidth,
	WindowHeight,
	0,
	0,
	Instance,
	&State);
    PrintGLVersion();
    PrintAvailableGLExtensions();
    LoadGLExtensions();

    if(!WindowHandle)
    {
	DWORD LastError = GetLastError();
	printf("failed to get window handle: %d\n", LastError);
	return 1;
    }

    input Inputs[2];
    Inputs[0] = {0};
    Inputs[1] = {0};
    input *NewInput = &Inputs[0];
    input *LastInput = &Inputs[1];
    State.Running = true;
    while(State.Running)
    {
        
        //prework
        mouse *OldMouse = &LastInput->Mouse;
        mouse *NewMouse = &NewInput->Mouse;

        NewMouse->Moved = 0;
        NewMouse->X = OldMouse->X;
        NewMouse->Y = OldMouse->Y;
        for(int ButtonIndex = 0;
            ButtonIndex < ArrayCount(NewMouse->Buttons);
            ++ButtonIndex)
        {
            NewMouse->Buttons[ButtonIndex].Down =
                OldMouse->Buttons[ButtonIndex].Down;
        }
        
        controller *OldKeyboard = &LastInput->Keyboard;
        controller *NewKeyboard = &NewInput->Keyboard;
        
        for(int ButtonIndex = 0;
            ButtonIndex < ArrayCount(NewKeyboard->Buttons);
            ++ButtonIndex)
        {
            NewKeyboard->Buttons[ButtonIndex].Down =
                OldKeyboard->Buttons[ButtonIndex].Down;
        }
        Win32ProcessPendingMessages(WindowHandle, &State, NewMouse, NewKeyboard);
    }

    return 0;
}
