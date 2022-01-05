#define MAX_HEAP = 1111 * 1111
void mem_init(unsigned char *my_memory, unsigned int my_mem_size);
void *my_malloc(unsigned size);
void print_stats(char *prefix);
void my_free(void *mem_pointer);
typedef struct item item_info;
extern item_info * arr_p;
extern int* arr_last_p;
extern int* total_allocated;
static item_info *mem_max_addr; /* Max legal heap addr plus 1*/
extern int global_mem_size;
/*good source that I used here
https://stackoverflow.com/questions/6618921/calling-a-function-from-another-file-in-the-same-directory-in-c
*/