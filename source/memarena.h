#ifndef memarenah
#define memarenah
#include "common.h"


typedef struct MemPosition
{
	void 		   	   *pos;  // The actual memory pointer
	struct MemPosition *next; // Linked list for dynamic allocation within the stack
} MemPosition;

// A basic holder for dynamic memory allocation and returning.
typedef struct
{
	void    **memory;
	uint64_t  fragCount;	// How much of the memory is fragmented?  If, for instance, 0x0, 0x32 and 0xFF, the first two 
							// holding 4 bytes of memory.  Thus, we have a frag count of 2, as the last entry is our 
							// unfragremnted allocation of memory.
} AllocatedMem;

typedef struct
{
	uint8_t     *mem;        // The 0 index of our memory
	uint64_t     offset;     // Our current base offset within that memory
	uint8_t		 max;		 // The max reserved space within our allocator, in GB
	uint64_t     size;	     // The size of the memory for each item within the Arena.
	uint64_t     psize;		 // The actually allocated size of our memory.  If offset is greater than this, we need to expand.
	MemPosition *firstEmpty; // For dynamic allocation, allows us to pop things out of the middle of the stack if necessary.
	MemPosition *skip;		 // Skip memory holder for iteration purposes
} Arena;



void  PopMemory(Arena *a, uint64_t size);
Arena *CreateArena(uint64_t chunk, uint8_t size, uint32_t initChunks);
void DestroyArena(Arena *a);
void ar_ClearAll(Arena *a);
void** ar_ArrayAlloc(Arena *a, int count);
void ar_Free(Arena *a, void *m);
void* ar_AllocOneFromArray(Arena *a);
void* ar_ArenaIterator(Arena *a, int *i);

//Some macros to make things a bit simpler
#define MakeDataArena(type, VirtualGB, pageCount)     CreateArena(sizeof(type), VirtualGB, pageCount)
#define ar_AllocOne(arena)						   	  ar_AllocOneFromArray(arena)

// By going in reverse in our array, we should be freeing from the top of the stack if possible
#define ar_FreeArray(arena, array, count)		   for (int i = count-1; i >= 0; i--){ar_Free(arena,array[i]);}























#endif