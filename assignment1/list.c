#include <stdlib.h>

#include "list.h"

static List listArray[LIST_MAX_NUM_HEADS];
// TODO
//static Node nodeArray[LIST_MAX_NUM_NODES];

static short available_heads = 0x3FF;

static short get_next_head(){
    short temp = available_heads;
    for(int i=0; i<LIST_MAX_NUM_HEADS; ++i){
        if(available_heads & 1){
            available_heads ^= 1;
            return i;
        }
        temp >>= 1;
    }
    return -1;
}

List* List_create(){
    if (available_heads == 0){
        return NULL;
    }
 
    return *(&listArray) + get_next_head();
}

int List_count(List* pList){
    if(pList == NULL){
        exit(1);
    }
    return pList->size;
}

void* List_first(List* pList){
    // TODO
    return NULL;
}

void* List_last(List* pList){
    // TODO
    return NULL;
}

void* List_next(List* pList){
    // TODO
    return NULL;
}

void* List_prev(List* pList){
    // TODO
    return NULL;
}

void* List_curr(List* pList){
    // TODO
    return NULL;
}

int List_add(List* pList, void* pItem){
    // TODO
    return LIST_SUCCESS;
}

int List_insert(List* pList, void* pItem){
    // TODO
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem){
    // TODO
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem){
    // TODO
    return LIST_SUCCESS;
}

void* List_remove(List* pList){
    // TODO
    return NULL;
}

void List_concat(List* pList1, List* pList2){
    // TODO
}

void List_free(List* pList, FREE_FN pItemFreeFn){
    // TODO
}

void* List_trim(List* pList){
    // TODO
    return NULL;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    // TODO
    return NULL;
}


