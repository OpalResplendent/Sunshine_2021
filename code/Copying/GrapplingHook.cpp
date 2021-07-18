#include "GrapplingHook.h"
#include "collision.h"

void GrapplingHook::Update(GameState* gs)
{
    if (this->state == DEPLOYING &&
        this->maxlengthsquared < DistanceSquaredV2(pc->position, this->hook.position))
    {
        this->state = RETURNING;
    }

    if (this->state == DEPLOYING)
    {
        CollisionResult lowest = { INFINITY };
        AABB hookbox = { this->hook.position + this->hook.size / 2, this->hook.size,
                        this->hook.velocity * gs->dT };

        // cycle through all the platforms and get lowest collision
        for (std::list<AABB>::iterator pfitr = gs->Platforms.begin();
            pfitr != gs->Platforms.end(); ++pfitr)
        {
            AABB platformbox = { pfitr->position, pfitr->size, pfitr->velocity };
            CollisionResult thistime = SweptAABBvAABB(&hookbox, &platformbox);

            if (thistime.time > 0 && thistime.time < lowest.time)
            {
                lowest.time = thistime.time;
                lowest.negatex = thistime.negatex;
            }
        }

        if (lowest.time == INFINITY)
        {
            // no collision, do full movement
            this->hook.position += this->hook.velocity * gs->dT;
        }
        else {
            // collision, kill velocity, set state
            this->hook.position += this->hook.velocity * gs->dT * lowest.time;
            //this->velocity = v2{ 0,0 };

            this->state = EMBEDDED;

            float epsilon = 0.2f;
            v2 direction = this->hook.velocity;
            direction = NormalizeV2(direction);
            this->hook.position -= direction * epsilon;
        }
    }

    if (this->state == RETURNING)
    {
        v2 direction = DirectionV2(this->hook.position, pc->position);
        float magnitude = 755.0f;
        this->hook.velocity = direction * magnitude;
        this->hook.position += this->hook.velocity * gs->dT;

        float maxdist = magnitude / 30;
        if (maxdist * maxdist > DistanceSquaredV2(pc->position, this->hook.position))
        {
            this->state = COOLDOWN;
        }
    }

    if (this->state == COOLDOWN)
    {
        this->state = READY;
    }

    if (this->state == EMBEDDED)
    {
        v2 direction = DirectionV2(pc->position, this->hook.position);
        this->pullStrength = 2500;
        pc->velocity += direction * this->pullStrength * gs->dT;
    }
}


void
GrapplingHook::Render(Shader* shader)
{
    unsigned int numtriangles = (this->points.size() - 1) * 2;
    unsigned int pointsbytes = sizeof(float) * this->points.size() * 12 * 2;
    unsigned int elementsbytes = sizeof(unsigned int) * numtriangles * 3;
    float* points = (float*)malloc(pointsbytes);
    unsigned int* elements = (unsigned int*)malloc(elementsbytes);

    std::list<v2>::iterator itr = this->points.begin();
    v2 current = *itr++;
    v2 next = *itr;
    //std::list<v2>::iterator next = current++;

    // first
    float ropewidth = 3.33;
    ropewidth /= 2;
    v2 direction = DirectionV2(current, next);
    v2 rightangle = RotateV2Right(direction);
    unsigned int index = 0;

    points[index + 0] = (current + (rightangle * ropewidth)).x;
    points[index + 1] = (current + (rightangle * ropewidth)).y;
    points[index + 2] = 0.1f;
    points[index + 3] = color.r;
    points[index + 4] = color.g;
    points[index + 5] = color.b;

    points[index + 6] = (current - (rightangle * ropewidth)).x;
    points[index + 7] = (current - (rightangle * ropewidth)).y;
    points[index + 8] = 0.1f;
    points[index + 9] = color.r;
    points[index + 10] = color.g;
    points[index + 11] = color.b;

    //current++;
    index += 12;

    // middle
    for (int ii = 1; ii < this->points.size() - 1; ++ii)
    {
        // TODO: Middle part
    }

    // last
    direction = -direction;
    rightangle = RotateV2Right(direction);
    current = next;

    points[index + 0] = (current + (rightangle * ropewidth)).x;
    points[index + 1] = (current + (rightangle * ropewidth)).y;
    points[index + 2] = 0.1f;
    points[index + 3] = color.r;
    points[index + 4] = color.g;
    points[index + 5] = color.b;

    points[index + 6] = (current - (rightangle * ropewidth)).x;
    points[index + 7] = (current - (rightangle * ropewidth)).y;
    points[index + 8] = 0.1f;
    points[index + 9] = color.r;
    points[index + 10] = color.g;
    points[index + 11] = color.b;

    //current++;
    //index += 12;

    // element index
    for (int ii = 0; ii < numtriangles / 2; ++ii)
    {
        elements[ii * 6 + 0] = ii + 0;
        elements[ii * 6 + 1] = ii + 1;
        elements[ii * 6 + 2] = ii + 2;
        elements[ii * 6 + 3] = ii + 0;
        elements[ii * 6 + 4] = ii + 3;
        elements[ii * 6 + 5] = ii + 2;
    }

    // render
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pointsbytes, points, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementsbytes, elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numtriangles * 3, GL_UNSIGNED_INT, 0);

    // free
    free(points);
    free(elements);
}
