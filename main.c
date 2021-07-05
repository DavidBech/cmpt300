#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

#define TEST_FAIL(line) fprintf(stderr, "Fail: at line %d\n", line)
#define FAILURE status = 1; TEST_FAIL(__LINE__);
/* Test File For list.c */

/* Helper Functions */
int* int_malloc();
void print_test_status(const char* name, int status);
void int_free(void* pInt);
int run_test(int testfunc(List**), int numLists);
bool compareInt(void*, void*);

/* Tests */
int test_empty_list(List*);
int test_head(List**);
int test_list_add(List**);
int test_list_insert(List**);
int test_list_append(List**);
int test_list_prepend(List**);
int test_traversal(List**);
int test_compare(List**);
int test_remove(List**);
int test_concat(List**);

int main(){
    printf("List Size: %ld\n", sizeof(List));
    int failed_tests = 0;
    failed_tests += run_test(test_head, 10);
    failed_tests += run_test(test_head, 10);
    failed_tests += run_test(test_remove, 1);
    failed_tests += run_test(test_concat, 2);
    failed_tests += run_test(test_list_add, 1);
    failed_tests += run_test(test_list_insert, 2);
    failed_tests += run_test(test_list_append, 1);
    failed_tests += run_test(test_list_prepend, 1);
    failed_tests += run_test(test_traversal, 1);
    failed_tests += run_test(test_compare, 1);
    failed_tests += run_test(test_head, 10);

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

int run_test(int test(List**), int numLists){
    List* pList_a[numLists];
    int status = 0;
    for(int i=0; i<numLists; ++i){
        pList_a[i] = List_create();
        status += test_empty_list(pList_a[i]);
    }
    status += test(pList_a);
    for(int i=0; i<numLists; ++i){
        List_free(pList_a[i], int_free);
    }
    return status;
}

bool compareInt(void* listIn, void* compareIn){
    return *(int*)listIn == *(int*)compareIn;
}

/* Start of Tests */

int test_empty_list(List* pList){
    int status = 0;
    assert(pList != NULL);
    if(List_count(pList) != 0){
        FAILURE
    } 
    if(List_curr(pList) != NULL){
        FAILURE
    }
    if(List_first(pList)!= NULL){
        FAILURE
    }
    if(List_last(pList) != NULL){
        FAILURE
    }
    if(List_prev(pList)){
        FAILURE
    }
    if(List_next(pList)){
        FAILURE
    }
    if(status){
        printf("%s: Failed\n", __func__);
    }
    return status;
}

int test_list_add(List** ppList){
    List* pList = *ppList;
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
        FAILURE
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
        FAILURE
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
        FAILURE
    }
    
    List_first(pList);
    List_prev(pList);
    List_prev(pList);

    x = int_malloc();
    *x = 7;
    List_add(pList, x);
    
    if(*(int *)List_first(pList) != 7 ||
        List_count(pList) != 7){
        FAILURE
    }
    
    List_last(pList);
    List_next(pList);
    x = int_malloc();
    *x = 8;
    List_add(pList, x);

    if(*(int *)List_last(pList) != 8 ||
        List_count(pList) != 8){
        FAILURE
    }
    List_free(pList, int_free);
    pList = List_create();
    for(int i=0; i<LIST_MAX_NUM_NODES; ++i){
        x=int_malloc();
        *x=i;
        if(List_add(pList, x) == LIST_FAIL){
            FAILURE
        }
    }
    x=int_malloc();
    *x=LIST_MAX_NUM_NODES+1;
    if(List_add(pList, x) == LIST_SUCCESS){
        FAILURE
    }
    int_free(x);
    if(List_count(pList) != LIST_MAX_NUM_NODES){
        FAILURE
    }
    print_test_status(__func__, status);
    return status;

    print_test_status(__func__, status);
    return status;
}

