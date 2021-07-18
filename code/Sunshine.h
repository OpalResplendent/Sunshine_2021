#if SGE_DEBUG
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}
#else
#define Assert(Expression)
#endif

#define sizeofarray(array) (sizeof(array)/sizeof(array[0]))

struct memory_arena
{
	uint32 size;
	uint32 used;
	uint8* base;
};

#define pushstruct(arena, type) (type*)pushsize_(arena, sizeof(type))
#define pusharray(arena, count, type) (type*)pushsize_(arena, count * sizeof(type))
void*
pushsize_(memory_arena *arena, uint32 size)
{
	void* Result = arena->base + arena->used;
	arena->used += size;
	return(Result);
};

struct camera_state
{
	float x;
	float y;
	float targetx;
	float targety;
	float boxwidth;
	float boxheight;
	float heightoffset;
	float camerasnapspeed;
	bool fallingfar;
};

struct game_state
{
	//bool initialized;
	pcg_state RNG;
	//uint32 networkstatus;
    
	uint32 level[32*32];
    
	memory_arena gamememory;
};
