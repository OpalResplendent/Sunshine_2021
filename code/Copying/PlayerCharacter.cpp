#include "PlayerCharacter.h"

#include "Input.h"
#include "platform.h"
#include "shaders.h"
#include "collision.h"

void PlayerCharacter::Update(GameState* gs)
{
    // Red Grappling Hook Logic ===============================================
    if (gs->Input.mousebutton[0].wd)//&& this->redHook.state == READY)
    {
        this->redHook->hook.position = this->position;
        v2 direction = DirectionV2(this->position,
            v2{(float)gs->Input.mx, (float)gs->Input.my});
        float magnitude = 1155.0f;
        this->redHook->hook.velocity = direction * magnitude;
        this->redHook->state = DEPLOYING;
    }

    if (gs->Input.mousebutton[0].wu
        && (this->redHook->state == DEPLOYING || this->redHook->state == EMBEDDED))
    {
        this->redHook->state = RETURNING;
    }
     

    // Player Movement ========================================================
    v2 gravity = { 0, -1100 };

    this->velocity += gravity * gs->dT;
    float calculateddrag = 1 / (1 + (gs->dT * this->drag));
    this->velocity *= calculateddrag;
    this->drag = 0.1f;

    AABB groundcheck = this->playerbox;
    groundcheck.position.y -= 10.0f;
    bool onground = false;

    for (std::list<AABB>::iterator pfitr = gs->Platforms.begin();
        pfitr != gs->Platforms.end(); ++pfitr)
    {
        AABB platformbox = { pfitr->position, pfitr->size, pfitr->velocity };

        if (AABBvAABB(&groundcheck, &platformbox))
        {
            onground = true;
        }
    }


    float jumpstrength = 333;
    float movespeed = 144;

    if (onground && gs->Input.virtualcontroller.a.wentdown)
    {
        this->velocity.y += jumpstrength;
    }

    if (gs->Input.virtualcontroller.left.down || gs->Input.virtualcontroller.right.down)
    {
        this->velocity.x += movespeed * gs->dT 
            * (!!gs->Input.virtualcontroller.right.down - !!gs->Input.virtualcontroller.left.down);
    }

    // collision
    float timeleft = 1.0f;
    
    while (timeleft > 0)
    {
        CollisionResult lowest = { INFINITY };
        v2 playerloopvelocity = this->velocity * gs->dT * timeleft;
        AABB playerbox = { this->position+(this->hitbox/2), this->hitbox, 
                             playerloopvelocity };

        // cycle through all the platforms and get lowest collision
        for (std::list<AABB>::iterator pfitr = gs->Platforms.begin();
            pfitr != gs->Platforms.end(); ++pfitr)
        {
            AABB platformbox = { pfitr->position, pfitr->size, pfitr->velocity };
            CollisionResult thistime = SweptAABBvAABB(&playerbox, &platformbox);

            if (thistime.time > 0 && thistime.time < lowest.time)
            {
                lowest.time = thistime.time;
                lowest.negatex = thistime.negatex;
                // TODO: save collision direction
            }
        }

        if (lowest.time == INFINITY)
        {
            // no collision, do full movement
            this->position += playerloopvelocity;
            //this->velocity *= 1.0f - (this->drag * (1.0f-(gs->dT * timeleft)));
            timeleft = 0;
        } else {
            timeleft -= lowest.time;
            v2 playerlowestvelocity = this->velocity * gs->dT * lowest.time;
            // collision, negate velocity in normal
            float epsilon = 0.2f;
            v2 direction = this->velocity;
            direction = NormalizeV2(direction);
            
            if (lowest.negatex)
            {
                //this->velocity *= 1.0f - (this->drag * (gs->dT * lowest.time));
                this->position += playerlowestvelocity;
                this->position -= direction * epsilon;
                this->velocity.x = 0;
            } else {
                if (this->velocity.y < 0)
                {
                    this->drag = 5.02f;
                }
               // this->velocity *= 1.0f-(this->drag * (gs->dT * lowest.time));
                this->position += playerlowestvelocity;
                this->position -= direction * epsilon;
                this->velocity.y = 0.0f;
            }
        }
        
    }

    // Update Grappling Hook Points
    if (this->redHook->state != READY
        && this->redHook->state != COOLDOWN)
    {
        this->redHook->points.clear();
        this->redHook->points.push_front(this->position);
        this->redHook->points.push_back(this->redHook->hook.position);
    }
}

void PlayerCharacter::Render(Shader* shader)
{
    if (this->redHook->state != READY
        && this->redHook->state != COOLDOWN)
    {
        //redHook.DrawRope(shader, glm::vec3{ 0.64f, 0.1f, 0.1f });
        redHook->Render(shader);

        // draw hook
        Color color = Color{ 0.88f, 0.44f, 0.44f };
        AABB tempbox = redHook->hook;
        tempbox.position -= tempbox.size/2;
        FancyDrawAABB(tempbox, color, shader);
    }

    // draw player
    AABB temppplayer = this->playerbox;
    temppplayer.position -= temppplayer.size / 2;
    Color color = Color{ 0.64f, 0.64f, 0.84f };

    FancyDrawAABB(temppplayer, color, shader);
}

void PlayerCharacter::DebugReset()
{
    this->redHook->hook.size = v2{ 8,8 };
    this->redHook->maxlengthsquared = 500 * 500;

    this->box = {};
    this->redHook->state = READY;

    this->box.position = v2{ 200, 200 };
    this->box.velocity = v2{ -10, 100 };
    this->box.size = v2{ 16, 32 };
}
