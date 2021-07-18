#ifndef COLLISION_H

#include "lazymath.cpp"
#include "PlayerCharacter.h"
#include "platform.h"

struct CollisionResult {
	float time;
	bool negatex;
};

inline float
OriginLSvLS(v2 vector, v2 start, v2 end)
{
	v2 r = vector;
	v2 s = end - start;

	float numerator = CrossProductV2(start, r);
	float denominator = CrossProductV2(r, s);

	if (numerator == 0 && denominator == 0)
	{
		// co-linear lines
		return INFINITY;

		// can check if they overlap or touch but whatever
	}

	if (denominator == 0)
	{
		// parallel lines
		return INFINITY;
	}

	float u = numerator / denominator;
	float t = CrossProductV2(start, s) / denominator;
	if ((t >= 0) && (t <= 1) && (u >= 0) && (u <= 1))
	{
		return t;
	}

	return(INFINITY);
}


inline CollisionResult
RayvBox(AABB* box)
{
	float Return = INFINITY;
	float comparing;
	bool negatex = 0;

	// unpack the box
	float minx = box->position.x;
	float maxx = box->position.x + box->size.x;
	float miny = box->position.y;
	float maxy = box->position.y + box->size.y;

	// Top edge
	comparing = OriginLSvLS(box->velocity, v2{ minx, maxy }, v2{ maxx, maxy });
	if (comparing < Return) {
		Return = comparing;
		negatex = false;
	}

	// Bottom edge
	comparing = OriginLSvLS(box->velocity, v2{ minx, miny }, v2{ maxx, miny });
	if (comparing < Return) {
		Return = comparing;
		negatex = false;
	}

	// Left edge
	comparing = OriginLSvLS(box->velocity, v2{ minx, miny }, v2{ minx, maxy });
	if (comparing < Return) {
		Return = comparing;
		negatex = true;
	}

	// Right edge
	comparing = OriginLSvLS(box->velocity, v2{ maxx, miny }, v2{ maxx, maxy });
	if (comparing < Return) {
		Return = comparing;
		negatex = true;
	}

	// Returns INFINITY if no collision
	return(CollisionResult{ Return, negatex });
}

inline CollisionResult
SweptAABBvAABB(AABB* boxa, AABB* boxb)
{
	CollisionResult Return;

	v2 relativeVelocity = boxa->velocity - boxb->velocity;

	// pack a box with relative position, combined size, and relative velocity
	AABB tempbox = AABB{ boxb->position - boxa->position,
						 boxa->size + boxb->size,
						 relativeVelocity };

	Return = RayvBox(&tempbox);

	return(Return);
}

inline bool
AABBvAABB(AABB* boxa, AABB* boxb)
{	

	v2 halfsize = (boxa->size + boxb->size) / 2;
	v2 position = boxa->position - boxb->position;

	if ( position.x - halfsize.x < 0 && position.x + halfsize.x > 0
		&& position.y - halfsize.y < 0 && position.y + halfsize.y > 0)
	{
		return true;
	}
	return false;
}

#define COLLISION_H
#endif