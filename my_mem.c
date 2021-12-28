
//this routine is guaranteed to be called before any of the other routines, and can do whatever initialization is needed.  The memory to be managed is passed into this routine.
void mem_init(unsigned char *my_memory, unsigned int my_mem_size) {
    //something here
}


//a function functionally equivalent to malloc() , but allocates it from the memory pool passed to mem_init()
void *my_malloc(unsigned size){
/*returns a pointer to a block of memory of at least size bytes
that is suitably aligned for any kind of data object that mfght be contained in the
bl9ck.
*/
}


//a function equivalent to free() , but returns the memory to the pool passed to mem_init()
void my_free(void *mem_pointer){

}



//provides statistics about the current allocation of the memory poo
void mem_get_stats(mem_stats_ptr mem_stats_ptr){
    typedef struct  {
    int num_blocks_used;
    int num_blocks_free;
    int smallest_block_free;
    int smallest_block_used;
    int largest_block_free;
    int largest_block_used;
    } mem_stats_struct, *mem_stats_ptr;
    //QUESTION FOR PROF SHAMASH ABOUT THIS MEM-STATS-PTR ERROR
    //I JUST COPY-PASTED. SO SHOULD I CHANGE THIS?
}