int test_list_insert(List** ppList){
    List* pList0 = ppList[0];
    List* pList1 = ppList[1];
    int status = 0;
    int* x;
    int expected[] ={42, 14, 17, 18, 20};
    for(int i=0; i<LIST_MAX_NUM_NODES/2; ++i){
        x = int_malloc();
        *x = i;    
        if(List_insert(pList0, x)){
            FAILURE
        }
        x = int_malloc();
        *x = LIST_MAX_NUM_NODES/2 - i;    
        if(List_insert(pList1, x)){
            FAILURE
        }
    }
    if(*(int *)List_curr(pList0) != LIST_MAX_NUM_NODES/2 -1){
        FAILURE
    }
    
    if(*(int *)List_curr(pList1) != 1){
        FAILURE
    }

    x = int_malloc();
    *x = 0xDEADBEEF;
    if(List_insert(pList0, x) == LIST_SUCCESS){
        FAILURE
    }
    if(List_insert(pList1, x) == LIST_SUCCESS){
        FAILURE
    }
    int_free(x);

    for(int i=1; i<LIST_MAX_NUM_NODES/2 -1; ++i){
        if(*(int *)List_next(pList0) != LIST_MAX_NUM_NODES/2 - (i+1)){
            FAILURE
        }
        if(*(int *)List_next(pList1) != (i+1)){
            FAILURE
        }
    }
    
    List_free(pList1, int_free);
    List_free(pList0, int_free);
    pList0 = List_create();
    pList1 = List_create();
    x = int_malloc();
    *x = 14;
    List_append(pList0, x);
    x = int_malloc();
    *x = 18;
    List_append(pList0, x);
    
    List_last(pList0);
    x = int_malloc();
    *x = 17;
    if(List_insert(pList0, x) == LIST_FAIL){
        FAILURE
    }
    List_last(pList0);
    List_next(pList0);
    x = int_malloc();
    *x = 20;
    if(List_insert(pList0,x) == LIST_FAIL){
        FAILURE
    }
    if(*(int *)List_curr(pList0) != 20){
        FAILURE
    }
    if(*(int *)List_last(pList0) != 20){
        FAILURE
    }
    List_first(pList0);
    List_prev(pList0);
    x = int_malloc();
    *x = 42;
    if(List_insert(pList0,x) == LIST_FAIL){
        FAILURE
    }

    if(*(int *)List_curr(pList0) != 42){
        FAILURE      
    }
    if(*(int *)List_first(pList0) != 42){
        FAILURE
    }

    x = int_malloc();
    *x = 20;
    if(List_insert(pList0, x) == LIST_FAIL){
        FAILURE
    } 
    if(*(int *)List_curr(pList0) != 20){
        FAILURE       
    }
    if(*(int *)List_first(pList0) != 20){
        FAILURE
    }
    int temp;
    int cntr = 0;
    while(List_next(pList0)){
        temp = *(int*)List_curr(pList0);
        if(temp != expected[cntr]){
            FAILURE
        }
        ++cntr;
    }
    if(List_count(pList0) != 6){
        FAILURE
    }
    print_test_status(__func__, status);
    return status;
}

int test_list_append(List** ppList){
    int status = 0;
    List* pList = *ppList;
    int* x;
    int expected[] = {17, 20, 78, 42, 86, 90, 93};
    for(int i=0; i<LIST_MAX_NUM_NODES; ++i){
        x = int_malloc();
        *x = i;
        if(List_append(pList, x) == LIST_FAIL){
            FAILURE
        }
    }
    x = int_malloc();
    *x = 0xdeadbeef;
    if(List_append(pList, x) == LIST_SUCCESS){
        FAILURE
    }
    
    List_free(pList, int_free);
    List_create(pList);
    *x = 17;
    List_add(pList, x);

    x = int_malloc();
    *x = 20;
    List_add(pList, x);

    x = int_malloc();
    *x = 78;
    List_add(pList, x);

    List_first(pList);
    List_prev(pList);

    x = int_malloc();
    *x = 42;
    if(List_append(pList, x) == LIST_FAIL){
        FAILURE
    }
    
    List_next(pList);
    if(List_curr(pList) != NULL){
        FAILURE
    }
    x = int_malloc();
    *x = 86;
    if(List_append(pList, x) == LIST_FAIL){
        FAILURE
    }
    
    List_last(pList);
    x = int_malloc();
    *x = 90;
    if(List_append(pList, x) == LIST_FAIL){
        FAILURE
    }
    List_first(pList);
    List_next(pList);
    x = int_malloc();
    *x = 93;
    if(List_append(pList, x) == LIST_FAIL){
        FAILURE
    }
    List_first(pList);
    int cnt = 0;
    if(List_curr(pList) == NULL){
        FAILURE
    }
    do{
        if(*(int*)List_curr(pList) != expected[cnt]){
            FAILURE
        }
        ++cnt;
    } while(List_next(pList));

    print_test_status(__func__, status);
    return status;
}

