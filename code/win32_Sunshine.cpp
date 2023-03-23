#include <windows.h>
//#include <dsound.h>

HWND MainWindowHandle;
WNDCLASSEX MainWindowClass;
char MainWindowTitle[256] = "Sunshine, an effigy.";
int MainWindowWidth = 1600;
int MainWindowHeight = 900;

HWND win_createwindow(HINSTANCE hInstance);

// no CRT stuff
#if NOCRT
#include <nocrt/crt_float.cpp>
#include <nocrt/crt_math.cpp>
#include <nocrt/crt_memory.cpp>
#include <nocrt/crt_fix.cpp>
#endif

#include "win32_helpers.cpp"

#include "platform.h"
#include "input.cpp"
#include "gfx/render_ogl.cpp"

union offBACKBUFFER_buffer{
    struct {
        void* memory;
        int height;
        int width;
        int pitch;
        int bpp;
    };
    struct {
        void* Memory;
        int Height;
        int Width;
        int Pitch;
        int BytesPerPixel;
    };
};

struct win_offBACKBUFFER_buffer
{
	// windows pixels: 32-bit, upside-down, BB GG RR XX
	BITMAPINFO Info;
	offBACKBUFFER_buffer buffer;
};

struct win_window_dimension
{
	int Width;
    int Height;
};

offBACKBUFFER_buffer GlobalBackBuffer;
BITMAPINFO BackBufferInfo;
char GameCodeName[MAX_PATH];
char TempGameCodeName[MAX_PATH];

bool WindowFocused = true;
int GameRunning = false;

LARGE_INTEGER LastFrameTime;
long long CurrentFrameTime;

void* EndOfGlobalBackBuffer;


void win_updateInput()
{
	if(GetKeyState(VK_ESCAPE) & 0x8000
       && WindowFocused )
	{
		GameRunning = false;
		return;
	}
    
	// ALL buttons downprev
	for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
	{
		inputstate.button[ii].downprevious = inputstate.button[ii].down;
	}
    for(int ii = 0; ii < MAX_CONTROLLERS; ++ii)
    {
        for(int jj = 0; jj < CONTROLLER_BUTTONS; ++jj)
        {
            inputstate.controllers[ii].button[jj].down = false;
        }
    }
    
    // keyboard to virtualcontroller
	if(WindowFocused || INACTIVE_KEYBOARD_INPUT)
	{
		for(uint32 ii = 0; ii < DIRECTION_BUTTONS; ++ii)
		{
			int hits = 0;
			for(uint32 jj = 0; jj < directionmap[ii].totalbinds; ++jj)
			{
				if(GetKeyState(*(directionmap[ii].data + jj)) & 0x8000)
					hits++;
				inputstate.directions[ii].down = !!(GetKeyState(*(directionmap[ii].data + jj)) & 0x8000);
			}
			inputstate.directions[ii].down = !!hits;
		}
        
		for(uint32 ii = 0; ii < BUTTONS; ++ii)
		{
			int hits = 0;
			for(uint32 jj = 0; jj < buttonmap[ii].totalbinds; ++jj)
			{
				if(GetKeyState(*(buttonmap[ii].data + jj)) & 0x8000)
					hits++;
				inputstate.buttons[ii].down = !!(GetKeyState(*(buttonmap[ii].data + jj)) & 0x8000);
			}
			inputstate.buttons[ii].down += !!hits;
		}
        
	} else {
		for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
		{
			inputstate.button[ii].down = false;
		}
	}
    
    // xinput handling
    if(WindowFocused || INACTIVE_CONTROLLER_INPUT)
    {
        updateXInput();
    }
    
	// mouse
	inputstate.mousexprevious = inputstate.mousex;
	inputstate.mouseyprevious = inputstate.mousey;
    
	POINT mousepos;
	GetCursorPos(&mousepos);
	ScreenToClient(MainWindowHandle,&mousepos);
	inputstate.mousex = mousepos.x;
	inputstate.mousey = mousepos.y;
    
	// L, R, M, X1, X2
	// getting l and r through windows messaging for now
	if(WindowFocused) // || INACTIVE_INPUT)
	{
		inputstate.mousebutton[0].down = !!(GetKeyState(VK_LBUTTON) & 0x80);
		inputstate.mousebutton[1].down = !!(GetKeyState(VK_RBUTTON) & 0x80);
		inputstate.mousebutton[2].down = !!(GetKeyState(VK_MBUTTON) & 0x80);
		inputstate.mousebutton[3].down = !!(GetKeyState(VK_XBUTTON1) & 0x80);
		inputstate.mousebutton[4].down = !!(GetKeyState(VK_XBUTTON2) & 0x80);
	}
    
	for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
	{
		inputstate.button[ii].wentdown = (inputstate.button[ii].down > 0
                                          && inputstate.button[ii].downprevious == 0);
		inputstate.button[ii].wentup = (inputstate.button[ii].down == 0
                                        && inputstate.button[ii].downprevious > 0);
	}
}

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Return = 0;
    
	switch(uMsg)
	{
		case WM_QUIT:
		case WM_DESTROY:
		case WM_CLOSE:
        {
            GameRunning = false;
        } break;
        
        case WM_ACTIVATE:
        {
        	WindowFocused = !!wParam;
        }
        
		//WM_NCRBUTTONDOWN nHittest:HTCAPTION
		case WM_NCLBUTTONDBLCLK:	// doubleclick maximize
        case WM_NCRBUTTONDOWN:		// right click freezing
        case WM_CONTEXTMENU:		// right click menu
        {
        	// disable this stuff
        } break;
        
        case WM_SYSCOMMAND:
        case WM_COMMAND:
        {
        	WPARAM sysParam = wParam & 0xFFF0;
        	switch(sysParam)
        	{
        		case SC_MOUSEMENU:
        		case SC_KEYMENU:
        		{
        			//if(lParam)
        		} break;
        		default:
                Return = DefWindowProc(hwnd, uMsg, wParam, lParam);
                break;
        	}
            
        } break;
		default:
        Return = DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
	}
    
	return(Return);
}

