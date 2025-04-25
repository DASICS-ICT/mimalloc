#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "mimalloc.h"
#include "testhelper.h"
/* test library-heap */

int main (int argc, char** argv) {
    mi_heap_t* heap1 = mi_heap_new_dasics(1);
    mi_heap_t* heap2 = mi_heap_new_dasics(2);

    // test alloc 
    int *p11 = (int*)mi_malloc_dasics(sizeof(int)*1024, 1, 1);
    int *p12 = (int*)mi_malloc_dasics(sizeof(int)*1024, 1, 2);

    int *p21 = (int*)mi_malloc_dasics(sizeof(int)*1024, 2, 1);
    int *p22 = (int*)mi_malloc_dasics(sizeof(int)*1024, 2, 2);

    for (int i = 0; i < 1024; i++) {
        p11[i] = 0x11 * i;
        p12[i] = 0x12 * i;
        p21[i] = 0x21 * i;
        p22[i] = 0x22 * i; 
    }

    mi_free_dasics(p11, 1, 1);
    mi_free_dasics(p12, 1, 2);
    mi_free_dasics(p21, 2, 1);
    mi_free_dasics(p22, 2, 2);

    printf("dasics simple test finished\n");
    return 0;
}
