#define WINDOWS

#include <stdio.h>
#include <windows.h>
#include <Windowsx.h>
#include <Dsound.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

#include <openvr.h>
#include "win32_platform.h"
#include "platform.h"
#include "glHelper.cpp"
#include "vrHelper.cpp"
#include "game.cpp"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Dxguid.lib")
#pragma comment(lib, "Dsound.lib")
#if OPEN_VR
#pragma comment(lib, "openvr_api.lib")
#endif

void ShowAlert(char* Message)
{
    MessageBoxA(0, Message, "Something Broke", 0);
}

void ShowErrorAlert(const char* Format, const char* Message)
{
    int errorMessageLength = strlen(Message) + strlen(Format);
    char* errorMessageBuffer = (char*)malloc(errorMessageLength);
    sprintf_s(errorMessageBuffer,
              errorMessageLength,
              Format,
              Message);
    ShowAlert(errorMessageBuffer);
    free(errorMessageBuffer);
}

LARGE_INTEGER GetCPUTime()
{
    LARGE_INTEGER Res;
    QueryPerformanceCounter(&Res);
    return Res;
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
    SetWindowText(Console, "Ship Game Console");
    ShowWindow(Console, 0);
    
    return Console;
}

char* ConsoleInputMutexName = "SGConsoleInputMutex";
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

win32_point GetCursorPosition()
{
    POINT CursorPoint;
    GetCursorPos(&CursorPoint);
    win32_point CursorPosition = {CursorPoint.x, CursorPoint.y};
    return CursorPosition;
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

win32_point GetWindowCenter(HWND Window)
{
    win32_point Result;
    RECT ClientRect;
    GetWindowRect(Window, &ClientRect);
    Result.X = (ClientRect.right - ClientRect.left)/2 + ClientRect.left;
    Result.Y = (ClientRect.bottom - ClientRect.top)/2 + ClientRect.top;
    return Result;
}

void CenterCursor(HWND Window)
{
    win32_point Center = GetWindowCenter(Window);
    SetCursorPos(Center.X, Center.Y);
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

bool
Win32InitDirectSound(HWND WindowHandle, LPDIRECTSOUNDBUFFER* SecondaryBuffer) {
    LPDIRECTSOUNDBUFFER PrimaryBuffer;
    LPDIRECTSOUND8 DirectSound;
    if (FAILED(DirectSoundCreate8(&DSDEVID_DefaultPlayback, &DirectSound, 0))) {
        printf("DirectSoundCreate8() failed!\n");
        return false;
    }
    
    if (FAILED(DirectSound->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY)))
    {
        printf("DirectSound->SetCooperativeLevel() failed!\n");
        return false;
    }
    
    int32 SamplesPerSecond = 48000;
    int32 BytesPerSample = sizeof(int16)*2;
    uint32 SecondaryBufferSize = SamplesPerSecond*BytesPerSample;
    
    DSBUFFERDESC BufferDescription = {};;
    BufferDescription.dwSize = sizeof(BufferDescription);
    BufferDescription.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_PRIMARYBUFFER;
    if (FAILED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
    {
        printf("DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0) failed!\n");
        return false;
    }
    
    WAVEFORMATEX WaveFormat = {};
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2;
    WaveFormat.nSamplesPerSec = SamplesPerSecond;
    WaveFormat.wBitsPerSample = BytesPerSample*4;
    WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
    WaveFormat.cbSize = 0;
    
    DSBUFFERDESC SecondaryBufferDescription = {};
    SecondaryBufferDescription.dwSize = sizeof(SecondaryBufferDescription);
    SecondaryBufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
    SecondaryBufferDescription.dwBufferBytes = SecondaryBufferSize;
    SecondaryBufferDescription.lpwfxFormat = &WaveFormat;
    if (FAILED(DirectSound->CreateSoundBuffer(&SecondaryBufferDescription, SecondaryBuffer, 0)))
    {
        printf("DirectSound->CreateSoundBuffer(Secondary) failed!\n");
        return 0;
    }
    DirectSound->Release();
    return true;
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
            CREATESTRUCT* Create = (CREATESTRUCT*)LParam;
            win32_state* State = (win32_state*)Create->lpCreateParams;
            SetWindowLongPtr(Window, GWLP_USERDATA, (LONG_PTR)State);
            State->Focused = false;
        } break;
        case WM_CLOSE:
        {
            win32_state* State = GetAppState(Window);
            State->Running = false;
#if OPEN_VR
            vr::VR_Shutdown();
            State->VRSystem = 0;
#endif
            PostQuitMessage(0);
        } break;
        case WM_MOUSELEAVE:
        {
            CenterCursor(Window);
        } break;        
        case WM_SETCURSOR:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
        case WM_DESTROY:
        {
            win32_state* State = GetAppState(Window);
            State->Running = false;
        } break;
        case WM_SETFOCUS:
        {
            win32_state* State = GetAppState(Window);
            win32_window_dimension WindowDimension = GetWindowDimension(Window);
            CenterCursor(Window);
            ShowCursor(false);
            State->Focused = true;
        } break;
        case WM_KILLFOCUS:
        {
            win32_state* State = GetAppState(Window);
            ShowCursor(true);
            State->Focused = false;
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
        case WM_SIZE:
        {
            win32_state* State = GetAppState(Window);
            int Width = LOWORD(LParam);
            int Height = HIWORD(LParam);
            State->WindowWidth = Width;
            State->WindowHeight = Height;
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

void VRProcessPendingMessages(vr::IVRSystem *VRSystem)
{
    vr::VREvent_t event;
    while(VRSystem->PollNextEvent(&event, sizeof(event)))
    {
        switch(event.eventType)
        {
            case vr::VREvent_None:
            {
                printf("Invalid event received.\n");
            } break;
            case vr::VREvent_TrackedDeviceActivated:
            {
                printf("Device %u attached.\n", event.trackedDeviceIndex);
            } break;
            case vr::VREvent_TrackedDeviceDeactivated:
            {
                printf("Device %u detached.\n", event.trackedDeviceIndex);
            } break;
            case vr::VREvent_TrackedDeviceUpdated:
            {
                printf("Device %u updated.\n", event.trackedDeviceIndex);
            } break;
            default:
            {
                printf("Something else happened.\n");
            } break;
        }
    }
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
            case WM_LBUTTONUP:
            {
                Mouse->Mouse1.Down = 0;
                ReleaseCapture();
            } break;
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
                    if (VKCode == 'C')
                    {
                        CenterCursor(Window);
                    }
                    else if(VKCode == 'W')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Forward, IsDown);
                    }
                    else if(VKCode == 'A')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Left, IsDown);
                    }
                    else if(VKCode == 'S')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Back, IsDown);
                    }
                    else if(VKCode == 'D')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Right, IsDown);
                    }
                    else if(VKCode == 'Q')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->UpperLeft, IsDown);
                    }
                    else if(VKCode == 'E')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->UpperRight, IsDown);
                    }
                    else if (VKCode == VK_SHIFT)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Up, IsDown);
                    }
                    else if (VKCode == VK_CONTROL)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Down, IsDown);
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
                        State->Running = false;
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
            }
            break;
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            break;
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
    ShowConsole(&State, 1);
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
    
    /*Init Window*/
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    WindowClass.hCursor = LoadCursor(0, NULL);
    WindowClass.lpszClassName="SGMainWindowClass";
    
    if (!RegisterClassA(&WindowClass))
    {
        printf("failed to register window class");
        return 1;
    }
    /*End Init Window*/
    
    /*Init OpenVR*/
