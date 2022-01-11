# Memory Allocater in C
This is a memory allocater, coded in C that does the 2 basic functions of C's malloc() and free().
Malloc allocates a size of bytes (AKA memory) for the user and returns a pointer to the allocated size in memory.
Free frees the size of bytes, deallocates it, so that that memory can be used again.
This implementation uses an implicit list and traverses it from the beginning each time a Malloc is performed. Therefore, the performance of malloc is dependent on the number of blocks, both allocated and free (O(n) where n is the number of blocks). Since this was implemented using the implicit list of bytes, a header of size 8 was added so that checking for allocation is easy: the bit used for allocation is always going to be either 0 for unallocated or 1 for allocated, and then if you want to check if it's allocated you can & the size bytes with a 1--> if it's 0 it's unallocated, if it's 1, it's allocated. Therefore, Free can just deallocate by setting the allocation bit to 0 and has O(1) performance.





