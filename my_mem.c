#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "mem_all.h"

typedef struct item item_info;
// PROF - can I copy this hashmap implementation?
//https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
struct item {
    item_info *this_pointer; //points to mem_allocator head pointer
    item_info *end_pointer; //points to tail- the size of the allocation
    item_info *next_p; //points to next p which is the end
    size_t size;
    size_t allocated;
};

/*this routine is guaranteed to be called before any of the other routines, 
and can do whatever initialization is needed.  The memory to be managed is passed into 
this routine.*/
int global_mem_size;
/*perform any necessary initializations, such as allocating the initial heap 
area. The return value should be -1 if there was a problem in performing 
the initialization, 0 otherwise*/
void mem_init(unsigned char *my_memory, unsigned int my_mem_size) {
    //my_memory is pointing to the first allocated address
    //so you can get the next address just by my_memory + 1
    //bec it's already pointing to the address vs
    //to get the value of the address you'd do *(my_memory)
    
    /*
    //address of first spot in array for sizes
    arr_p = (int*)malloc((my_mem_size * sizeof(int)));
    arr_last_p = (int *)arr_p; 
    */
    //alternatively could make a structs array for
    //sizes and pointers
    item_info *arr_p = (item_info*)malloc(my_mem_size * sizeof(item_info));
    arr_p->size = 0;
    arr_p->this_pointer = arr_p;
    arr_p->end_pointer = arr_p;
    //the next pointer is just right after where this one ends
    arr_p->next_p = arr_p->end_pointer+1;
    //set it to be unallocated
    arr_p->allocated = 0;

    //also, allocate memory for the total allocated to 
    //be used later - points to an int
    total_allocated = (int*)malloc(sizeof(int));

    //allocate memory for the end address
    mem_max_addr = (item_info *)malloc(sizeof(int));
    //pointer to the end of the heap of allocated memory
    mem_max_addr = (item_info*)(arr_p + (my_mem_size * sizeof(item_info)));
}


//a function functionally equivalent to malloc() , but allocates it from the memory pool passed to mem_init()
void *my_malloc(unsigned size){
    /*
    loop through the structs array as long as we have not
    reached the end of the memory pool 
    */
    while(*(total_allocated)<=global_mem_size
        && arr_p-mem_max_addr!=0){ //this is to make sure we haven't reached the end of the pool
        if ((arr_p->allocated == 0) && (mem_max_addr-arr_p>=sizeof(size))){
            break;
        }
        arr_p = arr_p->next_p;
    }
    // if there's no more allocation space or we've reached the end without finding a spot
    if((*(total_allocated)<=global_mem_size) || (arr_p-mem_max_addr!=0)){
        return NULL;
    }
    //if there is a free spot
    else if(arr_p->allocated==0){
        //set the size
        arr_p->size = sizeof(size);
        //update the end and next pointers
        arr_p->end_pointer = arr_p + size;
        arr_p->next_p = arr_p->end_pointer + 1;
        //set it to be allocated
        arr_p->allocated = 1;
        //add it to the total allocation
        *total_allocated = *total_allocated + arr_p->size;

    }
    
    // returns the pointer to the current start of the allocated memory
    return arr_p;
    //HOW DO YOU ACCESS THE MEMORY POOL WITHOUT PASSING IT IN
/*returns a pointer to a block of memory of at least size bytes
that is suitably aligned for any kind of data object that mfght be contained in the
bl9ck.
*/
}


//a function equivalent to free() , but returns the memory to the pool passed to mem_init()
/*
The mm free routine frees the block pointed to by ptr. It returns nothing. 
This routine is only guaranteed to work when the passed pointer (ptr) was 
returned by an earlier call to
mm malloc or has not yet been freed
*/
void my_free(void *mem_pointer){
    //need to know where to stop freeing space
    //make a struct for each mem_pointer with a second_end pointer
    // it could be a key value pair instead of a struct
    
    //loop through the pointers and when you get to the right one
    //set the allocation to 0
    while(arr_p-(item_info*)mem_pointer!=0){
        if(arr_p-(item_info*)mem_pointer == 0){ //if you find it
            (item_info*)mem_pointer->allocated = 0;
        }
    }
    
}


typedef struct  {
    int num_blocks_used;
    int num_blocks_free;
    int smallest_block_free;
    int smallest_block_used;
    int largest_block_free;
    int largest_block_used;
    } mem_stats_struct, *mem_stats_ptr;

//provides statistics about the current allocation of the memory poo
void mem_get_stats(mem_stats_ptr mem_stats_ptr){
    
}

void print_stats(char *prefix) {
  mem_stats_struct mem_stats;

  mem_get_stats(&mem_stats);
  printf("mem stats: %s: %d free blocks, %d used blocks, free blocks: smallest=%d largest=%d, used blocks: smallest=%d largest=%d\n",
	 prefix,
	 mem_stats.num_blocks_free,
	 mem_stats.num_blocks_used,
	 mem_stats.smallest_block_free,
	 mem_stats.largest_block_free,
	 mem_stats.smallest_block_used,
	 mem_stats.largest_block_used);
}
