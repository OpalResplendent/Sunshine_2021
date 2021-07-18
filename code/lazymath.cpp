#ifndef LAZYMATH_H
#define LAZYMATH_H

#include <cmath>

inline float
lerp(float a, float b, float t)
{
    float result = (1 - t)* a + t * b;
    return(result);
}

union v2
{
    struct { float x, y; };
    struct { float a, b; };
    struct { float w, h; };
    struct { float width, height; };
    float data[2];
};

inline v2
operator +(v2 lhs, v2 rhs)
{
    v2 result = { lhs.x + rhs.x, lhs.y + rhs.y };
    return(result);
}

inline v2
operator -(v2 lhs, v2 rhs)
{
    v2 result = { lhs.x - rhs.x, lhs.y - rhs.y };
    return(result);
}

inline v2
operator *(v2 lhs, float rhs)
{
    v2 result = { lhs.x * rhs, lhs.y * rhs };
    return(result);
}

inline v2
operator /(v2 lhs, float rhs)
{
    v2 result = { lhs.x / rhs, lhs.y / rhs };
    return(result);
}

inline v2
operator +=(v2& lhs, v2 rhs)
{
    lhs = { lhs.x + rhs.x, lhs.y + rhs.y };
    return(lhs);
}

inline v2
operator -=(v2& lhs, v2 rhs)
{
    lhs = { lhs.x - rhs.x, lhs.y - rhs.y };
    return(lhs);
}

inline v2
operator *=(v2& lhs, float incb)
{
    lhs = { lhs.x * incb, lhs.y * incb };
    return(lhs);
}

inline bool
operator !=(v2 lhs, v2 rhs)
{
    bool result = (lhs.x = !rhs.x) && (lhs.y = !rhs.y);
    return(result);
}

inline v2
operator -(v2 lhs)
{
    v2 result = { -lhs.x, -lhs.y };
    return(result);
}


#endif
