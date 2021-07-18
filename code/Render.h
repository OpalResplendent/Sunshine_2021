#ifndef RENDER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaders.h"


inline v2
NormalizeV2(v2& n)
{
    float m = sqrt(n.x * n.x + n.y * n.y);
    n = n / m;
    return n;
}

inline v2
DirectionV2(v2 a, v2 b)
{
    v2 rv2 = b - a;
    NormalizeV2(rv2);
    return rv2;
}

inline v2
RotateV2Right(v2 a)
{
    v2 r = v2{ a.y, -a.x };
    return r;
}

inline float
DistanceSquaredV2(v2 a, v2 b)
{
    float r = (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
    return r;
}

inline float
CrossProductV2(v2 a, v2 b)
{
    float r = a.x * b.y - a.y * b.x;
    return r;
}


struct AABB {
    union {
        struct {
            v2 position;
            v2 size;
            v2 velocity;
        };
        //float x, y, w, h, dx, dy;
    };
};

struct Color {
    float r, g, b, a;
};

inline Color
Blend(Color lhs, Color rhs)
{
    Color result = { lhs.r + rhs.r, 
        lhs.g + rhs.g,
        lhs.b + rhs.b };
    return(result);
}

inline Color
Blend(Color a, Color b, float t)
{
    Color result = { lerp(a.r, b.r, t),
        lerp(a.g, b.g, t),
        lerp(a.b, b.b, t) };
    return(result);
}


void DrawSquare(glm::vec4 square, glm::vec3 color, Shader* shader);
void DrawAABB(AABB box, Color color, Shader* shader);

void FancyDrawAABB(AABB box, Color color, Shader* shader);

#define RENDER_H
#endif
