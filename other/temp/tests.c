#include "header_file.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
    unsigned int global_mem_size = 20;
    unsigned char *global_memory = malloc(global_mem_size);
    
    mem_init(global_memory, global_mem_size); //pointer to the global memory, and the size
    print_stats("init");

    unsigned char *ptr_array[4];
    unsigned int sizes[] = {1, 3, 7, 0};

    //test for a zero malloc
    sizes[0] = 0;
    ptr_array[0] = my_malloc(sizes[0]);
    if(ptr_array[0]==NULL){
            printf("test case passed!\n");
        }
    else printf("after the zero malloc\n");
    sizes[0] = 1;
    for (int i = 0; i<4; i++) {
        ptr_array[i] = my_malloc(sizes[i]);
        printf("size of i is = to %d\n", sizes[i]);
        if(ptr_array[i]==NULL){
            printf("value return is null after i is %d\n", i);
            printf("test case passed!\n");
            printf("--------------------\n");
        }
        else{
            printf("i is %d and ptr array is %s\n", i, ptr_array[i]);
        }

    }
    printf("after the for loop\n");

}