void myFree (void *obj)
{
	 
	/// TODO ///
	header *objHeader = (header *)(obj);
	// if argument to myFree() does not represent an allocated chunk in the heap, print error
	if(objHeader->status != ALLOC) {
		fprintf(stderr, "%s", "Attempt to free unallocated chunk\n" );
		exit(1);
	}
	// if the argument is an address somewhere in the middle of an allocated block, print error
	// pointer to the start of the header
	// pointer to the end of the block
	header *start = objHeader;
	startOffset = heapOffset(start);
	header *end = (header *)(obj + obj->size);
	endOffset = heapOffset(end);

	objOffset = heapOffset(obj);

	// loop through and check if the argument is equal
	while(startOffset < endOffset) {
		if(objOffset = startOffset) {
			fprintf(stderr, "%s", "Attempt to free unallocated chunk\n" );
			exit(1);
		}
		startOffset = startOffset + sizeof(byte);
	}

	// free chuncks adjacent to the new free chunk are merged into a single larger free chunk
	// get left bound (check left adjacent)
	

	// get right bound (check right adjacent)
	header *rightBound = (header *)(obj + obj->size);
	if(rightBound->status == ALLOC) {
		rightBound = rightBound + 
	}
	
}