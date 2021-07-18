#ifndef GAPPLINGHOOK_H
#define GAPPLINGHOOK_H

#include "platform.h"
#include "lazymath.cpp"
#include "Render.h"

#include <list>

enum HOOK_STATE { READY, COOLDOWN, DEPLOYING, EMBEDDED, RETURNING };

class PlayerCharacter;
struct GameState;

class GrapplingHook {
public:
    HOOK_STATE state;

    AABB hook;

    float length;
    float maxlengthsquared;
    float pullStrength;
    Color color;
    Color hookColor;

    std::list <v2> points;

    void Update(GameState* gs);
    void Render(Shader* shader);

    void Shoot(v2 start, v2 target);
    void Return();
private:
};


#endif