int test_list_prepend(List** ppList){
    int status = 0;
    List* pList = *ppList;
    int* x;

    for(int i=0; i<LIST_MAX_NUM_NODES; ++i){
        x = int_malloc();
        *x = i;
        List_prepend(pList, x);
    }
    x = int_malloc();
    *x = 0xdeadbeef;
    if(List_prepend(pList, x) == LIST_SUCCESS){
        FAILURE
    }
    int_free(x);
    List_first(pList);
    for(int i=0; i<LIST_MAX_NUM_NODES; ++i){
        if(*(int*)List_curr(pList)!= LIST_MAX_NUM_NODES - (i+1)){
            FAILURE
        }
        List_next(pList);
    }
    List_free(pList, int_free);
    pList = List_create();
    x = int_malloc();
    *x = 1;
    List_add(pList, x);
    x = int_malloc();
    *x = 2;
    List_add(pList, x);
    x = int_malloc();
    *x = 3;
    List_add(pList, x);
    List_prev(pList);
    x = int_malloc();
    *x = 5;
    if(List_prepend(pList,x) == LIST_FAIL){
        FAILURE
    }
    List_last(pList);
    List_next(pList);
    x = int_malloc();
    *x = 6;
    if(List_prepend(pList,x) == LIST_FAIL){
        FAILURE
    }
    
    List_next(pList);
    x = int_malloc();
    *x = 10;
    if(List_prepend(pList,x) == LIST_FAIL){
        FAILURE
    }

    int expected[] = {10,6,5,1,2,3};
    List_first(pList);
    int cnt = 0;
    if(List_curr(pList) == NULL){
        FAILURE
    }
    do{
        if(*(int*)List_curr(pList) != expected[cnt]){
            FAILURE
        }
        ++cnt;
    } while(List_next(pList));
    
    print_test_status(__func__,status);
    return status;
}

int test_traversal(List** ppList){
    int status = 0;
    List* pList = *ppList;
    int* x;
    // empty lists tests
    if(List_first(pList)){
        FAILURE
    }

    if(List_last(pList)){
        FAILURE
    }

    if(List_next(pList)){
        FAILURE
    }
    
    if(List_last(pList)){
        FAILURE
    }

    if(List_curr(pList)){
        FAILURE
    }

    for(int i=0; i<10; ++i){
        x = int_malloc();
        *x = i;
        List_append(pList, x);
    }
    /*first last next prev cur*/
    /* first */
    if(*(int*)List_first(pList) != 0){
        FAILURE
    }
    List_prev(pList);
    if(*(int*)List_first(pList) != 0){
        FAILURE
    }
    List_last(pList);
    List_next(pList);
    if(*(int*)List_first(pList) != 0){
        FAILURE
    }

    if(*(int*)List_last(pList) != 9){
        FAILURE
    }
    List_next(pList);

    if(*(int*)List_last(pList) != 9){
        FAILURE
    }
    List_first(pList);
    List_prev(pList);
    if(*(int*)List_last(pList) != 9){
        FAILURE
    }
    List_prev(pList);
    List_prev(pList);
    List_prev(pList);

    if(*(int*)List_last(pList) != 9){
        FAILURE
    }
    print_test_status(__func__, status);
    return status;
}

