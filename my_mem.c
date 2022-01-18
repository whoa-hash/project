#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>

// code is inspired by the Ilinois Institute College of Science
/*alignment is n + 8 (for the header) + extra(extra-for alignment to be multiple of 8)  so if you have n bytes allocated 
it's really n + 8(header) + extra to round up to a multiple of 8 i.e. 1 byte will have a 16 byte block allocated - 1+8 = 9, round up to 16.
*/
//if it's a power of 2 you can use the last bit on the right (which is 8 bytes over) for allocation
#define ALIGNMENT 8 // must be a power of 2 for the byte allocation
//aligns to be a multiple of 8 i.e. Aligning a header which is already 8 bytes will keep it at 8 bytes
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1)) //this will just be 8 bytes for the header when you don't add in a size
#define SIZE_T_SIZE (ALIGN(sizeof(size_t))) // header size in bytes, size_t is 8 bytes;

/* Private global variables */
//char is one byte
static char *mem_heap;     /* Points to first byte of heap */ 
static char *mem_brk;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr; /* Max legal heap addr plus 1*/ 

int global_mem_size;

//for my mem_stats function
int total_used = 0;
int not_allocated =0;

void *find_fit(size_t size);

//initializes memory that I'll be using (my 'heap')
void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
    // first pointer to my heap
    mem_heap = (char *)malloc(my_mem_size);            
    mem_max_addr = (char *)(mem_heap + my_mem_size); 
    global_mem_size = my_mem_size;

    // initialize the spots in memory;   
    size_t *first = (size_t *)mem_heap;
    // initialize the first block
    *(size_t *)mem_heap = global_mem_size;   //it's a free block and has all the memory
    first++;

    // let's loop throught the memheap and initialize everything else to 0
    // loop by char because it's one byte
    for(char i=1; (char *)first<mem_max_addr; i++){
        *first = 0;
        first++;
    }
}

void *my_malloc(unsigned size) {
 // some base cases - error handling
 if((size ==0) || (size>global_mem_size)){
     return NULL;
 }
 //if we ran out of space
 if((total_used +size)>global_mem_size){  
     return NULL;
 }
  
 // 1 byte is 8 bits
 // block that's aligned by a multiple of 8 with an 8 byte header
 size_t blk_size = ALIGN(size + SIZE_T_SIZE);
 
 // find a spot for the block
 size_t *header = find_fit(blk_size); //give it a pointer/place on the heap
 if(header == NULL){
     return NULL;
 }
	
/* if there's more free space than was needed to be allocated
   instead of allocating the whole free block that we have
   let's just allocate the size that we need and then initialize the next block with the size that is left
*/
 if (header && (blk_size < *header)){
 // split block if possible 
    /* after giving the current allocation/block a size - allocating it - 
       the next free block now has a size of what's left from the free space 
    */
    *(size_t *)((char *)header + blk_size) = *header - blk_size;
    // give the header the aligned block size
    *header = blk_size;
    // now WHAT ABOUT THE ORIGINAL SIZE - we don't need it -
 
 }
 
 // else all the space is done -- after this there must be a full block
      
 total_used += (*header); //update the global variable to be used in the memory stats
 *header = blk_size | 1; //allocate the memory, set the allocated bit

 not_allocated +=1; //not allocated is now false - also to be used in memory stats
 return (char *)header + 8; //returns a pointer to the payload (not to the header)
}

void my_free(void *ptr) {
   if(NULL == ptr){
        exit(EXIT_FAILURE);
	// I think the below is really what C returns from free()
        // assert(ptr !=NULL);
   }

// get the pointer from our 'implicit array'
 size_t *header = (size_t *)((char *)ptr - SIZE_T_SIZE); // we don't need the 8 bytes from the header
	
 //invalid pointer passed in so just return
 if(header == NULL){
     //  assert(header !=NULL); //again, this is actually what C's free does I think
     return;
 }

 total_used -= ((*header) -1); // update the global variable
 *header = *header & ~1L; // set the allocation bit to 0, now the block is unallocated/free
 not_allocated -=1;	 // so update the global variable

// Here I could merge the free ones together but I don't really have to because I merge when
// I search for a spot - this would make the O(1) into O(n) where n is the number of blocks I have
}



// finds a spot for the size that is inputted
void *find_fit(size_t size) {
// start pointing to the the beginning of our heap
size_t *header = (size_t *)mem_heap,
*next;
while ((char*)header < mem_max_addr) { // while we haven't reached the end
    if (((*header) & 1) ==0) {         // if there's a spot available
        if ((int)*header >= size)      // if there's enough space to be allocated, take it
            return header;  	       // returns a pointer to the beginning of the block(including the header)

        // if there's not enough space
        next = (size_t *)((char*)header + (*header & ~1L)); //go to the next spot
        // merge with next block if available & free
        if ((char *)next < mem_max_addr && !(*next & 1)) {
            *header += *next; //add next's size
            continue;
        }
        
    } // if the spot is not free continue go to the next block in the 'implicit' array
    header = (size_t *)((char*)header + (*header & ~1L)); // have to minus the allocated bit
    
}
// we didn't find a free spot
return NULL;
}

