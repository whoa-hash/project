#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


#define ALIGNMENT 8 // must be a power of 2 for the bit allocation
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1)) //aligns to be a multiple
#define SIZE_T_SIZE (ALIGN(sizeof(size_t))) // header size

/* Private global variables */
static char *mem_heap;     /* Points to first byte of heap */ 
static char *mem_brk;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr; /* Max legal heap addr plus 1*/ 
unsigned int global_mem_size;
unsigned int total_free;
int not_allocated;
int total_used = 0;


void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
    mem_heap = (char *)malloc(my_mem_size);
    mem_brk = (char *)mem_heap;               
    mem_max_addr = (char *)(mem_heap + my_mem_size); 
    global_mem_size = my_mem_size;

}

void *sbrk(int incr) 
{
    char *old_brk = mem_brk;
    if((mem_max_addr - mem_brk) < incr){
        printf("true\n");
        return NULL;
    }

    if ( (incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        //an obvious invalid address
        return NULL; //could also change this to return null
    }

    else{
        printf("mem break is %p\n",mem_brk);
        mem_brk += incr;
        return (void *)old_brk;
    }
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

void *find_fit(size_t size) {
 size_t *header = heap_start(),
 *next;
 while (header < (size_t *)heap_end()) {
 if (!(*header & 1)) {
    if (*header >= size)
        return header;
    next = (size_t *)header + *header;
 // merge with next block if available & free
    if (next < (size_t *)heap_end() && !(*next & 1)) {
        *header += *next;
        continue;
    }
    
 }
 header = (size_t *)header + (*header & ~1L);
 }
 return NULL;
}

void *my_malloc(unsigned size) {
 total_used += size;
 printf("total used: %d\n",total_used);
 if(total_used>global_mem_size){
     return NULL;
 }
 printf("inside malloc\n");
 if((size ==0) || (size>global_mem_size)){
     return NULL;
 }
 size_t blk_size = ALIGN(size + SIZE_T_SIZE);
//  printf("blk size is %zu: \n", blk_size);
 size_t *header = find_fit(blk_size);
 if((void *)header == ((void *)-1)){
     printf("returning null");
     return NULL;
 }

 if (header && blk_size < *header)
 // split block if possible 
 *(size_t *)((char *)header + blk_size) = *header - blk_size;
 else
    header = sbrk(blk_size);
  if (header == NULL){
      return NULL;
  }

 *header = blk_size | 1;
//  printf("at end of my_malloc :%zu \n", (*header)-1);
 not_allocated = 0; //not allocated is now false
 total_used += 1;
 return (char *)header + 8; //returns a pointer to the payload (not to the header)
}

void my_free(void *ptr) {
 size_t *header = (size_t *)((char *)ptr - SIZE_T_SIZE);
 *header = *header & ~1L; //set the allocation bit to 0

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
    if(not_allocated == 1){
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
    mem_stats_ptr->smallest_block_used = 0;
    size_t *header = heap_start();
    int cur_size;
    while (header < (size_t *)heap_end()) {
        cur_size = 0;
        if((!(*header & 1)) == 1){
            mem_stats_ptr->num_blocks_free += 1;
            while((!(*header & 1)) == 1){ //there's a free block
            cur_size += *header;
            header = (size_t *)((char *)header + (*header & ~1L));      
            // printf("%d",cur_size);  
             }
        }       
        else if((!(*header & 1)) == 0){ //there's a used block
            // printf("block was used\n");
            mem_stats_ptr->num_blocks_used += 1;
            //if the blksize is smaller than the smallest used block size
            //then minusing the blksize from global memory will be the biggest free
            //block
            if (*header <mem_stats_ptr->smallest_block_used){
                mem_stats_ptr->smallest_block_used = *header -1;
                mem_stats_ptr->largest_block_free = global_mem_size-*header -1;
            }
            // printf("header %zu in used block\n", *header);
            if ((*header >mem_stats_ptr->largest_block_used)){
                mem_stats_ptr->smallest_block_free = global_mem_size-*header-1;
                mem_stats_ptr->largest_block_used = *header - 1;
            }
        }
        else printf("WHAT WOULD THIS BE?");
        // printf("this is the header %zu", *header);
        header = header + *header;
    }
    if(header == (size_t *)heap_end()-1){
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