#include "header_file.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
    unsigned int global_mem_size = 30;
    unsigned char *global_memory = malloc(global_mem_size);
    
    mem_init(global_memory, global_mem_size); //pointer to the global memory, and the size
    print_stats("init");

    unsigned char *ptr_array[4];
    unsigned int sizes[] = {1, 3, 7, 0};

    //test for a zero malloc
    sizes[0] = 0;
    ptr_array[0] = my_malloc(sizes[0]);
    if(ptr_array[0]==NULL){
            printf("checking for size 0 value");
            printf("test case passed!\n");
        }

    sizes[0] = 1;
    for (int i = 0; i<4; i++) {
        ptr_array[i] = my_malloc(sizes[i]);
        printf("size of i is = to %d\n", sizes[i]);
        if(ptr_array[i]==NULL && i>=1){
            printf("checking that the memory is full doesn't break\n");
            printf("value return is null after i is %d\n", i);
            printf("test case passed!\n");
            printf("--------------------\n");
        }
        else{
            printf("i is %d and ptr array is %s\n", i, ptr_array[i]);
        }

    }
    my_free(ptr_array[0]);
    printf("checking that memory can be freed");
    ptr_array[0] = my_malloc(1);
    print_stats((char *)ptr_array[0]);

}