win_window_dimension win_getWindowDimension(HWND Window)
{
    win_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return(Result);
}

// TODO: vsync is totally broken
void win_wait()
{
	LARGE_INTEGER currentframe;
	long long sleeptime;
    
	QueryPerformanceCounter(&currentframe);
    
	sleeptime = CurrentFrameTime - (currentframe.QuadPart - LastFrameTime.QuadPart) / 100000;
	if(sleeptime>0)
	{
		sleeptime = abs(sleeptime-1);
		if(sleeptime > 15)
		{
			sleeptime = 15;
		}
		Sleep((DWORD)sleeptime);
	}
    
	QueryPerformanceCounter(&LastFrameTime);
}

void
win_resizeDIBSection( offBACKBUFFER_buffer *Buffer, int Width, int Height )
{
	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;
	Buffer->Pitch = GlobalBackBuffer.BytesPerPixel * GlobalBackBuffer.Width;
}

void
win_clearBuffer(offBACKBUFFER_buffer *Buffer)
{
	unsigned int* pixels = (unsigned int *)Buffer->Memory;
    
	unsigned int pixelssize = Buffer->Width * Buffer->Height;
    
	for(unsigned int ii = 0; ii < pixelssize; ++ii)
	{
		pixels[ii] = 0x00000000;
	}
}

void
win_swapBuffer(offBACKBUFFER_buffer *Buffer, HDC DeviceContext)
{
	StretchDIBits( DeviceContext,
                  0, 0, Buffer->Width, Buffer->Height,
                  0, 0, Buffer->Width, Buffer->Height,
                  GlobalBackBuffer.Memory, &BackBufferInfo,
                  DIB_RGB_COLORS, SRCCOPY );
}

