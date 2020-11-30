#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "rcmalloc.h"


int main(){
    // printf("%lu\n", sizeof(node_t));

    void* ptr[10];
    rcinit(1024);
    // rcdump();

    clock_t start = clock();

    for (int i = 0; i < 7; i++){
        ptr[i] = rcmalloc(128, M_BESTFIT);
        // printf("ptr[%d] = %p\n", i, ptr[i]);
    }

    rcfree(ptr[4]);
    rcfree(ptr[1]);
    rcfree(ptr[2]);
    rcfree(ptr[6]);

    ptr[0] = rcmalloc(128, M_BESTFIT);
    rcfree(ptr[2]);
    rcdump();

    clock_t diff = clock() - start;
    printf("CLOCKS_PER_SEC = %d, clock_diff = %lu\n", CLOCKS_PER_SEC, diff);
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("time cost: %d ms", msec);

    return 0;
}
