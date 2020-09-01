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

// static:
static void mergeTwoAddress(void *b1, void *b2);
static int bs(void *key, int low, int high, void *freeList[]);
static void deleteSorted(void *key);
static void insertSorted(void *key);
static void mergeAdjacent(void);


/// Variables:

/** The heap proper. */
static struct heap Heap;

/// Functions:

static addr heapMaxAddr (void);
//static void sortList(void);


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
		if(size % 4 == 1) {
			size = size + 3;
		}
		else if (size % 4 == 2) {
			size = size + 2;
		}
		else if( size % 4 == 3) {
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

	// find the smallest free chunk larger than N + HeaderSize
	int i = 0;
	int index = 0;
	// loop through the freeList array
	while(i < Heap.freeElems) {
		
        iterater = (header *)(Heap.freeList[i]);
		// iterater should be greater than zero
		// check if the size of the chunk is less than smallestSize and greater than N + Header size
		if(iterater > 0 && (iteraterSize < smallestSize) && (iteraterSize > (size + sizeof(header)))) {
            // set iterater as smallest
			smallest = iterater;
            index = i;
        }
        i++;
        
    }

	//If the free chunk is smaller than N + HeaderSize + MIN_CHUNK, allocate the whole chunk.
    if(smallestSize < size + sizeof(header) + MIN_CHUNK) {
        //printf("We here!\n");
        smallest->status = ALLOC;
        smallestSize = size + sizeof(header);
        Heap.freeList[index] = 0;
        Heap.nFree--;
        //sort the freeList                
        //sortList();
        //return 
        return ((char *)smallest + sizeof(header));
    } 
	// if the free chunk is larger, then split it into two chuncks, 
	// with the lower chunck allocated for the request, and the upper chunk being a new free chunk.
	else {
        //printf("Yeet\n");
        //split it into two chunks
		// set lower chunck as ALLOC and the upper chunck becomes the new FREE
        int bigChunk = smallestSize;
        header *lower = smallest;
		uint lowerSize = lower->size;
        lower->status = ALLOC;
        lowerSize = size + sizeof(header);
        //printf("lower chunk is %p\n",lowerChunk);
        //printf("The size of the lower chunk is %d\n", lowerChunk->size);
 
        //The upper chunk should be located after the lower chunk.
        header *upper = (header *)(((char *)lower) + lowerSize);
		uint upperSize = upper->size;
        //printf("Upper chunk is %p\n",upperChunk);
        upper->status = FREE;
        //size of the upper chunk is equal to the size of smallest Free - the size of lowerChunk
        upperSize = bigChunk - lowerSize;
        //printf("The size of the upper chunk is smallestFree->size (%d) - lowerChunk->size(%d) = %d\n", smallestFree->size, lowerChunk->size, upperChunk->size);
        //make freelist point to the start of the free chunk. Take the next element and set it to upperCunk.
        Heap.freeList[index] = upper;
       
        //return the address after the header of the lower chunk
        return ((char *)lower + sizeof(header));
    }
	return NULL; // this just keeps the compiler quiet
}

static int bs(void *key, int low, int high, void **freeList){
   if(high < low)
      return -1;
   int mid = (low + high)/2;
   if(key == Heap.freeList[mid])
      return mid;
   if (key > Heap.freeList[mid])
      return bs(key, mid+1,high, Heap.freeList);
   return bs(key, low, mid-1, Heap.freeList);
}

static void deleteSorted(void *key) {
   // find the position of element to be deleted
   int pos = bs(key, 0, Heap.nFree, Heap.freeList);
   if(pos == -1){
      printf("element not found\n");
      return;
   }
   // Deleting Elements
   int i;
   for (i = pos; i < Heap.nFree - 1; i++){
      Heap.freeList[i] = Heap.freeList[i+1];
   }
   Heap.nFree--;
}

// merge two thingy
static void mergeTwoAddress(void *b1, void *b2) {
   header *m1 = (header *)b1;
   header *m2 = (header *)b2;
   m1->size += m2->size;
}

