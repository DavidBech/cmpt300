#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

#define TEST_FAIL(line) fprintf(stderr, "Fail: at line %d\n", line);

/* Test File For list.c */
int* int_malloc();
int test_list_add(List* pList);


int main(){
    int failed_tests = 0;
    
    /* List Add Test Case */
    List* pList0 = List_create();
    test_list_add(pList0);

    printf("failed_tests: %d\n", failed_tests);
    return failed_tests > 0 ? 1 : 0;
}

int* int_malloc(){
    return (int* ) malloc(sizeof(int));
}

int test_list_add(List* pList){
    assert(pList != NULL);
    int status = 0;
    int* x;

    x = int_malloc();
    *x = 10;
    List_add(pList, x);
    if(List_first(pList) == NULL 
            || List_last(pList) == NULL 
            || List_curr(pList) == NULL 
            || List_count(pList) != 1){
        status = 1;
        TEST_FAIL(__LINE__)
    }

    x = int_malloc();
    *x = 11;
    
    if(status){
        printf("%s: Failed\n", __func__);
    } else {
        printf("%s: Passed\n", __func__);
    }
    return status;
}