#if OPEN_VR
    vr::IVRSettings *VRSettings;
    
    if (!vr::VR_IsRuntimeInstalled())
    {
        ShowAlert("No VR runtime detected");
    }
    else if (!vr::VR_IsHmdPresent())
    {
        ShowAlert("No headset detected");
    }
    else
    {
        vr::HmdError vrInitError = vr::VRInitError_None;
        State.VRSystem = vr::VR_Init(&vrInitError, vr::VRApplication_Scene);
        if (vrInitError != vr::VRInitError_None)
        {
            State.VRSystem = 0;
            const char* vrErrorMessage = vr::VR_GetVRInitErrorAsEnglishDescription(vrInitError);
            char* errorMessageFormat = "Unable to init VR runtime: %s";
            ShowErrorAlert(errorMessageFormat, vrErrorMessage);
        }
    }
    
    if (State.VRSystem)
    {
        vr::EVRInitError vrInitError;
        VRSettings = (vr::IVRSettings*)VR_GetGenericInterface(vr::IVRSettings_Version, &vrInitError);
        if (vrInitError != vr::VRInitError_None)
        {
            VRSettings = 0;
            const char* vrErrorMessage = vr::VR_GetVRInitErrorAsEnglishDescription(vrInitError);
            char* errorMessageFormat = "Unable to init VR settings system: %s";
            ShowErrorAlert(errorMessageFormat, vrErrorMessage);
            
            if (VRSettings)
            {
                vr::EVRSettingsError vrSettingsError;
                if (VRSettings->GetBool(vr::k_pch_SteamVR_Section,
                                        vr::k_pch_SteamVR_ForceFadeOnBadTracking_Bool,
                                        &vrSettingsError))
                {
                    
                    if (vrSettingsError != vr::VRSettingsError_None)
                    {
                        ShowErrorAlert("Unable to get ForceFade bool: %s",
                                       "Unknown");
                    }
                    
                    VRSettings->SetBool(vr::k_pch_SteamVR_Section,
                                        vr::k_pch_SteamVR_ForceFadeOnBadTracking_Bool,
                                        0,
                                        &vrSettingsError);
                    if (vrSettingsError != vr::VRSettingsError_None)
                    {
                        ShowErrorAlert("Unable to set ForceFade bool: %s",
                                       "Unknown");
                    }
                    VRSettings->Sync(true, &vrSettingsError);
                    if (vrSettingsError != vr::VRSettingsError_None)
                    {
                        ShowErrorAlert("Unable to sync settings: %s",
                                       "Unknown");
                    }
                }
            }
        }
        
        //List tracked devices
        for(uint32 i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
        {
            vr::TrackedDeviceClass deviceClass = State.VRSystem->GetTrackedDeviceClass(i);
            if (deviceClass != vr::TrackedDeviceClass_Invalid)
            {
                printf("Device %d: %s\n", i, TrackedDeviceClassToEnglishDescription(deviceClass));
            }
        }
    }
#endif
    
    /*End Init OpenVR*/
    
    /*Init OpenGL*/
    int WindowWidth = 800;
    int WindowHeight = 600;
    
    HWND ContextInitWindow = CreateWindowEx(
        0,
        WindowClass.lpszClassName,
        "Ship Game",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WindowWidth,
        WindowHeight,
        0,
        0,
        Instance,
        &State);
    
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER |
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    
    HDC TempDeviceContext = GetDC(ContextInitWindow);
    int ChosenPixelFormat = ChoosePixelFormat(TempDeviceContext, &pfd);
    
    bool setResult = SetPixelFormat(TempDeviceContext, ChosenPixelFormat, &pfd);
    if (!setResult)
    {
        ShowAlert("Setting pixel format failed");
    }
    HGLRC TempRenderContext = wglCreateContext(TempDeviceContext);
    wglMakeCurrent(TempDeviceContext, TempRenderContext);
    LoadWGLBullshit();
    
    HWND WindowHandle = CreateWindowEx(
        0,
        WindowClass.lpszClassName,
        "Ship Game",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WindowWidth,
        WindowHeight,
        0,
        0,
        Instance,
        &State);
    
    if(!WindowHandle)
    {
        DWORD LastError = GetLastError();
        printf("failed to get window handle: %d\n", LastError);
        return 1;
    }
    HDC DeviceContext = GetDC(WindowHandle);
    
    const int pixelAttribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };
    
    int pixelFormatID;
    UINT numFormats;
    bool pixelFormatResult = wglChoosePixelFormatARB(DeviceContext, pixelAttribs, 0, 1, &pixelFormatID, &numFormats);
    if (!pixelFormatResult || numFormats == 0)
    {
        ShowAlert("wglChoosePixelFormatARB() failed");
        return 1;
    }
    
    PIXELFORMATDESCRIPTOR PFD;
    DescribePixelFormat(DeviceContext, pixelFormatID, sizeof(PFD), &PFD);
    SetPixelFormat(DeviceContext, pixelFormatID, &PFD);
    
    int glContextAttributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };
    
    HGLRC RenderContext = wglCreateContextAttribsARB(DeviceContext, 0, glContextAttributes);
    
    wglMakeCurrent(0, 0);
    wglDeleteContext(TempRenderContext); 
    ReleaseDC(ContextInitWindow, TempDeviceContext);
    DestroyWindow(ContextInitWindow);
    
    if(!wglMakeCurrent(DeviceContext, RenderContext))
    {
        ShowAlert("wglMakeCurrent() 2 failed");
        return 1;
    }
    
    PrintGLVersion();
    //PrintAvailableGLExtensions();
    LoadGLExtensions();
    GLErrorShow();
    /*End Init OpenGL*/
    
    /*Init DirectSound*/
    LPDIRECTSOUNDBUFFER SecondaryDirectSoundBuffer;
    bool SoundIsValid = Win32InitDirectSound(WindowHandle, &SecondaryDirectSoundBuffer);
    /*End Init DirectSound*/
    
