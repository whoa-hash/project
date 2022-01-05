#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "mem_all.h"

//maybe move the global variables that you use here
typedef struct item item_info;
// PROF - can I copy this hashmap implementation?
//https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
struct item {
    item_info *this_pointer; //points to mem_allocator head pointer
    // item_info *end_pointer; //points to tail- the size of the allocation
    // item_info *next_p; //points to next p which is the end
    size_t size;
    size_t allocated;
    unsigned char *my_mem; //or you can calculate it 
};

/*this routine is guaranteed to be called before any of the other routines, 
and can do whatever initialization is needed.  The memory to be managed is passed into 
this routine.*/

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
    arr_p->size = my_mem_size; //nothing has been allocated so we can just set the whole thing
    arr_p->this_pointer = arr_p;
    // arr_p->end_pointer = arr_p;
    arr_p->my_mem = my_memory;
    //the next pointer is just right after where this one ends
    // arr_p->next_p = arr_p->end_pointer+1;
    //set it to be unallocated
    arr_p->allocated = 0;

    //also, allocate memory for the total allocated to 
    //be used later - points to an int
    total_allocated = (int*)malloc(sizeof(int));

    //save memory size
    global_mem_size = my_mem_size;

    //allocate memory for the end address
    mem_max_addr = (item_info *)malloc(sizeof(int));
    //pointer to the end of the heap of allocated memory
    mem_max_addr = (item_info*)(arr_p + (my_mem_size * sizeof(item_info)));

}

void * find_free(unsigned size){
    //while you haven't reached the end of the memory pool
    //find the first free block, go till you reach a not free block and if it's not enough size,
    //then keep on going till you find another free block and keep on repeating.
    int i=0;
    int cur_size = 0;
    item_info *blockp;
    //if there's no more allocation space we can return right away
    if (global_mem_size - *total_allocated < size) return NULL;

    while(i<=global_mem_size){
        blockp = &arr_p[i];
        while(arr_p[i].allocated != 1){
            cur_size = cur_size + arr_p[i].size;

            // if we have enough space to allocate
            if(cur_size>=size){
                return blockp;
            }
        }
        // if there wasn't enough size bytes we fell here so let's reset the size
        cur_size = 0;
    }
    // if there was no spot found, return null
    return NULL;

}

//a function functionally equivalent to malloc() , but allocates it from the memory pool passed to mem_init()
void *my_malloc(unsigned size){
    //ignore size 0
    if (size==0){
        return NULL;
    }
    //need to somehow set the logic for the rest of the struct pointers, set their values, maybe iterate over them

    //maybe make a separate function for finding a free space
    /*
    loop through the structs array as long as we have not
    reached the end of the memory pool 
    */
    if(find_free(size) == NULL){  // if there's no more allocation space or we've reached the end without finding a spot
        return NULL;
    }

    //if there is a free spot
    else{
        item_info *return_p = (item_info*)find_free(size);
        //set the size
        return_p->size = size;
        //update the end and next pointers
        // arr_p->end_pointer = arr_p + size;
        // arr_p->next_p = arr_p->end_pointer + 1;

        //HOW DO I DO THIS?
        //set the current pointer to the address from the memory pool with a previous pointer
        //can just go to previous pointer with arr[i]
        return_p->my_mem = (arr_p - sizeof(item_info))->my_mem + sizeof(item_info); //gives the previous pointer's address and then we just, get 
        //its location
        
        //set it to be allocated
        return_p->allocated = 1;
        //add it to the total allocation
        *total_allocated = *total_allocated + return_p->size;
        // returns the pointer to the current start of the allocated memory
         /*returns a pointer to a block of memory of at least size bytes
        */
        return return_p;
    }
}


//a function equivalent to free() , but returns the memory to the pool passed to mem_init()
/*
The mm free routine frees the block pointed to by ptr. It returns nothing. 
This routine is only guaranteed to work when the passed pointer (ptr) was 
returned by an earlier call to
mm malloc or has not yet been freed
*/
void my_free(void *mem_pointer){
    int i = 0;
    while(i<=global_mem_size){ //this is to make sure we haven't reached the end of the pool
        //if it is allocated 
        if ((arr_p[i].allocated == 1) && (arr_p[i].my_mem == mem_pointer)){
            //unallocate the info
            arr_p[i].allocated = 0;
            total_allocated = total_allocated - arr_p[i].size;

        }
        i++;
    }

    //once you free a space, if you allocated let's say 3 structs for a continuous size, 
    //then wouldn't you need to know where that pointer ends? or take away the other pointers? 
    //so that the next time you loop through, you pass to the end of the allocated structs, contiguous blocks?

    //need to know where to stop freeing space
    //make a struct for each mem_pointer with a second_end pointer
    // it could be a key value pair instead of a struct
    
}


typedef struct  {
    int num_blocks_used;
    int num_blocks_free;
    int smallest_block_free;
    int smallest_block_used;
    int largest_block_free;
    int largest_block_used;
    } mem_stats_struct, *mem_stats_ptr;

//provides statistics about the current allocation of the memory pool
void mem_get_stats(mem_stats_ptr mem_stats_ptr){
    int i = 0;
    // the size was originally set to be the whole thing
    if (arr_p[i].size == global_mem_size){
         //with no allocations there should be zero allocated and 1 free and the largest 
        mem_stats_ptr->num_blocks_used = 0;
        mem_stats_ptr->num_blocks_free = 1;
        mem_stats_ptr->smallest_block_free = global_mem_size;
        mem_stats_ptr->smallest_block_used = 0;
        mem_stats_ptr->largest_block_free = global_mem_size;
        mem_stats_ptr->largest_block_used = 0;
        return;
    }
    mem_stats_ptr->num_blocks_used = 0;
    mem_stats_ptr->num_blocks_free = 1;
    mem_stats_ptr->smallest_block_free = global_mem_size;
    // mem_stats_ptr->smallest_block_used = 0; = we don't want this because nothing is going to be less than this
    // mem_stats_ptr->largest_block_free = global_mem_size; same logic except that this is greater than
    mem_stats_ptr->largest_block_used = 0;

    //loop through the list or until a pointer hasn't been initialized
    while(i<=global_mem_size || &arr_p[i] == NULL){
        if (arr_p[i].allocated == 1){
            //it'll start off at zero
            mem_stats_ptr->num_blocks_used +=1;

            if(arr_p[i].size <mem_stats_ptr->smallest_block_used){
                mem_stats_ptr->smallest_block_used = arr_p[i].size;
            }
            if(arr_p[i].size >mem_stats_ptr->largest_block_used){
                mem_stats_ptr->largest_block_used = arr_p[i].size;
            }
        }
        //if it's not 1 then how are you going to represent an unused block??? i guess with null
        //or if it has a 0 allocated and a size?
        else{
            if (arr_p[i].size < mem_stats_ptr->smallest_block_free){
                mem_stats_ptr->smallest_block_free = arr_p[i].size;
            }
            if (arr_p[i].size < mem_stats_ptr->smallest_block_free){
                mem_stats_ptr->smallest_block_free = arr_p[i].size;
            }
            mem_stats_ptr->num_blocks_free += 1;
            printf("in the else for the get mem stats");
    
        }
    
    }
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