int test_compare(List** ppList){
    List* pList = *ppList;
    int status = 0;
    int *x;
    int arrayInt[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,3};
    void* item;
    item = List_search(pList, compareInt, (void*)(&arrayInt[3]));
    if(item){
        FAILURE
    }
    if(List_curr(pList)){
        FAILURE
    }
    for(int i=0; i<16; ++i){
        x = malloc(sizeof(int));
        *x = arrayInt[i];
        List_append(pList, x);
    }
    List_first(pList);
    
    item = List_search(pList, compareInt, (void*)&arrayInt[15]);
    if(item == NULL){
        FAILURE
    }
    if(item != NULL && *(int*)item != 3){
        FAILURE
    }
    if(List_curr(pList) == NULL){
        FAILURE
    }
    if(*(int*)List_next(pList) != 4){
        FAILURE   
    }

    item = List_search(pList, compareInt, (void*)&arrayInt[15]);
    if(item == NULL){
        FAILURE
    }
    if(item != NULL && *(int*)item != 3){
        FAILURE
    }
    if(List_curr(pList) == NULL){
        FAILURE
    }

    int temp = 18247;
    List_first(pList);
    item = List_search(pList, compareInt, (void*)&temp);
    if(item != NULL){
        FAILURE
    }
    if(List_curr(pList) != NULL){
        FAILURE
    }

    List_first(pList);
    temp = 1;
    item = List_search(pList, compareInt, (void*)&temp);
    if(!item){
        FAILURE
    }
    if(!List_curr(pList)){
        FAILURE
    }

    List_next(pList);
    temp = 1;
    item = List_search(pList, compareInt, (void*)&temp);
    if(item){
        FAILURE
    }
    if(List_curr(pList)){
        FAILURE
    }

    List_first(pList);
    List_prev(pList);
    temp = 5;
    item = List_search(pList, compareInt, (void*)&temp);
    if(!item || *(int*) item != 5){
        FAILURE
    }
    if(!List_curr(pList)){
        FAILURE
    }
    if(List_count(pList) != 16){
        FAILURE
    }

    List_last(pList);
    List_next(pList);
    temp = 5;
    item = List_search(pList, compareInt, (void*)&temp);
    if(item){
        FAILURE
    }
    if(List_curr(pList)){
        FAILURE
    }
    if(List_count(pList) != 16){
        FAILURE
    }


    print_test_status(__func__, status);
    return status;
}

int test_remove(List **ppList){
    int status = 0;
    List* pList = *ppList;
    int* x;
    
    if(List_remove(pList)){
        FAILURE
    }
    if(List_count(pList) != 0){
        FAILURE
    }

    for(int i=0; i<10; ++i){
        x = int_malloc();
        *x = i;
        List_append(pList, x);
    }

    List_first(pList);
    List_next(pList);
    List_next(pList);
    List_next(pList);
    x = (int *)List_remove(pList);
    if(*x != 3){
        FAILURE
    }
    int_free(x);
    if(*(int*)List_curr(pList) != 4){
        FAILURE
    }
    if(List_count(pList) != 9){
        FAILURE
    }
    List_first(pList);
    List_prev(pList);
    if(List_remove(pList) || List_count(pList) != 9){
        FAILURE
    }
    
    List_last(pList);
    List_next(pList);
    if(List_remove(pList) || List_count(pList) != 9){
        FAILURE
    }
    List_first(pList);
    x = (int *)List_remove(pList);
    if(*x != 0 || List_count(pList) != 8){
        FAILURE
    }
    if(*(int*)List_curr(pList) != 1){
        FAILURE
    }
    List_last(pList);
    int_free(x);
    x = (int*)List_remove(pList);
    
    if(*x != 9 || List_count(pList) != 7){
        FAILURE
    }
    int_free(x);
    if(List_curr(pList)){
        FAILURE
    }

    print_test_status(__func__, status);
    return status;
}