void
win_processMessages()
{
	MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GameRunning = false;
            } break;
            
            case WM_INPUT:
            {
                updateRawInput(&Message); // needed this fast?
                UINT dwSize;
                
                GetRawInputData((HRAWINPUT)Message.lParam, RID_INPUT, NULL, &dwSize,
                                sizeof(RAWINPUTHEADER));
                BYTE rawdata[256] = {};
                if (dwSize == NULL || dwSize > 256)
                {
                    // this looks bad??
                    return;
                }
                
                if (GetRawInputData((HRAWINPUT)Message.lParam, RID_INPUT, rawdata, &dwSize,
                                    sizeof(RAWINPUTHEADER)) != dwSize )
                {
                    OutputDebugString (TEXT("GetRawInputData does not return correct size !\n"));
                    return; // still looks bad
                }
                
                debug_ps4_controller* ps4data = (debug_ps4_controller*)&rawdata[31];
                
                
                RAWINPUTHEADER header = {0};
                UINT hSize = sizeof(RAWINPUTHEADER);
                GetRawInputData((HRAWINPUT)Message.lParam, RID_HEADER, &header, &hSize,
                                sizeof(RAWINPUTHEADER));
                
                uint8 handleid = 0;
                
                
                
                
                if(ConnectedControllers[1].type == 0)
                {
                    handleid=1;
                }
                
                // match to controller
                if(header.hDevice == ConnectedControllers[handleid].handle)
                {
                    
                    inputstate.controllers[0].a.down = !!(rawdata[0x25] & 0x10);
                    inputstate.controllers[0].a.downprevious = inputstate.controllers[0].a.down;
                    
                    for(int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
                    {
                        inputstate.controllers[0].button[ii].wentdown = (inputstate.controllers[0].button[ii].down > 0
                                                                         && inputstate.button[ii].downprevious == 0);
                        inputstate.controllers[0].button[ii].wentup = (inputstate.button[ii].down == 0
                                                                       && inputstate.controllers[0].button[ii].downprevious > 0);
                    }
                }
                // do the input
                
                dwSize = 255;
                
            } break;
            
            case WM_SYSCOMMAND:
            case WM_COMMAND:
            {
                WPARAM sysParam = Message.wParam & 0xFFF0;
                switch(sysParam)
                {
	        		case SC_KEYMENU:
	        		{
                        //if(lParam)
	        		} break;
	        		case SC_MOUSEMENU:
	        		{
                        //
	        		} break;
	        		default:
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                    break;
                }
                
            } break;
            
            case WM_PAINT:
            {
                //ClearWindow;
                
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(MainWindowHandle, &Paint);
                win_window_dimension Dimension = win_getWindowDimension(MainWindowHandle);
                win_swapBuffer( &GlobalBackBuffer, DeviceContext );
                EndPaint(MainWindowHandle, &Paint);
            } break;
            case WM_MOUSEWHEEL:
            {
                if(WindowFocused)
                {
            		inputstate.mousewheel = GET_WHEEL_DELTA_WPARAM(Message.wParam);
                }
            }
#if 0
            case WM_LBUTTONDOWN:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[0].down = true;
                inputstate.mousebutton[0].wentdown = true;
                //inputstate.mousebutton[0].wentup = false;
            } break;
            
            case WM_LBUTTONUP:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[0].down = false;
                inputstate.mousebutton[0].wentup = true;
                //inputstate.mousebutton[0].wentdown = false;
            } break;
            
            case WM_RBUTTONDOWN:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[1].down = true;
                inputstate.mousebutton[1].wentdown = true;
                //inputstate.mousebutton[0].wentup = false;
            } break;
            
            case WM_RBUTTONUP:
            {
                //inputstate.mousebutton[0].downprevious = false;
                inputstate.mousebutton[1].down = false;
                inputstate.mousebutton[1].wentup = true;
                //inputstate.mousebutton[0].wentdown = false;
            } break;
            
#endif
            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
}

HWND
win_createwindow(HINSTANCE hInstance)
{
    HWND result = {0};
    win_resizeDIBSection(&GlobalBackBuffer,MainWindowWidth,MainWindowHeight);
    
    RECT lpRect = {100 , 100, (LONG)GlobalBackBuffer.Width + 100,
        (LONG)GlobalBackBuffer.Height + 100};
    DWORD dwStyle = (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX))
        | WS_VISIBLE;
    DWORD dwExStyle = 0;
    AdjustWindowRectEx( &lpRect, dwStyle, 0, dwExStyle);
    
    result = CreateWindowEx(dwExStyle,
                            MainWindowClass.lpszClassName,
                            MainWindowTitle,
                            dwStyle,
                            -1650, 50,
                            lpRect.right - lpRect.left,
                            lpRect.bottom - lpRect.top,
                            0, 0, hInstance, 0);
    return(result);
}

