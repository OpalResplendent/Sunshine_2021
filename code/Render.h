#ifndef RENDER_H

#include <glad/glad.h>
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

void DrawAABB(AABB box, Color color, Shader* shader);
void FancyDrawAABB(AABB box, Color color, Shader* shader);


enum RENDER_OBJECT {RO_LINE, RO_BOX, RO_TEXT, RO_TILE, RO_TYPES};

#define RO_COLOR union{uint32 color;struct {uint8 r, g, b, a;};}

struct RO_Line {
	real32 x1, y1;
	RO_COLOR;
	real32 x2, y2;
};

enum RO_BOX_STYLE {RO_BOX_FILLED, RO_BOX_LINES, RO_BOX_STYLE_TYPES};

struct RO_Box{
	real32 x, y;
	RO_COLOR;
	real32 h, w;
	RO_BOX_STYLE style;
};

struct RO_Text {
	real32 x, y; // bottom left pixel
	RO_COLOR;
	real32 scale;
	uint32 style;
	char* text;
};

struct RO_Tile {
	real32 x, y;
	uint32 textureindex;
	uint32 number;
	real32 sizex, sizey;
};

struct RenderObject {
	RENDER_OBJECT type;
	union{
		RO_Line line;
		RO_Box box;
		RO_Text text;
		RO_Tile tile;
	};
    uint64 depth;
	RenderObject* next;
};


#define RENDER_H
#endif
