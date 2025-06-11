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

int test_malloc_free_simple(void) {
    mi_heap_t* heap1 = mi_heap_new_dasics(1);
    mi_heap_t* heap2 = mi_heap_new_dasics(2);
    mi_heap_t* heap3 = mi_heap_new_dasics(3);

    // test alloc 
    int *p11 = (int*)mi_malloc_dasics(sizeof(int)*1024, 1, 1);
    int *p12 = (int*)mi_malloc_dasics(sizeof(int)*1024, 1, 2);

    int *p21 = (int*)mi_zalloc_dasics(sizeof(int)*1024, 2, 1);
    int *p22 = (int*)mi_zalloc_dasics(sizeof(int)*1024, 2, 2);

    int *p31 = (int*)mi_calloc_dasics(1024, sizeof(int), 3, 1);
    int *p32 = (int*)mi_calloc_dasics(1024, sizeof(int), 3, 2);

    for (int i = 0; i < 1024; i++) {
        p11[i] = 0x11 * i;
        p12[i] = 0x12 * i;

        if ((p21[i] != 0) || (p22[i] != 0)) {
            printf("zalloc failed: occur non-zero value at index %d\n", i);
        }
        p21[i] = 0x21 * i;
        p22[i] = 0x22 * i; 

        p31[i] = 0x31 * i;
        p32[i] = 0x32 * i; 

    }

    // new test: test realloc 
    p11 = mi_realloc_dasics(p11, sizeof(int)*512, 1, 1);
    p12 = mi_realloc_dasics(p12, sizeof(int)*2048, 1, 2);


    // new test: check pointer boundry
    // use original experimental function "mi_is_in_heap_region"

    /*
    bool check_result = false; 
    int check_offset = 0;
    int * check_pointer = p11;
    for (int i = 0; i < 4; i++) {
            check_result = mi_is_in_heap_region(check_pointer);
            printf("check pointer: 0x%x, check add offset : %d, check result: %d\n", (unsigned int)check_pointer, check_offset, check_result);
            check_offset += 512;
            check_pointer += 512;
    }

    check_offset = 0;
    check_pointer = p12;
    for (int i = 0; i < 4; i++) {
        check_result = mi_is_in_heap_region(check_pointer);
        printf("check pointer: 0x%x, check add offset : %d, check result: %d\n", (unsigned int)check_pointer, check_offset, check_result);
        check_offset += 512;
        check_pointer += 512;
    }
    */

    mi_free_dasics(p11, 1, 1);
    mi_free_dasics(p12, 1, 2);
    mi_free_dasics(p21, 2, 1);
    mi_free_dasics(p22, 2, 2);
    mi_free_dasics(p31, 3, 1);
    mi_free_dasics(p32, 3, 2);

    // new test: test alloc & free counters 
    int alloc_cnt = 0, free_cnt = 0;
    for (int i = 0; i < 4; i++) {
        alloc_cnt = mi_get_heap_alloc_count_dasics(i);
        free_cnt = mi_get_heap_free_count_dasics(i);
        printf("library %d : malloc times %d, free times %d\n", i, alloc_cnt, free_cnt);
    }


    // new test: test heap release ? 

    printf("dasics simple test finished\n");
    return 0;
}

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

    free(array);
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
    // test_malloc_free_simple();
    // test_malloc_free(2, 2, 1024);

    printf("test library heap isolation\n");

    mi_set_id_dasics(1, LIB_ID_LOC);
    mi_set_id_dasics(1, CLS_ID_LOC);
    int res1 = test_lib_add(8);

    mi_set_id_dasics(2, LIB_ID_LOC);
    mi_set_id_dasics(2, CLS_ID_LOC);
    int res2 = test_lib_mul(8);

    mi_set_id_dasics(0, LIB_ID_LOC);
    mi_set_id_dasics(0, CLS_ID_LOC);
    // add counter test 
    int alloc_cnt = 0, free_cnt = 0;
    for (int i = 0; i < 3; i++) {
        alloc_cnt = mi_get_heap_alloc_count_dasics(i);
        free_cnt = mi_get_heap_free_count_dasics(i);
        printf("library %d : malloc times %d, free times %d\n", i, alloc_cnt, free_cnt);       
    }

    printf("finish testing library heap isolation\n");

    return 0;
}