int test_head(List **ppList){
    int status = 0;
    int* x;
    List* listHeads[LIST_MAX_NUM_HEADS];
    for(int i=0; i<LIST_MAX_NUM_HEADS; ++i){
        listHeads[i] = *(ppList+i);
    }
    for(int i=0; i<LIST_MAX_NUM_HEADS; ++i){
        for(int j=0; j<5; ++j){
            x = int_malloc();
            *x = i+j;
            if(List_add(listHeads[i], x) == LIST_FAIL){
                FAILURE
            }
        }
    }
    
    for(int i=0; i<LIST_MAX_NUM_HEADS; ++i){
        if(List_count(listHeads[i]) != 5){
            FAILURE
        }
    }
    

    for(int i=LIST_MAX_NUM_HEADS-1; i>=0; --i){
        for(int j=0; j<5; ++j){
            x = int_malloc();
            *x = i+j;
            if(List_add(listHeads[i], x) == LIST_FAIL){
                FAILURE
            }
        }
    }

    for(int i=0; i<LIST_MAX_NUM_HEADS; ++i){
        if(List_count(listHeads[i]) != 10){
            FAILURE
        }
    }
    x = int_malloc();
    *x = 420;
    for(int i=0; i<LIST_MAX_NUM_HEADS; ++i){
        if(List_add(listHeads[i], x) == LIST_SUCCESS){
            FAILURE
        }
     
    }
    int_free(x);

    print_test_status(__func__, status);
    return status;

}

int test_concat(List **ppList){
    int status = 0;
    List* pList0 = *ppList;
    List* pList1 = *(ppList + 1);
    int* x;

    // both empty
    List_concat(pList0, pList1);
    if(pList1->inUse){
        FAILURE
    }
    if(!pList0->inUse){
        FAILURE
    }
    pList1 = List_create();

    // first empty
    for(int i=0; i<5; ++i){
        x = int_malloc();
        *x = i;
        List_add(pList1, x);
    }

    List_concat(pList0, pList1);
    if(pList1->inUse){
        FAILURE
    }
    if(List_count(pList0) != 5){
        FAILURE
    }
    if(List_next(pList0) != List_first(pList0)){
        FAILURE
    }

    // Second empty
    pList1 = List_create();
    List_first(pList0);
    List_next(pList0);
    List_concat(pList0, pList1);

    if(pList1->inUse){
        FAILURE
    }
    if(List_count(pList0) != 5){
        FAILURE
    }
    if(List_prev(pList0) != List_first(pList0)){
        FAILURE
    }

    // regular case
    pList1 = List_create();
    for(int i=0; i<5; ++i){
        x = int_malloc();
        *x = i + 5;
        List_add(pList1, x);
    }
    List_last(pList0);
    List_prev(pList0);
    List_concat(pList0, pList1);
    if(pList1->inUse){
        FAILURE
    }
    if(List_count(pList0) != 10){
        FAILURE
    }
    if(*(int*)List_curr(pList0)!= 3
        || *(int*)List_next(pList0)!= 4
        || *(int*)List_next(pList0)!= 5
        || *(int*)List_next(pList0)!= 6
        || *(int*)List_next(pList0)!= 7
        || *(int*)List_next(pList0)!= 8
        || *(int*)List_next(pList0)!= 9){
        FAILURE
    }
    
    List_next(pList0); // now oob end

    // cur oob end
    pList1 = List_create();
    for(int i=0; i<5; ++i){
        x = int_malloc();
        *x = i + 10;
        List_add(pList1, x);
    }
    List_concat(pList0, pList1);
    if(List_prev(pList0) != List_last(pList0)){
        FAILURE
    }
    if(List_count(pList0) != 15){
        FAILURE
    }
    
    
    // cur oob start
    List_first(pList0);
    List_prev(pList0);
    pList1 = List_create();
    for(int i=0; i<5; ++i){
        x = int_malloc();
        *x = i + 15;
        List_add(pList1, x);
    }
    List_concat(pList0, pList1);
    if(List_next(pList0) != List_first(pList0)){
        FAILURE
    }
    if(List_count(pList0) != 20){
        FAILURE
    }
    pList1 = List_create();
    print_test_status(__func__, status);
    return status;
}
