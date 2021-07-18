#ifndef RENDER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaders.h"
#include "lazymath.cpp"

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
