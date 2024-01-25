/*******************************************************************************************
*	Stack style memory allocator.  Allows for better lifetime grouping of memory by virtually
*	Transforming the heap into a stack.  As of now, this is Windows exclusive functionality,
*	Thus removing our ability to do cross platform building, but adding in some preprocessing
*	Commands for a linux build should be relatively painless.  I'm not gonna bother with Mac
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "memarena.h"
#include <memoryapi.h>
#include <stdio.h>
#include <math.h>


#define GB 1073741824
#define PAGE 4096

void *PushMemory(Arena *a, uint64_t size);

Arena *CreateArena(uint64_t chunk, uint8_t size, uint32_t pages)
{
	if (pages*PAGE > size*GB)
	{
		printf("Failed to create memory arena due to desired size being greater than virtual size!");
		return (Arena*){0};
	}
	Arena *a = malloc(sizeof(Arena));
	// This makes sure that we are byte aligned in our chunks.
	if (chunk%8 != 0)
	{
		int mod = 8 - (chunk%8);
		chunk += mod;
	}
	// Reserve virtual space so our dats is continuous and not fragmented even if we expand later
	a->mem = VirtualAlloc(NULL, size*GB, MEM_RESERVE, PAGE_NOACCESS);
	a->offset = 0;
	a->size = chunk;
	a->firstEmpty = 0;
	a->max = size;
	a->psize = pages*PAGE;
	// Commit actual physical memory
	VirtualAlloc(a->mem, pages*PAGE, MEM_COMMIT, PAGE_READWRITE);
	return a;
}

void DestroyArena(Arena *a)
{
	VirtualFree(a->mem, 0, MEM_RELEASE);
	free(a);
}

// mem should be an array of UNITITIALIZED pointers (might want to change into an array instead of a pointer pointer
// to avoid free and malloc)
void** ar_ArrayAlloc(Arena *a, int count)
{
	void** mem = malloc(sizeof(void*) * count);
	int i = 0;
	//We should attempt to get our fragmented memory, if any.
	if (a->firstEmpty != 0)
	{
		MemPosition *m = {1};
		while (m != 0 && count > 0)
		{
			mem[i] = a->firstEmpty->pos;
			m = a->firstEmpty;
			a->firstEmpty = m->next;
			free(m);
			count--;
			i++;
		}
		if (count == 0)
		{
			return mem;
		}
	}
	void *initm = PushMemory(a, count);
	if (initm != 0)
	{
		for (int k = 0; count > 0; count--)
		{
			mem[i] = initm + (k*a->size);
			i++;
			k++;
		}
		return mem;
	}
	else
	{
		printf("Array allocation failed on stack!");
		free(mem);
		return NULL;
	}
}

void* ar_AllocOneFromArray(Arena *a)
{
	void** m = ar_ArrayAlloc(a, 1);
	void* mem = m[0];
	free(m);
	return mem;
}

void ar_Free(Arena *a, void *m)
{
	// If the memory is at the end of our stack...
	if ((a->offset+a->mem) - a->size == m)
	{
		PopMemory(a, 1);
		return;
	}
	// Else we fragment our memory
	MemPosition *d = malloc(sizeof(MemPosition));
	d->pos = m;
	d->next = a->firstEmpty;
	a->firstEmpty = d;
}

// Jumps over our fragmented memory and returns only allocated memory positions
void* ar_ArenaIterator(Arena *a, int *i)
{
	//void* mem;
	while(true)
	{
		if (a->firstEmpty != NULL && (a->mem + ((*i)*a->size)) == a->firstEmpty)
		{
			*i++;
			a->skip = a->firstEmpty->next;
			continue;
		}
		if (a->skip != NULL && (a->mem + ((*i)*a->size)) == a->skip)
		{
			*i++;
			MemPosition* hold = a->skip->next;
			a->skip = hold;
			continue;
		}
		return a->mem + ((*i)*a->size);
	}
	
}

void ar_ClearAll(Arena *a)
{
	a->offset = 0;
}

void *PushMemory(Arena *a, uint64_t size)
{
	// Since our Arena a->size should be a multiple of 8, there should be no reason to do byte alignemnt checks
	// This is enforced in Arena creation code
	
	// We first check to see if our new desired size is within the physical limits we have already allocated.
	long int diff = a->psize - a->offset+(size*a->size);
	// Expands our physical space within the virtual stack by (diff/4096) + 1 pages, if possible.
	if (diff < 0)
	{
		diff = abs(diff);
		// Integer division to find how many pages we need.
		diff = ceil(diff/PAGE);
		int addMem = diff*PAGE;
		if (a->psize+addMem > a->max*GB)
		{
			printf("Failed to push to stack: Not enough Virtual Memory in the Arena!");
			return 0;
		}
		VirtualAlloc(a->mem+a->psize, addMem, MEM_COMMIT, PAGE_READWRITE);
		a->psize += addMem;
	}
	void *mem = a->mem+a->offset;
	a->offset += size*a->size;
	return mem;
}

void PopMemory(Arena *a, uint64_t size)
{
	a->offset -= (size*a->size);
	if (a->offset < 0)
	{
		a->offset = 0;
	}
}