static void mergeAdjacent(){
   void *curr = Heap.freeList[0];
   header *chunk = (header *)curr;
   for (int i = 0; i < Heap.nFree; i++) {
      chunk = (header*)curr;
      while (((void *)(char *)chunk + chunk->size) == Heap.freeList[i+1]){
         mergeTwoAddress(Heap.freeList[i], Heap.freeList[i+1]);
         deleteSorted(Heap.freeList[i+1]);
      }
      curr = Heap.freeList[i++];
   }
   
}

static void insertSorted(void *key) {
   int i;
   for (i = Heap.nFree -1; (i >= 0 && Heap.freeList[i] > key); i--)
      Heap.freeList[i+1] = Heap.freeList[i];
   Heap.freeList[i+1] = key;
   // increment elements in free list
   Heap.nFree++;
}

void myFree (void *obj) {
	header *chunk = (header*)obj;
	chunk--;
	// or if the argument is an address somewhere in the middle of an allocated block
	if(chunk->status != ALLOC){
		fprintf(stderr, "Attempt to free unallocated chunk\n");
		exit(1);
	}
	chunk->status = FREE;
	insertSorted(chunk);
	mergeAdjacent();
}
/** Deallocate a chunk of memory. */
/*void myFree (void *obj)
{
	printf("\n\nNew job \nobj is pointing to %p\n",obj);
    printf("Heap.nFree = %d\n",Heap.nFree);

	//sortList();
	// if obj is pointing to free chunck or it is NULL
	// print stderr
	assert(obj != NULL);

	int i = 0;
	for(i = 0; i < Heap.nFree; i++) {
		if(Heap.freeList[i] == obj) {
			fprintf(stderr, "Attempt to free unallocated chunk\n");
        	exit(1);
		}
	}

	// how do we check if obj is pointing to the middle of the chunk?

	printf("free list before inserting obj looks like: ");
	int j = 0;
    while(j < Heap.freeElems) {
        printf("%p",Heap.freeList[j]);
       j++;
   }
   printf("\n");

	// move obj to freeList array
	for(i = 0; i < Heap.nFree; i++) {
		if(Heap.freeList[i] == 0) 
			break;
	}
	Heap.freeList[i] = obj;
	Heap.nFree++;
	//sortList();

	printf("free list after inserting obj looks like: ");
	j = 0;
    while(j<Heap.freeElems){
        printf("%p",Heap.freeList[j]);
       j++;
   }
   printf("\n");
    
	printf("Heap.nFree is incremented = %d\n",Heap.nFree);
	
	// find the index of the pointer in the freeList array
	int index = 0;
	for(i = 0; i < Heap.nFree; i++) {
		if(Heap.freeList[i] == obj) 
			index = i;
	}
	printf("\nThe index of the freeList array containing object is %d\n", index);
 
	// header for new merged chunk
	header *newChunk;
	
	// cases to consider:
	header *currChunk = (header *)(Heap.freeList[index]);
	uint currSize = currChunk->size;
	header *beforeChunk = currChunk;
	header *afterChunk = currChunk;

	if(index == 0) {
		printf("case1: only after\n");
		// we only have after
		afterChunk = (header *)(Heap.freeList[index + 1]);
		uint afterSize = afterChunk->size;
		// check adjacency
		if(currChunk+currSize != afterChunk) {
			//set the chunk to free
			header *currChunk = (header *)(Heap.freeList[index]);
			uint currSize = currChunk->size;
			//put it in the freeList array
			currChunk->status = FREE;
			currChunk->size = currSize;

			//increment Heap.nFree as you have
			//Heap.nFree++;
		}
		else {
			//newChunk points at object
			if(currChunk->size >= afterChunk->size) {
				newChunk = currChunk;
				Heap.freeList[index] = 0;
			}
			else {
				newChunk = afterChunk;
				Heap.freeList[index + 1] = 0;
			}

			//object is already free, but maybe just set it here again:
			newChunk->status = FREE;
			//Set the size = size of current chunk + size of after chunk
			newChunk->size = currSize + afterSize;
			
			//Now, get rid of the pointer to after in the freeList
			
			Heap.nFree--;
			sortList();
		}
	}
	else if(index + 1 == Heap.nFree) {
		printf("case2: only before\n");
		// we only have before
		beforeChunk = (header *)(Heap.freeList[index - 1]);
		uint beforeSize = beforeChunk->size;

		if(beforeChunk+beforeSize != currChunk) {
			printf("not next to each other\n");
			//set the chunk to free
			header *currChunk = (header *)(Heap.freeList[index]);
			uint currSize = currChunk->size;
			//put it in the freeList array
			newChunk = currChunk;
			newChunk->status = FREE;
			newChunk->size = currSize;

			//increment Heap.nFree as you have
			Heap.nFree--;
		}
		else {
			//newChunk points at object
			if(currChunk->size >= beforeChunk->size) {
				newChunk = currChunk;
				Heap.freeList[index-1] = 0;
			}
			else {
				newChunk = beforeChunk;
				Heap.freeList[index] = 0;
			}

			//object is already free, but maybe just set it here again:
			newChunk->status = FREE;
			//Set the size = size of current chunk + size of after chunk
			newChunk->size = currSize + beforeSize;
			
			//Now, get rid of the pointer to after in the freeList
			
			Heap.nFree--;
			sortList();
		}
	}
	// none of the neighbors are free
	else if (Heap.nFree == 1) {
		printf("case3: This means it was the only one in the list!\n");
		//set the chunk to free
		header *currChunk = (header *)(Heap.freeList[index]);
		uint currSize = currChunk->size;
		//put it in the freeList array
		currChunk->status = FREE;
		currChunk->size = currSize;

		//increment Heap.nFree as you have
		//already incremented
		//Heap.nFree++;
	}
	else {
		printf("case4: both\n");
		// obj is in the middle of freeList
		// we have both beforeChunk and afterChunk
		beforeChunk = (header *)(Heap.freeList[index - 1]);
		uint beforeSize = beforeChunk->size;
		afterChunk = (header *)(Heap.freeList[index + 1]);
		uint afterSize = afterChunk->size;

		if(beforeChunk+beforeSize == currChunk && currChunk+currSize == afterChunk) {
			printf("case: side\n");
			if((beforeChunk->size) >= (currChunk->size) && (beforeChunk->size) >= (afterChunk->size)) {
				newChunk = beforeChunk;
				Heap.freeList[index] = 0;
				Heap.freeList[index+1] = 0;
			}
			else if ((currChunk->size) >= (beforeChunk->size) && (currChunk->size) >= (afterChunk->size)) {
				newChunk = currChunk;
				Heap.freeList[index-1] = 0;
				Heap.freeList[index+1] = 0;
			}
			else {
				newChunk = afterChunk;
				Heap.freeList[index-1] = 0;
				Heap.freeList[index] = 0;
			}
			newChunk->status = FREE;
			newChunk->size = (beforeChunk->size) + (currChunk->size) + (afterChunk->size);
			Heap.nFree = Heap.nFree - 2;
			sortList();
		}
		else if(beforeChunk+beforeSize != currChunk && currChunk+currSize == afterChunk) {
			printf("case: only after\n");
			if(currSize >= afterSize) {
				newChunk = currChunk;
				Heap.freeList[index+1] = 0;
			}
			else {
				newChunk = afterChunk;
				Heap.freeList[index] = 0;
			}
			newChunk->status = FREE;
			newChunk->size = (currChunk->size) + (afterChunk->size);
			Heap.nFree = Heap.nFree--;
			sortList();
		}
		else if(beforeChunk+beforeSize == currChunk && currChunk+currSize != afterChunk) {
			printf("case: before\n");
			if(currSize >= beforeSize) {
				newChunk = currChunk;
				Heap.freeList[index-1] = 0;
			}
			else {
				newChunk = beforeChunk;
				Heap.freeList[index] = 0;
			}
			newChunk->status = FREE;
			newChunk->size = (currChunk->size) + (beforeChunk->size);
			Heap.nFree = Heap.nFree--;
			sortList();
		}
		else {
			printf("case: only one\n");
			header *currChunk = (header *)(Heap.freeList[index]);
			uint currSize = currChunk->size;
			//put it in the freeList array
			currChunk->status = FREE;
			currChunk->size = currSize;
		}
	}

	return;
}
*/

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

 
