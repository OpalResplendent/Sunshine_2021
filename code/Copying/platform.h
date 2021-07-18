#ifndef PLATFORM_H
#define PLATFORM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PlayerCharacter.h"
#include "GrapplingHook.h"

#include <stdint.h> // int aliases

typedef int8_t int8;
typedef int32_t int32;

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real;
typedef float real32;

typedef uint32 bool32;

typedef uint8 u8;
typedef uint32 u32;
typedef uint64 u64;

typedef float r32;
typedef uint32 b32;
typedef uint8 b8;

#define MOUSE_BUTTONS 5
#define DIRECTION_BUTTONS 4
#define BUTTONS 12
#define MAX_CONTROLLERS 4
#define CONTROLLER_BUTTONS (DIRECTION_BUTTONS + BUTTONS)
//#define TOTAL_BUTTONS (((1+MAX_CONTROLLERS)*CONTROLLER_BUTTONS) + MOUSE_BUTTONS)

#define DIRECTIONS_CANCEL 1
//#define INACTIVE_INPUT 0
#define INACTIVE_KEYBOARD_INPUT 0
#define INACTIVE_CONTROLLER_INPUT 1 


struct input_buttonstate
{
    union {
        struct { uint8 down, downprevious, wentdown, wentup; };
        struct { uint8 d, dp, wd, wu; };
    };

};

struct controller_state
{
    union {
        input_buttonstate button[CONTROLLER_BUTTONS];
        struct {
            union {
                input_buttonstate directions[DIRECTION_BUTTONS];
                struct { input_buttonstate up, down, left, right; };
            };
            union {
                input_buttonstate buttons[BUTTONS];
                struct { input_buttonstate a, b, x, y, lb, rb, lt, rt, back, start, ls, rs; };
                struct { input_buttonstate db, rb, lb, ub, l1, r1, l2, r2, select, start, l3, r3; };
            };
        };
    };
    real32 lax, lay, rax, ray, lat, rat;
};

struct input_state
{
    union {
        controller_state virtualcontroller;
        union {
            input_buttonstate button[CONTROLLER_BUTTONS];
            struct {
                union {
                    input_buttonstate directions[DIRECTION_BUTTONS];
                    struct { input_buttonstate up, down, left, right; };
                    struct { input_buttonstate u, d, l, r; };
                };
                union {
                    input_buttonstate buttons[BUTTONS];
                    struct { input_buttonstate a, b, x, y, lb, rb, lt, rt, back, start, ls, rs; };
                    struct { input_buttonstate a, b, x, y, l1, r1, l2, r2, select, start, l3, r3; };
                };
                struct {
                    real32 lax, lay, rax, ray, lat, rat;
                };
            };

        };
    };
    controller_state controllers[MAX_CONTROLLERS];

    union {
        input_buttonstate mousebutton[MOUSE_BUTTONS]; // L, R, M, X1, X2
        struct { input_buttonstate LM, RM, MM, X1, X2; };
    };


    union {
        struct { int mousex, mousey, mousexprevious, mouseyprevious; };
        struct { int mx, my, mxp, myp; };
    };
    int8 mousewheel;
    uint8 activecontrollers;
};

struct GameState {
    float Time;
    float dT;

    void* PC;
    std::list <AABB> Platforms;

    input_state Input;
};


#endif
