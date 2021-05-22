#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

#define TEST_FAIL(line) fprintf(stderr, "Fail: at line %d\n", line);

/* Test File For list.c */

/* Helper Functions */

int* int_malloc();
void print_test_status(const char* name, int status);
void int_free(void* pInt);

/* Tests */

int test_empty_list(List* pList);
int test_list_add_0(List* pList);
int test_list_add_1(List* pList);

int main(){
    int failed_tests = 0;
    
    /* List Add Test Cases */
    List* pList0 = List_create();
    failed_tests += test_list_add_0(pList0);
    List_free(pList0, int_free);
    pList0 = List_create();
    failed_tests += test_empty_list(pList0);
    failed_tests += test_list_add_1(pList0);
    List_free(pList0, int_free);
    failed_tests += test_list_add_1(pList0);
    List_free(pList0, int_free);


    /* TODO more test cases */
    //pList0 = List_create();

    printf("failed_tests: %d\n", failed_tests);
    return failed_tests > 0 ? 1 : 0;
}

int* int_malloc(){
    return (int* ) malloc(sizeof(int));
}

void int_free(void* pInt){
    free(pInt);
}

void print_test_status(const char* name, int status){
    if(status){
        printf("%s: Failed\n", name);
    } else {
        printf("%s: Passed\n", name);
    }
}

int test_empty_list(List* pList){
    int status = 0;
    if(List_count(pList) != 0){
        status = 1;
        TEST_FAIL(__LINE__)
    } 
    if(List_curr(pList) != NULL){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    if(List_first(pList)!= NULL){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    if(List_last(pList) != NULL){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    if(List_prev(pList)){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    if(List_next(pList)){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    print_test_status(__func__, status);
    return status;
}

int test_list_add_0(List* pList){
    assert(pList != NULL);
    int status = 0;
    int* x;
    int temp;
    x = int_malloc();
    *x = 1;
    List_add(pList, x);
    if(List_first(pList) == NULL 
            || List_last(pList) == NULL 
            || List_curr(pList) == NULL 
            || List_count(pList) != 1){
        status = 1;
        TEST_FAIL(__LINE__)
    }

    x = int_malloc();
    *x = 2;
    List_add(pList, x);

    x = int_malloc();
    *x = 3;
    List_add(pList, x);
    
    temp = *(int *)List_prev(pList);
    if(*(int *)List_first(pList) != 1 || 
        *(int *)List_last(pList) != 3 ||
        temp != 2
        ){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    List_prev(pList);
    x = int_malloc();
    *x = 4;
    List_add(pList, x);
    
    x = int_malloc();
    *x = 5;
    List_add(pList, x);
    
    x = int_malloc();
    *x = 6;
    List_add(pList, x);

    temp = *(int *)List_curr(pList);    
    if(*(int *)List_first(pList) != 1 || 
        *(int *)List_last(pList) != 3 ||
        temp != 6
        ){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    
    List_first(pList);
    List_prev(pList);
    List_prev(pList);

    x = int_malloc();
    *x = 7;
    List_add(pList, x);
    
    if(*(int *)List_first(pList) != 7 ||
        List_count(pList) != 7){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    
    List_last(pList);
    List_next(pList);
    x = int_malloc();
    *x = 8;
    List_add(pList, x);

    if(*(int *)List_last(pList) != 8 ||
        List_count(pList) != 8){
        status = 1;
        TEST_FAIL(__LINE__)
    }

    print_test_status(__func__, status);
    return status;
}

int test_list_add_1(List* pList){
    int status = 0;
    int* x;
    for(int i=0; i<LIST_MAX_NUM_NODES; ++i){
        x=int_malloc();
        *x=i;
        if(List_add(pList, x) == LIST_FAIL){
            status = 1;
            TEST_FAIL(__LINE__)  
        }
    }
    x=int_malloc();
    *x=LIST_MAX_NUM_NODES+1;
    if(List_add(pList, x) == LIST_SUCCESS){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    int_free(x);
    if(List_count(pList) != LIST_MAX_NUM_NODES){
        status = 1;
        TEST_FAIL(__LINE__)
    }
    print_test_status(__func__, status);
    return status;
}