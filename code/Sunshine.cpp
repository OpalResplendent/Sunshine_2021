#include "platform.h"
#include "Sunshine.h"


void
initialize(game_memory* GameMemory)
{
    game_state *GameState = (game_state*)GameMemory->persist;
    pcg_state *RNG = &GameState->RNG;
    pcg_seed(RNG);
    
    GameMemory->initialized = true;
}

extern "C"
void GameUpdateAndRender(game_memory* GameMemory, input_state* InputState)
{
    // memory
    game_state *GameState = (game_state*)GameMemory->persist;
    pcg_state *RNG = &GameState->RNG;
    float testfloat = 200;
    //std::fmod(testfloat, 100);
    testfloat += 2;
    
    if(!GameMemory->initialized)
    {
        initialize(GameMemory);
    }
    
    if(GameMemory->swappedgamecode) {
        
    }
    
    // update
    GameMemory->timer++;
    
    return;
}
