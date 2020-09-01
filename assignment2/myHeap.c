// COMP1521 19t2 ... Assignment 2: heap management system

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myHeap.h"

/** minimum total space for heap */
#define MIN_HEAP 4096
/** minimum amount of space for a free Chunk (excludes Header) */
#define MIN_CHUNK 32


#define ALLOC 0x55555555
#define FREE  0xAAAAAAAA
#define LISTSIZE size / MIN_CHUNK

/// Types:

typedef unsigned int  uint;
typedef unsigned char byte;

typedef uintptr_t     addr; // an address as a numeric type

/** The header for a chunk. */
typedef struct header {
	uint status;    /**< the chunk's status -- ALLOC or FREE */
	uint size;      /**< number of bytes, including header */
	byte data[];    /**< the chunk's data -- not interesting to us */
} header;

/** The heap's state */
struct heap {
	void  *heapMem;     /**< space allocated for Heap */
	uint   heapSize;    /**< number of bytes in heapMem */
	void **freeList;    /**< array of pointers to free chunks */
	uint   freeElems;   /**< number of elements in freeList[] */
	uint   nFree;       /**< number of free chunks */
};



/// Variables:

/** The heap proper. */
static struct heap Heap;

/// Functions:

static addr heapMaxAddr (void);


/** Initialise the Heap. */
int initHeap (int size)
{
	Heap.nFree = 0;
	Heap.freeElems = 0;

	// if N is less than MIN_HEAP, set N as MIN_HEAP
	if (size < MIN_HEAP) {
		size = MIN_HEAP;
	}
	// Rounded up the value of N to the nearest multiple of 4
	if (size % 4 != 0) {
		if (size % 4 == 1) {
			size = size + 3;
		}
		else if (size % 4 == 2) {
			size = size + 2;
		}
		else if (size % 4 == 3) {
			size = size + 1;
		}
	}

	// set Heap.heapSize as size
	Heap.heapSize = size;
	// allocate Heap.heapMem of size
	Heap.heapMem = calloc(size, sizeof(byte));
	// Heap.heapMem shouldn't be NULL
	assert(Heap.heapMem != NULL);
	// initialize new
	header *new = (header *)(Heap.heapMem);
  	new->status = FREE;
  	new->size = size;      
   
  	// allocate a freeList array (of size N/MIN_CHUNK)
  	Heap.freeList = calloc(LISTSIZE, sizeof(header *));
	assert(Heap.freeList != NULL);
  	// set the fields for Heap:
  	Heap.freeList[0] = new;
  	Heap.nFree = 1;
  	Heap.freeElems = LISTSIZE;
 
	return 0; // this just keeps the compiler quiet
}

/** Release resources associated with the heap. */
void freeHeap (void)
{
	free (Heap.heapMem);
	free (Heap.freeList);
}

/** Allocate a chunk of memory large enough to store `size' bytes. */
void *myMalloc (int size)
{

	// if myMalloc() is called with a value less than 1, it should return NULL.
	if (size < 1) {
		return NULL;
	}

	// if N is not a multiple of 4, it should be increased to the next multiple of 4
	if (size % 4 != 0) {
		if (size % 4 == 1) {
			size = size + 3;
		}
		else if (size % 4 == 2) {
			size = size + 2;
		}
		else if (size % 4 == 3) {
			size = size + 1;
		}
	}

	// header to store a pointer to the smallest free chunk
	header *smallest = (header *)(Heap.freeList[0]); 
	uint smallestSize = smallest->size;
	// header for iterating Heap.freeList
    header *iterater = (header *)(Heap.freeList[0]);
	uint iteraterSize = iterater->size;
	assert(iterater != 0);

	// find the smallest free chunk larger than N + HeaderSize
	int index = 0;
	for (int i = 0; i < Heap.freeElems; i++) {
		// check if the size of the chunk is less than smallestSize and greater than N + Header size
        iterater = (header *)(Heap.freeList[i]);
		if((iteraterSize < smallestSize) && (iteraterSize > (size + sizeof(header)))) {
            smallest = iterater;
            index = i;
        }
	}

	// if the free chunk is smaller than N + HeaderSize + MIN_CHUNK, allocate the whole chunk.
    if(smallestSize < size + sizeof(header) + MIN_CHUNK) {
        smallest->status = ALLOC;
        smallestSize = size + sizeof(header);
        Heap.freeList[index] = 0;
        Heap.nFree--;

        return ((char *)smallest + sizeof(header));
    } 
	// split it into two chunks
	// set lower chunck as ALLOC and the upper chunck becomes the new FREE
	else {
        uint bigChunk = smallestSize;
        header *lower = smallest;
		lower->status = ALLOC;
        lower->size = size + sizeof(header);

		// upper should be located after lower
        header *upper = (header *)(((char *)lower) + lower->size);
        // keep the status
		upper->status = FREE;
        // size should be bigChunk - lowerSize
		upper->size = bigChunk - (lower->size);
		// the index of freeList should now be pointing to upper
        Heap.freeList[index] = upper;
       
        return ((char *)lower + sizeof(header));
    }
	return NULL; // this just keeps the compiler quiet
}


