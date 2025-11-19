#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "mimalloc.h"
#include "testhelper.h"

#define MAX_TEST_SHIFT 4
#define MAX_TEST_LIBRARY (0x1 << MAX_TEST_SHIFT)
#define MAX_TEST_CLOSURE (0x1 << MAX_TEST_SHIFT)

/* test library-heap */
/* assert input library and closure is less than MAX_TEST_LIBRARAY and MAX_TEST_CLOSURE */
int test_malloc_free(int max_library, int max_closure, int alloc_size) {
    /* malloc */
    int* ptr_arr[MAX_TEST_LIBRARY][MAX_TEST_CLOSURE]; 
    int* cur_ptr = NULL;
    for (int i = 0; i < max_library; i++) {
        for (int j = 0; j < max_closure; j++) {
            printf("try alloc memory space for library id %d and closure id %d\n", i, j);
            mi_set_id_dasics(i, LIB_ID_LOC);
            mi_set_id_dasics(j, CLS_ID_LOC);
            cur_ptr = (int*) mi_malloc(sizeof(int)*alloc_size);
            ptr_arr[i][j] = cur_ptr;

            for (int k = 0; k < alloc_size; k++) {
                cur_ptr[k] = (( i << MAX_TEST_SHIFT) + j) * k;
            } // for k
        } // for j
    } // for i

    /* free */
    for (int i = 0; i < max_library; i++) {             
        for (int j = 0; j < max_closure; j++) {
            printf("try free memory space of library id %d and closure id %d\n", i, j);
            mi_set_id_dasics(i, LIB_ID_LOC);
            mi_set_id_dasics(j, CLS_ID_LOC);
            mi_free(ptr_arr[i][j]);
        } // for j
    } // for i

    // add counter test 
    int alloc_cnt = 0, free_cnt = 0;
    for (int i = 0; i < max_library; i++) {
        alloc_cnt = mi_get_heap_alloc_count_dasics(i);
        free_cnt = mi_get_heap_free_count_dasics(i);
        printf("library %d : malloc times %d, free times %d\n", i, alloc_cnt, free_cnt);       
    }

    printf("dasics malloc free test finished\n");
    return 0;
}

/* fake library functions? */
int test_lib_add(int steps) {
    int * array = malloc(sizeof(int)*steps);
    array[0] = 0;
    for (int i = 1; i < steps; i++) {
        array[i] = array[i-1] + i;
    }

    int res = array[steps-1];
    printf("final result: %d\n", res);

    // TODO： add bound test 
    bool is_used;
    is_used = mi_ptr_check_bound_dasics(array);

    printf("check array ptr is used:%s\n", is_used ? "true" : "false");
    is_used = mi_ptr_check_bound_dasics(array+ (steps>>1));
    printf("check array mid is used:%s\n", is_used ? "true" : "false");
    is_used = mi_ptr_check_bound_dasics(array+ (steps<<1));
    printf("check array end is used:%s\n", is_used ? "true" : "false");   

    free(array);

    is_used = mi_ptr_check_bound_dasics(array);
    printf("check array ptr is used:%s\n", is_used ? "true" : "false");
    is_used = mi_ptr_check_bound_dasics(array+ (steps>>1));
    printf("check array mid is used:%s\n", is_used ? "true" : "false");
    is_used = mi_ptr_check_bound_dasics(array+ (steps<<1));
    printf("check array end is used:%s\n", is_used ? "true" : "false");       

    return res;
}

int test_lib_mul(int steps) {
    int * array = malloc(sizeof(int)*steps);
    array[0] = 1;
    for (int i = 1; i < steps; i++) {
        array[i] = array[i-1] * i;
    }

    int res = array[steps-1];
    printf("final result: %d\n", res);

    free(array);
    return res;
}


int main (int argc, char** argv) {
    // test_malloc_free(2, 2, 1024);

    mi_set_id_dasics(0, LIB_ID_LOC);
    mi_set_id_dasics(0, CLS_ID_LOC);
    //mi_count_id_dasics(&main);
    printf("test library heap isolation\n");

    mi_set_id_dasics(1, LIB_ID_LOC);
    mi_set_id_dasics(1, CLS_ID_LOC);
    //mi_count_id_dasics(&test_lib_add);
    int res1 = test_lib_add(8);

    mi_set_id_dasics(1, LIB_ID_LOC);
    mi_set_id_dasics(2, CLS_ID_LOC);
    //mi_count_id_dasics(&test_lib_mul);
    int res2 = test_lib_mul(8);

    mi_set_id_dasics(0, LIB_ID_LOC);
    mi_set_id_dasics(0, CLS_ID_LOC);
    //mi_count_id_dasics(&main);
    // add counter test 
    int alloc_cnt = 0, free_cnt = 0;
    for (int i = 0; i < 4; i++) {
        alloc_cnt = mi_get_heap_alloc_count_dasics(i);
        free_cnt = mi_get_heap_free_count_dasics(i);
        printf("library %d : malloc times %d, free times %d\n", i, alloc_cnt, free_cnt);       
    }

    printf("finish testing library heap isolation\n");

    return 0;
}