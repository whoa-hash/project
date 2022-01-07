#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// code is inspired by the Ilinois Institute College of Science
/*alignment is 8 + 8 for the header so if you have n bits allocated 
it's really n*alignment(8) + 8(header) i.e. 1 bit will have a 16 bit block allocated
*/
#define ALIGNMENT 8 // must be a power of 2 for the bit allocation
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1)) //aligns to be a multiple
#define SIZE_T_SIZE (ALIGN(sizeof(size_t))) // header size

/* Private global variables */
static char *mem_heap;     /* Points to first byte of heap */ 
static char *mem_brk;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr; /* Max legal heap addr plus 1*/ 

unsigned int global_mem_size;
unsigned int total_free;
int total_used = 0;
//for my mem_stats function
int not_allocated =0;

//initializes memory that I'll be using (my 'heap')
void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
    mem_heap = (char *)malloc(my_mem_size);
    mem_brk = (char *)mem_heap;               
    mem_max_addr = (char *)(mem_heap + my_mem_size); 
    global_mem_size = my_mem_size;

}

//places the new data into a new place in the array
void *sbrk(int incr) 
{
    char *old_brk = mem_brk;

    //if we're trying to access memory that we don't have return null
    if((incr < 0) || (mem_max_addr - mem_brk) < incr){
        printf("true\n");
        return NULL;
    }

    else{
        printf("mem break is %p\n",mem_brk);
    }
    // point to the top of the (now) heap, and return the pointer that was passed in
    printf("mem break is %p\n",mem_brk);
    mem_brk += incr;
    return (void *)old_brk;
    
}

/* return address of the first heap byte
 */
void *heap_start()
{
    return (void *)mem_heap;
}

/* 
 * return address of last heap byte
 */
void *heap_end()
{
    return (void *)(mem_brk - 1);
}

// finds a spot for the size that is inputted
void *find_fit(size_t size) {
 size_t *header = heap_start(),
 *next;

while (header < (size_t *)heap_end()) {
    if (!(*header & 1)) {
        if (*header >= size) //if there's enough space to be allocated, take it
            return header;
        next = (size_t *)header + *header; //go to the next spot
    // merge with next block if available & free
        if (next < (size_t *)heap_end() && !(*next & 1)) {
            *header += *next;
            continue;
        }
        
    } //if the spot is not free continue looping
    header = (size_t *)header + (*header & ~1L);
}
//we didn't find a free spot
return NULL;
}

void *my_malloc(unsigned size) {

 if(total_used>global_mem_size){
     return NULL;
 }
 printf("inside malloc\n");
 if((size ==0) || (size>global_mem_size)){
     return NULL;
 }
 size_t blk_size = ALIGN(size + SIZE_T_SIZE);
 printf("size_t %lu and total size is: %lu and blksize is%lu", SIZE_T_SIZE,(size+SIZE_T_SIZE), blk_size);
 size_t *header = find_fit(blk_size);
//  if(header == NULL){
//      printf("returning null");
//      return NULL;
//  }

 if (header && blk_size < *header)
 // split block if possible 
 *(size_t *)((char *)header + blk_size) = *header - blk_size;
 else
    header = sbrk(blk_size);
  if (header == NULL){
      return NULL;
  }
 total_used = total_used + *header;
 *header = blk_size | 1;

 not_allocated = not_allocated + 1; //not allocated is now false
 
 return (char *)header + 8; //returns a pointer to the payload (not to the header)
}

void my_free(void *ptr) {
   if(ptr == NULL){
        printf("Aborting because it is a null pointer and that's what happens in free implemented by c\njust as well I could've made it return null");
        assert(ptr !=NULL);
   }
    
 size_t *header = (size_t *)((char *)ptr - SIZE_T_SIZE);
 //invalid pointer passed in
 assert(header !=NULL);
 if(header == NULL){
     printf("invalid pointer");
     return;
 }
 printf("my freed pointer has size %lu", *header);
 total_used -= (size_t)header;
 *header = *header & ~1L; //set the allocation bit to 0
 printf("my freed pointer after setting allocation has size %lu", *header);
 not_allocated -=1;

}

typedef struct  {
  int num_blocks_used;
  int num_blocks_free;
  int smallest_block_free;
  int smallest_block_used;
  int largest_block_free;
  int largest_block_used;
} mem_stats_struct, *mem_stats_ptr;

void mem_get_stats(mem_stats_ptr mem_stats_ptr){
    //blocks are always a multiple of 8 because of alignment
    // so putting in i.e. 50 really rounds up to the closest multiple of 8 - 64
    if(!(not_allocated >0)){
        mem_stats_ptr->num_blocks_free = 1;
        mem_stats_ptr->num_blocks_used = 0;
        mem_stats_ptr->largest_block_used = 0;
        mem_stats_ptr->largest_block_free = global_mem_size;
        mem_stats_ptr->smallest_block_free = global_mem_size;
        mem_stats_ptr->smallest_block_used = 0;
        return;
    }
    mem_stats_ptr->num_blocks_free = 1;
    mem_stats_ptr->num_blocks_used = 0;
    mem_stats_ptr->largest_block_used = 0;
    mem_stats_ptr->largest_block_free = global_mem_size;
    mem_stats_ptr->smallest_block_free = global_mem_size;
    mem_stats_ptr->smallest_block_used = -1;
    size_t *header = heap_start();
    int smallest_free_size = global_mem_size;
    int largest_free_size = 0;
    int cur_free_size;
    //loop till the end of the heap
    while (header < (size_t *)heap_end()) {
        cur_free_size = 0;
        largest_free_size = 0;
        smallest_free_size = 0;
        if((!(*header & 1)) == 1){
            mem_stats_ptr->num_blocks_free += 1;
            while((!(*header & 1)) == 1){ //there's a free block
            cur_free_size += *header;
            header = (size_t *)((char *)header + (*header & ~1L));   //anding it helps with alignment   
           
            }
            if (cur_free_size < smallest_free_size){
                smallest_free_size = cur_free_size;
            }
            if (cur_free_size >largest_free_size){
                largest_free_size = cur_free_size;
            }
            //won't be a free bit anymore when we get here 
            printf("freed size is: %d\n",cur_free_size);
        }    
       
        else if((*header & 1) == 1){ //there's a used block
            
          
            mem_stats_ptr->num_blocks_used += 1;
            //if the blksize is smaller than the smallest used block size
            //then minusing the blksize from global memory will be the biggest free
            //block

            //for the first time
            if(largest_free_size == 0 && smallest_free_size==0){
                largest_free_size = global_mem_size - (mem_brk-mem_heap);
                smallest_free_size = global_mem_size - (mem_brk-mem_heap);
            }
            if(mem_stats_ptr->smallest_block_used == -1){
                mem_stats_ptr->smallest_block_used = *header -1;
            }
            if (*header <mem_stats_ptr->smallest_block_used){
                mem_stats_ptr->smallest_block_used = *header -1;
                mem_stats_ptr->largest_block_free = largest_free_size;
            }
            // printf("header %zu in used block\n", *header);
            if ((*header >mem_stats_ptr->largest_block_used)){
                mem_stats_ptr->smallest_block_free = smallest_free_size;
                // mem_stats_ptr->smallest_block_free = global_mem_size-*header-1;
                mem_stats_ptr->largest_block_used = *header - 1;
            }
        }
        else printf("WHAT WOULD THIS BE?");
        
        // header = header + *header;
        header = (size_t *)((char *)header + (*header & ~1L)); //continue looping
    }
    if(header == (size_t *)heap_end()-1){ //it will never get here, probably
        printf("true");
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
