#include <xinput.h>

input_state inputstate;

// xinput get and set functions
#define X_INPUT_GET_STATE(name) DWORD name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub){return(0);}
static x_input_get_state *XInputGetState_DYNAMIC = XInputGetStateStub;
#define XInputGetState XInputGetState_DYNAMIC

#define X_INPUT_SET_STATE(name) DWORD name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub){return(0);}
static x_input_set_state *XInputSetState_DYNAMIC = XInputSetStateStub;
#define XInputSetState XInputSetState_DYNAMIC


bool win_initXInput()
{
	HMODULE xinputdll = LoadLibraryA("xinput1_3.dll");
    
	if(xinputdll == 0){
		LoadLibraryA("xinput1_4.dll");
	} else if(xinputdll == 0) {
		LoadLibraryA("xinput9_1_0.dll");
	}
	// NOTE: xinput1_2.dll and xinput1_1.dll also exist
    
	if(xinputdll)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(xinputdll, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(xinputdll, "XInputSetState");
        return 0;
	} else {
        return 1;
    }
}

bool win_initRawInput()
{
    // Raw Input devices
    RAWINPUTDEVICE Rid[2];
    
    Rid[0].usUsagePage = 1;
    Rid[0].usUsage = 4; // Gamepad
    Rid[0].dwFlags = 0;
    Rid[0].hwndTarget=NULL;
    
    Rid[1].usUsagePage = 1;
    Rid[1].usUsage = 5; // Joystick
    Rid[1].dwFlags = 0;
    Rid[1].hwndTarget=NULL;
    
    bool success = RegisterRawInputDevices(Rid,2,sizeof(RAWINPUTDEVICE));
    // TODO: handle errors with GetLastError 
    
    
    return success;
}

#define MAX_KEYBINDS 8

struct keybind
{
	uint32 totalbinds;
	uint32 data[MAX_KEYBINDS];
};

// TODO: just have one keybind group instead of directions + buttons?
keybind directionmap[DIRECTION_BUTTONS];
keybind buttonmap[BUTTONS];

void win_bindKey(keybind* kb, uint32 bind)
{
    if(kb->totalbinds < MAX_KEYBINDS)
    {
        kb->data[kb->totalbinds] = bind;
        kb->totalbinds++;
    }
}

void win_unbindKey(keybind* kb)
{
	kb->totalbinds = 0;
}

