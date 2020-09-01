// COMP1521 19t2 ... Assignment 2: heap management system
//THIS WORKS WITH TEST 1 and 2.
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
 
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
 
    //If the size is less than the minimum size, set it to the minimum size
    if(size < MIN_HEAP){
        size = MIN_HEAP;
    }
    //If the size is not a multiple of 4, round up to the next multiple of 4.
    if(size%4!= 0){
        size = size +(4-(size%4));
    }
    Heap.heapSize = size;
    //Allocate memory of size bytes to the Heap, set Heap.heapMem to point at the
    //first byte and zero out the entire region
    Heap.heapMem = calloc(size, sizeof(byte));
 
    //Check that calloc was successful
    if(Heap.heapMem == NULL){
      return -1;
    }
 
    //Initialise the region to be a single Free-Space Chunk:
    //Create a header for the region:
    header *newHeader = (header *)(Heap.heapMem);
    newHeader->status = FREE;
    newHeader->size = size;      
   
 
    //Allocate a freeList array
    Heap.freeList = calloc(size/MIN_CHUNK,sizeof(header *));
    if(Heap.freeList == NULL){
      return -1;
    }
 
    //Set the first item in the array to point to the single free-space chunk.
    Heap.freeList[0] = newHeader;
 
   
    //Initialise fields of Heap:
    Heap.nFree = 1; //There is now one free chunk
    Heap.freeElems = size/MIN_CHUNK;
 
    //What about the condition about successfully returning?
 
    return 0;
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
    if(size <1 ){
        return NULL;
    }
    if(size%4!= 0){
        size = size +(4-(size%4));
    }
 
    int i = 0;
    int index = 0;
    header *smallestFree = (header *)(Heap.freeList[0]); //A pointer to the smallest free chunk
    header *iterationHeader = (header *)(Heap.freeList[0]);
    //dereference the pointer to each free chunk and check that the size of the chunk
    //is less than smallestSize and greater than N + Header size
    while(i < Heap.freeElems){
        iterationHeader = (header *)(Heap.freeList[i]);
        if (iterationHeader !=0 && iterationHeader->size < (smallestFree->size) && (iterationHeader->size) > (size + sizeof(header))){
            smallestFree = iterationHeader;
            index = i;
           
        }
        i++;
    }
 
    //printf("1: free list currently looks like: ");
    //int j = 0;
   // while(j<Heap.freeElems){
     //   printf("%p",Heap.freeList[j]);
     //   j++;
    //}
    //printf("index is %d\n", index);
    //printf("size is %d\n",size);
    //printf("size + sizeof(header)+ MIN_CHUNK is %lu\n",size + sizeof(header)+ MIN_CHUNK);
    //printf("smallestFree->size is %u\n", smallestFree->size );
    //If the free chunk is smaller than N + HeaderSize + MIN_CHUNK, allocate the whole chunk.
    if(smallestFree->size < size + sizeof(header)+ MIN_CHUNK){
        smallestFree->status = ALLOC;
        smallestFree->size = size +sizeof(header);
        Heap.freeList[index]= 0;
        //It used to be Heap.freeElems-- but this isn't supposed to change,
        Heap.nFree--;
        //sort the freeList                
        //sortList();
        //return the first usable byte of data.
        printf("1: I am returning %p\n",((char *)smallestFree + sizeof(header)) );
        return ((char *)smallestFree + sizeof(header));
 
    } else {
        //split it into two chunks, with the lower chunk allocated for the request, and the upper chunk being a new free chunk.
        int wholesize = smallestFree->size;
        header *lowerChunk = smallestFree;
        lowerChunk->status = ALLOC;
        lowerChunk->size = size + sizeof(header);
 
        //The upper chunk should be located after the lower chunk.
        header *upperChunk = (header*) (((char*) lowerChunk) + lowerChunk->size);
   
        upperChunk->status = FREE;
        //size of the upper chunk is equal to the size of smallest Free - the size of lowerChunk
        upperChunk->size = wholesize - lowerChunk->size;
        //make freelist point to the start of the free chunk. Take the next element and set it to upperCunk.
        Heap.freeList[index] = upperChunk;
 
        //resort the list.
        //sortList();
 
        //return the address after the header of the lower chunk
        printf("2: I am returning %p\n",((char *)(lowerChunk) + sizeof(header)));
        return ((char *)(lowerChunk) + sizeof(header));
    }
}
 