#if OPEN_VR
    uint32 VRWidth;
    uint32 VRHeight;
    FramebufferDesc LeftEyeBuffer = {0};
    FramebufferDesc RightEyeBuffer = {0};
    if (State.VRSystem)
    {
        if (!SetupStereoRenderTargets(State.VRSystem, &VRWidth, &VRHeight, &LeftEyeBuffer, &RightEyeBuffer))
        {
            ShowAlert("Failed to setup render targets!");
        }
        GLErrorShow();
        printf("VR Render target size: %d x %d\n", VRWidth, VRHeight);
        
        if (!vr::VRCompositor())
        {
            ShowAlert("Compositor initialization failed.");
            return 1;
        }
    }
#endif
    
    int HardRefreshHz = 120;
    
    float GameUpdateHz = (HardRefreshHz/2.0f);
    float TargetFrameSeconds = 1.0f/GameUpdateHz;
    
    platform_data PlatformData = {0};
    PlatformData.MainMemorySize = MEGABYTES(32);
    PlatformData.TempMemorySize = MEGABYTES(32);
    PlatformData.TotalMemorySize = PlatformData.MainMemorySize + PlatformData.TempMemorySize;
    PlatformData.MainMemory = VirtualAlloc(0,
                                           (size_t)PlatformData.TotalMemorySize,
                                           MEM_RESERVE|MEM_COMMIT,
                                           PAGE_READWRITE);
    PlatformData.TempMemory = (uint8 *)PlatformData.MainMemory+PlatformData.MainMemorySize;
    
    input Inputs[2];
    Inputs[0] = {0};
    Inputs[1] = {0};
    input *NewInput = &Inputs[0];
    input *LastInput = &Inputs[1];
    PlatformData.LastInput = LastInput;
    PlatformData.NewInput = NewInput;
    