int
win_init(HINSTANCE hInstance)
{
    // TODO: Move window class init to a function, also double check that it's needed at all
    // register window class
    MainWindowClass.style 			= CS_HREDRAW | CS_VREDRAW;
    MainWindowClass.lpfnWndProc 	= WindowProc;
    MainWindowClass.hInstance 		= hInstance;
    MainWindowClass.hCursor 		= LoadCursor(0, IDC_ARROW);
    MainWindowClass.hbrBackground 	= (HBRUSH)GetStockObject(BLACK_BRUSH);
    MainWindowClass.lpszClassName 	= "RandomWindowClass";
    
    // making sure everything is correct for ex
    MainWindowClass.lpszMenuName 	= NULL;
    MainWindowClass.hIcon 			= LoadIcon(NULL, IDI_APPLICATION);
    MainWindowClass.cbSize 		= sizeof(WNDCLASSEX);
    MainWindowClass.hIconSm 		= LoadIcon(NULL, IDI_APPLICATION);
    
    if(!RegisterClassEx(&MainWindowClass))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    
    //refresh rate stuff
    HDC hdc = GetDC(NULL);
    long long WinRefreshRate = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(NULL, hdc);
    CurrentFrameTime = 1000/WinRefreshRate;
    
    timeBeginPeriod(1);
    QueryPerformanceCounter(&LastFrameTime);
    
    // TODO: DSound support
    //win_initDSound(gWindowHandle);
    
    // TODO: clean up game code name handling assumptions (win32_gamenam.exe + gamename.dll)
    char directory[MAX_PATH] = {0};
    char gamename[MAX_PATH] = {0};
    
    GetModuleFileName( NULL, directory, MAX_PATH );
    size_t stringlength = strlen(directory);
    char* underscorelocation = strrchr(directory,'_');
    char* periodlocation = strrchr(directory,'.');
    char* slashlocation = strrchr(directory,'\\');
    if(underscorelocation != 0)
    {
        strncpy(gamename,underscorelocation+1,periodlocation-underscorelocation-1);
    } else {
        strncpy(gamename,slashlocation+1,periodlocation-slashlocation-1);
    }
    *(slashlocation + 1) = '\0'; // leaving garbage after, gets cleaned on copy
    
    strcpy(TempGameCodeName, directory);
    strcat(TempGameCodeName, gamename);
    strcat(TempGameCodeName, ".dll");
    
    strcpy(GameCodeName, directory);
    strcat(GameCodeName, ".");
    strcat(GameCodeName, gamename);
    strcat(GameCodeName, "_");
    DWORD pid;
    GetWindowThreadProcessId(MainWindowHandle, &pid);
    char pids[8];
#ifndef NOCRT
    itoa((int)pid,pids,10);
#endif
    strcat(GameCodeName, pids);
    strcat(GameCodeName, ".dll");
    
    //default keybinds
    win_bindKey(&directionmap[0], VK_UP);
    win_bindKey(&directionmap[1], VK_DOWN);
    win_bindKey(&directionmap[2], VK_LEFT);
    win_bindKey(&directionmap[3], VK_RIGHT);
    
    win_bindKey(&directionmap[0], 0x57); // w
    win_bindKey(&directionmap[1], 0x53); // s
    win_bindKey(&directionmap[2], 0x41); // a
    win_bindKey(&directionmap[3], 0x44); // d
    
    win_bindKey(&buttonmap[0], ' ');
    win_bindKey(&buttonmap[1], '1');
    win_bindKey(&buttonmap[2], '2');
    win_bindKey(&buttonmap[3], '3');
    win_bindKey(&buttonmap[4], '4');
    win_bindKey(&buttonmap[5], '5');
    win_bindKey(&buttonmap[6], '6');
    win_bindKey(&buttonmap[7], '7');
    win_bindKey(&buttonmap[8], '8');
    win_bindKey(&buttonmap[9], '9');
    win_bindKey(&buttonmap[10], '0');
    
    // xinput
    win_initXInput();
    
    win_initRawInput();
    
    
    return 0;
}