/** Deallocate a chunk of memory. */
void myFree (void *obj)
{
    //sortList();
    //What is object is null? Then it can't be allocced
    printf("obj is pointing to %p\n",obj);
    printf("Heap.nFree = %d\n",Heap.nFree);
   
    //find obj in the list;
    int i = 0;
    int inList = 0;
    while(i < Heap.freeElems){
        //printf("Heap.freeList[i] is %p \n", Heap.freeList[i]);
        if(Heap.freeList[i] == obj || obj == NULL){
            printf("It's in the list at index :%d\n", i);
            inList = 1;
        }
        i++;
    }
 
 
    //If the object passed is free
    if(inList ==1 ){
        fprintf(stderr, "Attempt to free unallocated chunk\n");
        exit(1);  
    } else { //it's allocated.
   
    //put the pointer in the free list array.
    i = 0;
    while(i < Heap.freeElems){
        if(Heap.freeList[i] == 0){
            break;
        }
        i++;
    }
    Heap.freeList[i] = obj;
 
    //sort the list
    sortList();
   
    //Find the index of the pointer in the freeList Array:
    i = 0;
    int index = 0;
    while(i < Heap.freeElems){
        if(Heap.freeList[i] == obj){
            index = i;
        }
        i++;
    }
    printf("free list currently looks like: ");
    int j = 0;
    while(j<Heap.freeElems){
        printf("%p",Heap.freeList[j]);
       j++;
   }
   
    printf("\nThe index of the freeList array containing object is %d\n", index);
 
 
    //Three cases: neither are free, one before is free, one after is free, both are free
    bool before = false;
    bool after = false;
 
    //Now, we need to determine whether things on either side are free or not.
    //If we're the only one in the list
    if(Heap.nFree == 1){//i.e. there is only me in the list and no before and after
        before = false;
        after = false;
        printf("only one in the list\n");
    //We can tell if the next is free or not. If index == nFree -1 then there can't be a free after
    } else if (index == Heap.nFree){ //because we've added something to the list this is the index
        printf("it's the last one in the list\n");
        before = true;
        after = false;
    //if it's at the beginning of the list, check object matches the first element
    } else if (obj == Heap.freeList[0]){
        printf("it's the first in the list\n");
        before = false;
        after = true;
    } else { //otherwise it's in the middle somewhere
        printf("it's in the middle somewhere\n");
        before = true;
        after = true;
    }
 
    //This is the new header for our merged memory
    header * newChunk;
 
   //Now, go through the cases:
        //If neither are free, we'll do nothing because we've already made the chunk free and put it in freeList
        //If before is free but after is not: Merge the before and current together
        if(before == true && after == false){
            header *beforeChunk = (header *)(Heap.freeList[index-1]);
            uint beforeSize = beforeChunk->size;
            header *currChunk = (header *)(Heap.freeList[index]);
            uint currSize = currChunk->size;
 
            printf("Case 1: before free after alloc\n");
            //create a new header at the location of before.
            newChunk = beforeChunk;
            //Set it to free: Is this necessary? I already know its free.
            newChunk->status = FREE;
            //Set the size = size of the before chunk + the current chunk
            newChunk->size = beforeSize + currSize;
 
            //Now, get rid of the pointer to the current index in the freeList
            Heap.freeList[index] = 0;
           
            //Now sort the list:
            //sortList();
 
        //If before is Alloced and the after is free: Merge the current and after together.
        } else if (before == false && after == true){
            header *currChunk = (header *)(Heap.freeList[index]);
            uint currSize = currChunk->size;
            header *afterChunk = (header *)(Heap.freeList[index+1]);
            uint afterSize = afterChunk->size;
            printf("Case 2: before alloc after free\n");
            //newChunk points at object
            newChunk = currChunk;
            //object is already free, but maybe just set it here again:
            newChunk->status = FREE;
            //Set the size = size of current chunk + size of after chunk
            newChunk->size = currSize + afterSize;
           
            //Now, get rid of the pointer to after in the freeList
            Heap.freeList[index+1] = 0;
 
            //Now sort the list:
            //sortList();
 
        //If before is free and after is free: Merge before, current and after together.
        } else if (before == true && after == true){
            printf("Case 3: before free and  after free\n");
            header *currChunk = (header *)(Heap.freeList[index]);
            header *beforeChunk = (header *)(Heap.freeList[index-1]);
            header *afterChunk = (header *)(Heap.freeList[index+1]);
			printf("we are here");
            uint beforeSize = beforeChunk->size;
            uint currSize = currChunk->size;
            uint afterSize = afterChunk->size;
            newChunk = beforeChunk;
 
            newChunk->status = FREE;
           
            newChunk->size = beforeSize + currSize + afterSize;
 
            //Now get rid of the pointers to the current and after indexes in the freelist
            Heap.freeList[index] = 0;
            Heap.freeList[index+1] = 0;
 
            Heap.nFree--;
 
            //Now, sort the list:
            //sortList();
 
        } else{
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
    }
 
   
    return;
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
            if (Heap.freeList[k] <= Heap.freeList[k+1]){  
                header *store = Heap.freeList[j];
                Heap.freeList[j]=Heap.freeList[k];
                Heap.freeList[k]= store;
            }
        }
    }
}