#if OPEN_VR
    PlatformData.VRBufferWidth = VRWidth;
    PlatformData.VRBufferHeight = VRHeight;
    PlatformData.LeftEye = &LeftEyeBuffer;
    PlatformData.RightEye = &RightEyeBuffer;
#endif
    NewInput->dT = 0.0f;
    State.Running = true;
    
    LARGE_INTEGER LastCounter = GetCPUTime();
    
    int FPSSampleFrames = 60;
    int FPSFrameCounter = 0;
    float FPSAverageAccumulator = 0.0f;
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
        if (State.VRSystem)
        {
            VRProcessPendingMessages(State.VRSystem);
            
        }
        
        NewKeyboard->RightStick.X = 0;
        NewKeyboard->RightStick.Y = 0;
        if (State.Focused)
        {
            win32_point WindowCenter = GetWindowCenter(WindowHandle);
            win32_point MousePosition = GetCursorPosition();
            win32_point dMousePosition = {MousePosition.X - WindowCenter.X,
                MousePosition.Y - WindowCenter.Y};
            NewKeyboard->RightStick.X = dMousePosition.X;
            NewKeyboard->RightStick.Y = dMousePosition.Y;
            CenterCursor(WindowHandle);
        }
        
        
        game_screen_buffer Buffer = {};
        Buffer.Memory = State.Backbuffer.Memory;
        Buffer.Width = State.Backbuffer.Width;
        Buffer.Height = State.Backbuffer.Height;
        Buffer.Pitch = State.Backbuffer.Pitch;
        Buffer.BytesPerPixel = State.Backbuffer.BytesPerPixel;
        
        PlatformData.WindowWidth = State.WindowWidth;
        PlatformData.WindowHeight = State.WindowHeight;
        
        UpdateAndRender(&PlatformData);
#if OPEN_VR
        if (State.VRSystem)
        {
            vr::Texture_t LeftEyeTexture = {(void*)(uintptr_t)PlatformData.LeftEye->ResolveTextureId,
                vr::TextureType_OpenGL,
                vr::ColorSpace_Gamma };
            vr::VRCompositor()->Submit(vr::Eye_Left, &LeftEyeTexture);
            vr::Texture_t RightEyeTexture = {(void*)(uintptr_t)PlatformData.RightEye->ResolveTextureId,
                vr::TextureType_OpenGL,
                vr::ColorSpace_Gamma };
            vr::VRCompositor()->Submit(vr::Eye_Right, &RightEyeTexture);
        }
#endif
        glFinish();
        SwapBuffers(DeviceContext);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glFlush();
        glFinish();
#if OPEN_VR
        if (State.VRSystem)
        {
            vr::TrackedDevicePose_t TrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
            vr::VRCompositor()->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, 0, 0);
        }
#endif
        //post work
        float FrameSecondsElapsed = 0;
        LARGE_INTEGER TimeNow = GetCPUTime();
        while(FrameSecondsElapsed < TargetFrameSeconds)
        {
            TimeNow = GetCPUTime();
            FrameSecondsElapsed = Win32GetSecondsElapsed(LastCounter, TimeNow);
            if (FrameSecondsElapsed < TargetFrameSeconds)
            {
                DWORD SleepMS = (DWORD)(1000.0f*(TargetFrameSeconds - FrameSecondsElapsed));
                if (SleepMS > 0)
                {
                    Sleep(SleepMS);
                    TimeNow = GetCPUTime();
                }
            }
        }
        
        *PlatformData.LastInput = *PlatformData.NewInput;
        PlatformData.NewInput->dT = FrameSecondsElapsed;
        LastCounter = TimeNow;
    }
    
    return EXIT_SUCCESS;
}