void WindowsKill()
{
    timeEndPeriod(1);
}


struct win32_gamecode
{
    HMODULE gamecode;
    FILETIME writetime;
    
    game_update_and_render *UpdateAndRender;
};

unsigned int
scaleToWindow(unsigned int height, unsigned int border)
{
    unsigned int Result = (MainWindowHeight - 2 * border) / height;
    
    return(Result);
}

bool
LoadGameCode(win32_gamecode* GameCode)
{
    WIN32_FILE_ATTRIBUTE_DATA FileTimeData;
    // TODO: set a game code location variable?
    GetFileAttributesEx(TempGameCodeName, GetFileExInfoStandard, &FileTimeData);
    FILETIME newtime = FileTimeData.ftLastWriteTime;
    
    if( CompareFileTime(&newtime, &GameCode->writetime) != 0 )
    {
        // unload
        FreeLibrary(GameCode->gamecode);
        GameCode->gamecode = 0;
        
        // load
        CopyFile(TempGameCodeName,GameCodeName,FALSE);
        GameCode->gamecode = LoadLibraryA(GameCodeName);
        GameCode->UpdateAndRender = (game_update_and_render *)GetProcAddress(GameCode->gamecode, "GameUpdateAndRender");
        GameCode->writetime = newtime;
        
        return true;
    }
    return false;
}

int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    GameRunning = true;
    //MainWindowWidth = BACKBUFFER_WIDTH;
    //MainWindowHeight = BACKBUFFER_HEIGHT;
    
    // Start up Windows Platform
    if(win_init(hInstance) != 0)
    {
        GameRunning = false;
    } else if(win_initOpenGL(hInstance) != 0)
    {
        GameRunning = false;
    }
    
    // TODO: make this not garbage, just a hack for streaming on left mon for now
    //SetWindowPos(MainWindowHandle, HWND_TOP, -1600, 0, MainWindowWidth, MainWindowHeight, SWP_NOSIZE);
    
    game_memory GameMemory = {};
    win32_gamecode GameCode = {};
#if 1
    GameMemory.persistsize = GAME_MEMORY_SIZE;
    GameMemory.tempmemsize = TEMP_MEMORY_SIZE;
    
    uint64 persistsize = GameMemory.persistsize + GameMemory.tempmemsize;
    
    GameMemory.persist = (uint8*)VirtualAlloc(0, persistsize,
                                              MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    GameMemory.tempmem = GameMemory.persist + GameMemory.persistsize;
#endif
    
    while(GameRunning != 0)
    {
        
        // check game dll and swap if needed
        GameMemory.swappedgamecode = LoadGameCode(&GameCode);
        
        // input
        win_updateInput();
        win_processMessages();
        
        // update game state
        GameCode.UpdateAndRender(&GameMemory, &inputstate);
        GameMemory.timer++;
        
        
        // setup render stuff
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        RenderObject* ropointer = GameMemory.renderbufferstart;
        
        while(ropointer != 0)
        {
            switch(ropointer->type)
            {
                case RO_LINE:
                DrawLine(&ropointer->line);
                break;
                case RO_BOX:
                DrawBox(&ropointer->box);
                break;
                case RO_TEXT:
                DrawText(&ropointer->text);
                break;
                case RO_TILE:
                DrawTile(&ropointer->tile);
                default:
                break;
            }
            ropointer = ropointer->next;
        }
        
        
        
        // end of rendering
        glUseProgram(0);
        
        HDC DeviceContext = GetDC(MainWindowHandle);
        SwapBuffers(DeviceContext);
        glFinish();
        win_wait(); // this is still very bad
        
        ReleaseDC(MainWindowHandle, DeviceContext);
    }
    
    FreeLibrary(GameCode.gamecode);
    DeleteFile(GameCodeName);
    
    return 0;
}