typedef struct  {
  int num_blocks_used;
  int num_blocks_free;
  int smallest_block_free;
  int smallest_block_used;
  int largest_block_free;
  int largest_block_used;
} mem_stats_struct, *mem_stats_ptr;


// this function sort of works
void mem_get_stats(mem_stats_ptr mem_stats_ptr){
    // blocks are always a multiple of 8 because of alignment
    // so putting in i.e. 50 bytes really rounds up to the closest multiple of 8 - 64
    // this is just for after calling mem init when we haven't done anything yet
    if(not_allocated ==0){
        mem_stats_ptr->num_blocks_free = 1;
        mem_stats_ptr->num_blocks_used = 0;
        mem_stats_ptr->largest_block_used = 0;
        mem_stats_ptr->largest_block_free = global_mem_size;
        mem_stats_ptr->smallest_block_free = global_mem_size;
        mem_stats_ptr->smallest_block_used = 0;
        return;
    }
    mem_stats_ptr->num_blocks_free = 0; //everything starts off at zero
    mem_stats_ptr->num_blocks_used = 0;
    mem_stats_ptr->largest_block_used = 0;
    mem_stats_ptr->largest_block_free = -1;
    mem_stats_ptr->smallest_block_free = -1;
    mem_stats_ptr->smallest_block_used = 0;
    size_t *header = (size_t *)mem_heap;
    int smallest_free_size = global_mem_size;
    int largest_free_size = 0;
    int cur_free_size =0;
	
    //loop till the end of the heap
    while ((char *)header < mem_max_addr) {
        cur_free_size=0;
        largest_free_size = 0;
        smallest_free_size = 0;

        //start by a free block
        if((*header & 1) == 0){
           
            mem_stats_ptr->num_blocks_free += 1;
            //go until we reach an unallocated bit and stop if we've reached the end
            while(((*header & 1) != 1) && ((char *)header < mem_max_addr)){ //there's a free block
                cur_free_size += *header; //add up the sizes that are in those blocks
                header = (size_t *)((char *)header + (*header & ~1L)); //go to the next block     
            }
            // check if we've set these already - we only need to check one of them
            // because if we've set one, then we've set the other
            // or if we haven't because if we haven't we can just set to what we have
            if(mem_stats_ptr->largest_block_free == -1){
                smallest_free_size = cur_free_size;
                largest_free_size = cur_free_size;
                mem_stats_ptr->largest_block_free = cur_free_size;
                mem_stats_ptr->smallest_block_free = cur_free_size;
            }
            else{
                    // we're here at the end of a free block
                // so let's set the smallest free block if we can
                if (cur_free_size < smallest_free_size){
                    smallest_free_size = cur_free_size;
                    mem_stats_ptr->smallest_block_free = cur_free_size;
                }
                // or the largest free block if we can
                if (cur_free_size >largest_free_size){
                    largest_free_size = cur_free_size;
                    mem_stats_ptr->largest_block_free = cur_free_size;
                }
            }
            
            //won't be a free bit anymore when we get here or we've reached the end of the block
            if(header == (size_t *)mem_max_addr){
                mem_stats_ptr->largest_block_free = largest_free_size;
                mem_stats_ptr->smallest_block_free = smallest_free_size;
                return;
            }
        }    
       
        else if((*header & 1) == 1){ //there's a used block
             //need to reset free size to 0
            cur_free_size = 0;
            mem_stats_ptr->num_blocks_used += 1;


            // all the *header -1 gives the actual size block because allocation uses 1 btye
            //for the first time, the header is at the beginning of the heap
            if(header == (size_t *)mem_heap){
                // printf("we're in the first spot\n");
                largest_free_size = global_mem_size - *header;
                smallest_free_size = global_mem_size - (mem_brk-mem_heap);
                mem_stats_ptr->smallest_block_used = *header -1;
                mem_stats_ptr->largest_block_used = *header - 1;
            }
            // if we're not by the first spot in the heap
            else{
                if((*header-1) < mem_stats_ptr->smallest_block_used){
                mem_stats_ptr->smallest_block_used = *header -1;
                }
                
                if ((*header-1) > mem_stats_ptr->largest_block_used){
                    mem_stats_ptr->largest_block_used = *header -1;
                }
            }
        }
        // while we're not at the end of the heap
        header = (size_t *)((char *)header + (*header & ~1L)); //continue looping
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