void updateXInput()
{
	for(DWORD ii = 0; ii < XUSER_MAX_COUNT; ++ii)
	{
		XINPUT_STATE state = {};
		if(XInputGetState(ii, &state) == ERROR_SUCCESS)
		{
			// TODO: also returns a packet number that might be useful
			XINPUT_GAMEPAD *cc = &state.Gamepad;
			// WORD  wButtons;
            
			// reference: https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx
            // XINPUT_GAMEPAD_DPAD_UP	0x0001
			// XINPUT_GAMEPAD_DPAD_DOWN	0x0002
			// XINPUT_GAMEPAD_DPAD_LEFT	0x0004
			// XINPUT_GAMEPAD_DPAD_RIGHT	0x0008
			// XINPUT_GAMEPAD_START	0x0010
			// XINPUT_GAMEPAD_BACK	0x0020
			// XINPUT_GAMEPAD_LEFT_THUMB	0x0040
			// XINPUT_GAMEPAD_RIGHT_THUMB	0x0080
			// XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
			// XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
			// XINPUT_GAMEPAD_A	0x1000
			// XINPUT_GAMEPAD_B	0x2000
			// XINPUT_GAMEPAD_X	0x4000
			// XINPUT_GAMEPAD_Y	0x8000
            
            
            // d-pad
			inputstate.up.down += !!(cc->wButtons & XINPUT_GAMEPAD_DPAD_UP);
			inputstate.down.down += !!(cc->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			inputstate.left.down += !!(cc->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			inputstate.right.down += !!(cc->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
            
            // face buttons
			inputstate.a.down += !!(cc->wButtons & XINPUT_GAMEPAD_A);
			inputstate.b.down += !!(cc->wButtons & XINPUT_GAMEPAD_B);
			inputstate.x.down += !!(cc->wButtons & XINPUT_GAMEPAD_X);
			inputstate.y.down += !!(cc->wButtons & XINPUT_GAMEPAD_Y);
            
            // shoulder buttons
            inputstate.lb.down += !!(cc->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            inputstate.rb.down +=  !!(cc->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            inputstate.lt.down +=  (cc->bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
            inputstate.rt.down +=   (cc->bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
            
            // select and start
            inputstate.back.down +=  !!(cc->wButtons & XINPUT_GAMEPAD_BACK);
            inputstate.start.down +=  !!(cc->wButtons & XINPUT_GAMEPAD_START);
            
            // stick buttons
            inputstate.ls.down +=  !!(cc->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
            inputstate.rs.down +=  !!(cc->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
            
            // analog shit:
            // XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
            // XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
            // XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			// BYTE  bLeftTrigger;
			// BYTE  bRightTrigger;
			// SHORT sThumbLX;
			// SHORT sThumbLY;
			// SHORT sThumbRX;
			// SHORT sThumbRY;
            // max is 32767 from being a short
            
            // TODO: apply trigger threshhold here too
            inputstate.lat = cc->bLeftTrigger;
            inputstate.rat = cc->bRightTrigger;
            
            // left stick
            float lx = cc->sThumbLX;
            float ly = cc->sThumbLY;
            
            float magnitude = sqrt(lx*lx + ly*ly);
            
            if(magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            {
                if(magnitude > 32767)
                {
                    magnitude = 32767;
                }
                
                inputstate.lax = lx/magnitude;
                inputstate.lay = ly/magnitude;
                
            } else {
                inputstate.lax = 0;
                inputstate.lay = 0;
            }
            
            
            // right stick
            float rx = cc->sThumbRX;
            float ry = cc->sThumbRY;
            
            magnitude = sqrt(rx*rx + ry*ry);
            
            if(magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            {
                if(magnitude > 32767)
                {
                    magnitude = 32767;
                }
                
                inputstate.rax = rx/magnitude;
                inputstate.ray = ry/magnitude;
                
            } else {
                inputstate.rax = 0;
                inputstate.ray = 0;
            }
            
            
		} else {
			// not connected
		}
	}
    
}

struct rawinputcontroller {
    char name[64];
    uint64 vendor;
    uint64 product;
};

uint32 SupportedRIDControllersCount = 14;
rawinputcontroller SupportedRIDControllers[14] = {
    {"PS4 Controller", 0x0000054c, 0x000005c4},
    {"PS4 Controller", 0x0000054c, 0x000009cc},
    {"PS4 Controller", 0x0000054c, 0x00000ba0},
    {"iBuffalo SNES Controller", 0x00000583, 0x00002060},
    {"8Bitdo NES30 Pro", 0x00002002, 0x00009000},
    {"8Bitdo NES30 Pro", 0x00003820, 0x00000009},
    {"8Bitdo SFC30 GamePad", 0x00002810, 0x00000009},
    {"8Bitdo Zero GamePad", 0x000005a0, 0x00003232},
    {"PS3 Controller", 0x00008888, 0x00000308},
    {"PS3 Controller", 0x0000054c, 0x00000268},
    {"PS3 DualShock", 0x00000925, 0x00000005},
};

struct connectedcontroller {
    HANDLE handle;
    uint32 type;
    char name[256];
};

struct debug_ps4_controller {
    union{
        uint8 raw[11];
        struct{
            uint8 ignore[2]; // static data
            uint8 lsx, lsy, rsx, rsy; // 0x00 = left/up, 0xFF = right/down
            uint8 dpad:4, square:1, x:1, circle:1, triangle:1; // 0x8 = released, 0x0-0x7 clockwise from north
            uint8 l1:1, r1:1, l2:1, r2:1, share:1, options:1, l3:1, r3:1;
            uint8 ps:1, tpad:1, counter:6;
            uint8 lt, rt; // 0x00 = released, 0xFF = pressed
        };
    };
};

uint8 ControllersConnected;
connectedcontroller ConnectedControllers[8];

uint32 ConnectedRIDDevices = 0;

// Raw Input shit
void updateRawInput(MSG* Message)
{
    RAWINPUTDEVICELIST RIDList[32] = {0};
    UINT puiNumDevices = 0;
    GetRawInputDeviceList(NULL, &puiNumDevices, sizeof(RAWINPUTDEVICELIST));
    
    if(puiNumDevices != ConnectedRIDDevices && puiNumDevices <= 32)
    {
        ConnectedRIDDevices = puiNumDevices;
        GetRawInputDeviceList(RIDList, &puiNumDevices, sizeof(RAWINPUTDEVICELIST));
    } else {
        return;
    }
    
    RID_DEVICE_INFO devinfo[32] = {};
    UINT infosize = sizeof(RID_DEVICE_INFO);
    
    for(uint32 device = 0; device < puiNumDevices; ++device)
    {
        GetRawInputDeviceInfo(RIDList[device].hDevice, RIDI_DEVICEINFO, &devinfo[device], &infosize);
    }
    
    for(uint32 ii = 0; ii < puiNumDevices; ++ii)
    {
        if(devinfo[ii].dwType != 2)
        {
            ii = puiNumDevices;
            break;
        }
        for(uint32 jj = 0; jj < SupportedRIDControllersCount; ++jj)
        {
            if(devinfo[ii].hid.dwVendorId == SupportedRIDControllers[jj].vendor && devinfo[ii].hid.dwProductId == SupportedRIDControllers[jj].product)
            {
                // TODO: should check for name as well actually
                
                UINT namesize = 256;
                GetRawInputDeviceInfo(RIDList[ii].hDevice, RIDI_DEVICENAME, ConnectedControllers[ControllersConnected].name, &namesize);
                ConnectedControllers[ControllersConnected].handle = RIDList[ii].hDevice;
                ConnectedControllers[ControllersConnected].type = jj;
                ++ControllersConnected;
                jj = SupportedRIDControllersCount;
            }
        }
        
    }
    
    char devicename[256] = {};
    UINT namesize = 256;
    GetRawInputDeviceInfo(RIDList[1].hDevice, RIDI_DEVICENAME, devicename, &namesize);
    
    //rawinputcontroller controller = {"Testing", data2.hid.dwVendorId, data2.hid.dwProductId };
    
    infosize = 255;
}