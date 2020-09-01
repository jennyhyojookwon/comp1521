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
static void sortList(void);


/** Initialise the Heap. */
int initHeap (int size)
{
	Heap.nFree = 0;
	Heap.freeElems = 0;

	/// TODO ///

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

	// set heapSize
	Heap.heapSize = size;

	// Allocate memory of size bytes to the Heap, set Heap.heapMem to point at the
    // first byte and zero out the entire region
	Heap.heapMem = calloc(size, sizeof(byte));

	// return -1 if it's unsuccessful
	if(Heap.heapMem == NULL) {
		return -1;
	}

	// initialize the region to be a single large free-space chunk
	header *newHeader = (header *)(Heap.heapMem);
  	newHeader->status = FREE;
  	newHeader->size = size;      
   
 
  	// allocate a freeList array (of size N/MIN_CHUNK)
  	Heap.freeList = calloc(size/MIN_CHUNK,sizeof(header *));

	// return -1 if it's unsuccessful
  	if(Heap.freeList == NULL) {
    	return -1;
  	}
 
  	// set the first item in the array to point to the single free-space chunk.
  	Heap.freeList[0] = newHeader;
   
  	// set rest of the fields for Heap:
  	Heap.nFree = 1;
  	Heap.freeElems = size/MIN_CHUNK;
 
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
	/// TODO ///

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

	// find the smallest free chunk larger than N + HeaderSize

	header *smallestFree = (header *)(Heap.freeList[0]); //A pointer to the smallest free chunk
    header *iterationHeader = (header *)(Heap.freeList[0]);

	int i = 0;
	int index = 0;
	// loop through the freeList array
	while(i < Heap.freeElems) {
		// check if the size of the chunk is less than smallestSize and greater than N + Header size
        iterationHeader = (header *)(Heap.freeList[i]);
		if(iterationHeader > 0 && (iterationHeader->size < smallestFree->size) && iterationHeader->size > (size + sizeof(header))) {
            smallestFree = iterationHeader;
            index = i;
        }
        i++;
        
    }

	//If the free chunk is smaller than N + HeaderSize + MIN_CHUNK, allocate the whole chunk.
    if(smallestFree->size < size + sizeof(header)+ MIN_CHUNK) {
        //printf("We here!\n");
        smallestFree->status = ALLOC;
        smallestFree->size = size + sizeof(header);
        Heap.freeList[index] = 0;
        Heap.nFree--;
        //sort the freeList                
        sortList();
        //return the first usable byte of data.
        return ((char *)smallestFree + sizeof(header));
    } 
	// if the free chunk is larger, then split it into two chuncks, 
	// with the lower chunck allocated for the request, and the upper chunk being a new free chunk.
	else {
        //printf("Yeet\n");
        //split it into two chunks, with the lower chunk allocated for the request, and the upper chunk being a new free chunk.
        int wholesize = smallestFree->size;
        header *lowerChunk = smallestFree;
        lowerChunk->status = ALLOC;
        lowerChunk->size = size + sizeof(header);
        //printf("lower chunk is %p\n",lowerChunk);
        //printf("The size of the lower chunk is %d\n", lowerChunk->size);
 
        //The upper chunk should be located after the lower chunk.
        header *upperChunk = (header *)(((char *)lowerChunk) + lowerChunk->size);
        //printf("Upper chunk is %p\n",upperChunk);
        upperChunk->status = FREE;
        //size of the upper chunk is equal to the size of smallest Free - the size of lowerChunk
        upperChunk->size = wholesize - lowerChunk->size;
        //printf("The size of the upper chunk is smallestFree->size (%d) - lowerChunk->size(%d) = %d\n", smallestFree->size, lowerChunk->size, upperChunk->size);
        //make freelist point to the start of the free chunk. Take the next element and set it to upperCunk.
        Heap.freeList[index] = upperChunk;
       
        //return the address after the header of the lower chunk
        return ((char *)lowerChunk + sizeof(header));
    }
	return NULL; // this just keeps the compiler quiet
}

