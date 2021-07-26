#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "set.h"

unsigned long int_hash(void* pItem);
bool int_comparison(void* pItem0, void* pItem1);

int test_basic_function();
int test_collision();
int test_removal_after_collison();
int test_wrap();
int test_full();

int main(){
    int failed = 0;
    failed += test_basic_function();
    failed += test_collision(); 
    failed += test_removal_after_collison();
    failed += test_wrap();
    failed += test_full();

    printf("Number Failed Tests:%d\n", failed);
    return EXIT_SUCCESS;
}

unsigned long int_hash(void* pItem){
    return (unsigned long)*(int *)pItem;
}

bool int_comparison(void* pItem0, void* pItem1){
    return *(int*)pItem0 == *(int*)pItem1;
}

int test_basic_function(){
    int array[4];
    array[0] = 3;
    array[1] = 101;
    array[2] = 102;
    array[3] = 103 + SET_SIZE;
    
    Set* pSet = Set_create(int_hash, int_comparison);
    if(Set_contains(pSet, &array[0])){
        printf("Test Failed %u\n", __LINE__);
        Set_free(pSet);
        return 1;
    }
    Set_insert(pSet, &array[0]);
    if(!Set_contains(pSet, &array[0])){
        printf("Test Failed %u\n", __LINE__);
        Set_free(pSet);
        return 1;
    }
    Set_insert(pSet, &array[1]);
    Set_insert(pSet, &array[2]);
    Set_insert(pSet, &array[3]);
    if(!Set_contains(pSet, &array[1]) || !Set_contains(pSet, &array[2]) || !Set_contains(pSet, &array[3])){
        printf("Test Failed %u\n", __LINE__);
        Set_free(pSet);
        return 1;
    }
    Set_remove(pSet, &array[2]);
    if(Set_contains(pSet, &array[2])){
        printf("Test Failed %u\n", __LINE__);
        Set_free(pSet);
        return 1;
    }

    if(!Set_contains(pSet, &array[1]) || !Set_contains(pSet, &array[0]) || !Set_contains(pSet, &array[3])){
        printf("Test Failed %u\n", __LINE__);
        Set_free(pSet);
        return 1;
    }

    Set_free(pSet);
    printf("%s Passed\n", __func__);
    return 0;
}

int test_collision(){
    int array[5];
    array[0] = 6;
    array[1] = SET_SIZE + 6;
    array[2] = 2*SET_SIZE + 6;
    array[3] = SET_SIZE + 7;
    array[4] = SET_SIZE + 8;
    Set* pSet = Set_create(int_hash, int_comparison);
    Set_insert(pSet, &array[0]);
    Set_insert(pSet, &array[1]);
    Set_insert(pSet, &array[2]);
    Set_insert(pSet, &array[3]);
    Set_insert(pSet, &array[4]);
    if(!Set_contains(pSet, &array[0]) 
            || !Set_contains(pSet, &array[1])
            || !Set_contains(pSet, &array[2])
            || !Set_contains(pSet, &array[3])
            || !Set_contains(pSet, &array[4])){
        Set_free(pSet);
        printf("Test Failed %u\n", __LINE__);    
        return 1;
    }
    Set_free(pSet);
    printf("%s Passed\n", __func__);
    return 0;
}