void myFree (void *obj) {
	
	assert(obj != NULL);

	header *freeChunk = (header *)obj;
	freeChunk--;

	// if the argument to myFree() does not represent an allocated chunk in the heap, 
	// the program should print erro message
	if(freeChunk->status != ALLOC) {
		fprintf(stderr, "Attempt to free unallocated chunk\n");
		exit(1);
	}

	freeChunk->status = FREE;

	// insert freeChunk to freeList in correct position
	int i;
	for (i = Heap.nFree - 1; ((i >= 0) && ((header *)Heap.freeList[i] > freeChunk)); i--) {
		Heap.freeList[i + 1] = Heap.freeList[i];
	}
	// store freeChunk
	Heap.freeList[i + 1] = freeChunk;
	// increment nFree
	Heap.nFree++;

	// merge chuncks if they are adjacent to each other
	void *curr = Heap.freeList[0];
    header *chunk = (header *)curr;
    for (int i = 0; i < Heap.nFree; i++) {
      chunk = (header *)curr;
      while (((void *)(char *)chunk + chunk->size) == Heap.freeList[i + 1]) {
		//merge the size of the two adjacent chunk
		header *before = (header *)Heap.freeList[i];
		header *after = (header *)Heap.freeList[i + 1];
		before->size += after->size;

		// find the index of the chunck that needs to be deleted
		int index;
		for(int j = 0; j < Heap.nFree; j++) {
			if(Heap.freeList[j] == Heap.freeList[i + 1]) {
				index = j;
			}
		}

		// delete the chunk
		int v;
		for (v = index; v < Heap.nFree - 1; v++) {
			Heap.freeList[v] = Heap.freeList[v + 1];
		}
		Heap.nFree--;
      }
      curr = Heap.freeList[i++];
   }
}

/** Return the first address beyond the range of the heap. */
static addr heapMaxAddr (void)
{
	return (addr) Heap.heapMem + Heap.heapSize;
}

/** Convert a pointer to an offset in the heap. */
int heapOffset (void *obj)
{
	addr objAddr = (addr) obj;
	addr heapMin = (addr) Heap.heapMem;
	addr heapMax =        heapMaxAddr ();
	if (obj == NULL || !(heapMin <= objAddr && objAddr < heapMax))
		return -1;
	else
		return (int) (objAddr - heapMin);
}

/** Dump the contents of the heap (for testing/debugging). */
void dumpHeap (void)
{
	int onRow = 0;

	// We iterate over the heap, chunk by chunk; we assume that the
	// first chunk is at the first location in the heap, and move along
	// by the size the chunk claims to be.
	addr curr = (addr) Heap.heapMem;
	while (curr < heapMaxAddr ()) {
		header *chunk = (header *) curr;

		char stat;
		switch (chunk->status) {
		case FREE:  stat = 'F'; break;
		case ALLOC: stat = 'A'; break;
		default:
			fprintf (
				stderr,
				"myHeap: corrupted heap: chunk status %08x\n",
				chunk->status
			);
			exit (1);
		}

		printf (
			"+%05d (%c,%5d)%c",
			heapOffset ((void *) curr),
			stat, chunk->size,
			(++onRow % 5 == 0) ? '\n' : ' '
		);

		curr += chunk->size;
	}

	if (onRow % 5 > 0)
		printf ("\n");
}