/** Deallocate a chunk of memory. */
void myFree (void *obj)
{
	printf("\n\nNew job \nobj is pointing to %p\n",obj);
    printf("Heap.nFree = %d\n",Heap.nFree);

	//sortList();
	// if obj is pointing to free chunck or it is NULL
	// print stderr
	int i = 0;
	for(i = 0; i < Heap.nFree; i++) {
		if(Heap.freeList[i] == obj || obj == NULL) {
			fprintf(stderr, "Attempt to free unallocated chunk\n");
        	exit(1);
		}
	}

	printf("free list before inserting obj looks like: ");
	int j = 0;
    while(j<Heap.freeElems){
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
	// left neighbor is free && right neighbor is allocated
	// since obj is the first element in freeList
	if((index + 1) == Heap.nFree) {
		header *beforeChunk = (header *)(Heap.freeList[index-1]);
		uint beforeSize = beforeChunk->size;
		header *currChunk = (header *)(Heap.freeList[index]);
		uint currSize = currChunk->size;

		printf("Case 1: before free after alloc\n");
		//create a new header at the location of before.
		if(beforeChunk + beforeChunk->size != currChunk) {
			printf("not next to it\n");
			//set the chunk to free
			header *currChunk = (header *)(Heap.freeList[index]);
			uint currSize = currChunk->size;
			//put it in the freeList array
			currChunk->status = FREE;
			currChunk->size = currSize;
			Heap.nFree--;

			//increment Heap.nFree as you have
			//Heap.nFree++;
			printf("\nfree list after free not next currently looks like: ");
			j = 0;
			while(j<Heap.freeElems){
				printf("%p",Heap.freeList[j]);
			j++;
			}
			printf("\nHeap.nFree = %d\n",Heap.nFree);

		}
		else {
			newChunk = beforeChunk;
			//Set it to free: Is this necessary? I already know its free.
			newChunk->status = FREE;
			//Set the size = size of the before chunk + the current chunk
			newChunk->size = beforeSize + currSize;

			//Now, get rid of the pointer to the current index in the freeList
			Heap.freeList[index-1] = 0;
			Heap.nFree--;
		}

		//Now sort the list:
		//sortList();
		printf("case1 done\n");
	}
	// right neighbor is free
	// since index is the last free chunk
	else if (obj == Heap.freeList[0]) {
		printf("here?\n");
		header *currChunk = (header *)(Heap.freeList[index]);
		uint currSize = currChunk->size;
		header *afterChunk = (header *)(Heap.freeList[index+1]);
		uint afterSize = afterChunk->size;
		printf("Case 2: before alloc after free\n");

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
			newChunk = currChunk;
			//object is already free, but maybe just set it here again:
			newChunk->status = FREE;
			//Set the size = size of current chunk + size of after chunk
			newChunk->size = currSize + afterSize;
			
			//Now, get rid of the pointer to after in the freeList
			Heap.freeList[index] = 0;
			Heap.nFree--;
		}

		printf("case2 done\n");

		//Now sort the list:
		//sortList();
	}
	// none of the neighbors are free
	else if (Heap.nFree == 1) {
		printf("This means it was the only one in the list!\n");
		//set the chunk to free
		header *currChunk = (header *)(Heap.freeList[index]);
		uint currSize = currChunk->size;
		//put it in the freeList array
		currChunk->status = FREE;
		currChunk->size = currSize;

		//increment Heap.nFree as you have
		Heap.nFree++;
	}
	else { // else: both are free
		printf("Case 3:\n");
		
		header *currChunk = (header *)(Heap.freeList[index]);
		header *beforeChunk = (header *)(Heap.freeList[index-1]);
		header *afterChunk = (header *)(Heap.freeList[index+1]);
        
		// all next to each other
		//if(beforeChunk + (beforeChunk->size) == currChunk && currChunk+(currChunk->size) == afterChunk) {
			if((beforeChunk->size) >= (currChunk->size) && (beforeChunk->size) >= (afterChunk->size)) {
				newChunk = beforeChunk;
			}
			else if ((currChunk->size) >= (beforeChunk->size) && (currChunk->size) >= (afterChunk->size)) {
				newChunk = currChunk;
			}
			else {
				newChunk = afterChunk;
			}
			
			printf("we here now\n");

			newChunk->status = FREE;
			
			newChunk->size = (beforeChunk->size) + (currChunk->size) + (afterChunk->size);

			//Now get rid of the pointers to the current and after indexes in the freelist
			Heap.freeList[index-1] = 0;
			Heap.freeList[index+1] = 0;

			Heap.nFree = Heap.nFree - 2;
		//}
		printf("\nfree list after case3 currently looks like: ");
	    j = 0;
    	while(j<Heap.freeElems){
        	printf("%p",Heap.freeList[j]);
       		j++;
   		}


		//Now, sort the list:
		//sortList();
	}
	//return;
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

 
static void sortList(void){
    int j,k;
    for(j=0;j<Heap.freeElems-1;j++){
        for (k = 0; k < Heap.freeElems-j-1; k++) {
            if (Heap.freeList[k] > Heap.freeList[k+1]){  
                header *store = Heap.freeList[j];
                Heap.freeList[j]=Heap.freeList[k];
                Heap.freeList[k]= store;
            }
        }
    }
}