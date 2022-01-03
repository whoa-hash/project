void mem_init(unsigned char *my_memory, unsigned int my_mem_size);
void *my_malloc(unsigned size);
void print_stats(char *prefix);
typedef struct mem_stats_struct *mem_stats_ptr;
void mem_get_stats(mem_stats_ptr mem_stats_ptr);
void my_free(void *mem_pointer);

/*good source that I used here
https://stackoverflow.com/questions/6618921/calling-a-function-from-another-file-in-the-same-directory-in-c
*/