int test_removal_after_collison(){
    
    int array[10];
    array[0] = 6;
    array[1] = SET_SIZE + 6;
    array[2] = 2*SET_SIZE + 6;
    array[3] = SET_SIZE + 7;
    array[4] = SET_SIZE + 8;
    array[5] = 8;
    array[6] = 8 + SET_SIZE;
    array[7] = 5;
    array[8] = 5 + SET_SIZE;
    array[9] = 10;
    Set* pSet = Set_create(int_hash, int_comparison);
    for(int i=0; i<sizeof(array)/sizeof(int); ++i){
        Set_insert(pSet, &array[i]);
    }

    if(!Set_contains(pSet, &array[0]) 
        || !Set_contains(pSet, &array[1])
        || !Set_contains(pSet, &array[2])
        || !Set_contains(pSet, &array[3])
        || !Set_contains(pSet, &array[4])
        || !Set_contains(pSet, &array[5])
        || !Set_contains(pSet, &array[6])
        || !Set_contains(pSet, &array[7])
        || !Set_contains(pSet, &array[8])
        || !Set_contains(pSet, &array[9])){
        printf("Test Failed %u\n", __LINE__);    
        return 1;
        Set_free(pSet);
    }
    
    Set_remove(pSet, &array[2]);
    
    if(Set_contains(pSet, &array[2])){
        Set_free(pSet);
        printf("Test Failed %u\n", __LINE__);    
        return 1;
    }
    
    if(!Set_contains(pSet, &array[0]) 
            || !Set_contains(pSet, &array[1])
            || !Set_contains(pSet, &array[3])
            || !Set_contains(pSet, &array[4])
            || !Set_contains(pSet, &array[5])
            || !Set_contains(pSet, &array[6])
            || !Set_contains(pSet, &array[7])
            || !Set_contains(pSet, &array[8])
            || !Set_contains(pSet, &array[9])){
        printf("Test Failed %u\n", __LINE__);    
        return 1;
        Set_free(pSet);
    }
    Set_remove(pSet, &array[0]);
    if(!Set_contains(pSet, &array[1])
            || !Set_contains(pSet, &array[3])
            || !Set_contains(pSet, &array[4])
            || !Set_contains(pSet, &array[5])
            || !Set_contains(pSet, &array[6])
            || !Set_contains(pSet, &array[7])
            || !Set_contains(pSet, &array[8])
            || !Set_contains(pSet, &array[9])){
        printf("Test Failed %u\n", __LINE__);    
        return 1;
        Set_free(pSet);
    }
    Set_remove(pSet, &array[7]);
    if(!Set_contains(pSet, &array[1])
            || !Set_contains(pSet, &array[3])
            || !Set_contains(pSet, &array[4])
            || !Set_contains(pSet, &array[5])
            || !Set_contains(pSet, &array[6])
            || !Set_contains(pSet, &array[8])
            || !Set_contains(pSet, &array[9])){
        printf("Test Failed %u\n", __LINE__);    
        return 1;
        Set_free(pSet);
    }
    Set_free(pSet);
    printf("%s Passed\n", __func__);
    return 0;
}

int test_wrap(){
    int array[6];
    array[0] = 0;
    array[1] = SET_SIZE -1;
    array[2] = SET_SIZE -2;
    array[3] = 1;
    array[4] = SET_SIZE + 1;
    array[5] = 2*SET_SIZE -2;
    Set* pSet = Set_create(int_hash, int_comparison);
    for(int i=0; i<sizeof(array)/sizeof(int); ++i){
        Set_insert(pSet, &array[i]);
    }
    
    if(!Set_contains(pSet, &array[0]) 
            || !Set_contains(pSet, &array[1])
            || !Set_contains(pSet, &array[2])
            || !Set_contains(pSet, &array[3])
            || !Set_contains(pSet, &array[4])
            || !Set_contains(pSet, &array[5])
            ){
        Set_free(pSet);
        printf("Test Failed %u\n", __LINE__);    
        return 1;
    }

    Set_remove(pSet, &array[2]);
    if(!Set_contains(pSet, &array[0])
            || !Set_contains(pSet, &array[1])
            || !Set_contains(pSet, &array[3])
            || !Set_contains(pSet, &array[4])
            || !Set_contains(pSet, &array[5])
            ){
        Set_free(pSet);
        printf("Test Failed %u\n", __LINE__);
        return 1;
    }


    printf("%s Passed\n", __func__);
    return 0;
}

int test_full(){
    // TODO this is not implemented in the struct
    printf("TODO %s: this is not implemented nor tested\n", __func__);
    printf("%s Passed\n", __func__);
    return 0;
}
