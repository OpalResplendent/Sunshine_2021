#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <list>
#include <iostream>

#include "platform.h"

#include "lazymath.cpp"
#include "Render.h"

class GrapplingHook;
struct GameState;

class PlayerCharacter {
public:
    union {
        AABB playerbox;
        AABB box;
        struct {
            v2 position;
            v2 hitbox;
            v2 velocity;
        };
    };
    float gravity;
    float drag;

    GrapplingHook* redHook;
    GrapplingHook* blueHook;

    void Update(GameState* gs);
    void Render(Shader* shader);
    void DebugReset();
    PlayerCharacter();
private:

};


#endif
