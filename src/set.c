#include <stdlib.h>
#include <assert.h>
#include <string.h> //memset

#include "set.h"

// This function fixes the linear probing errors after setting
//  an entry to NULL
static void fix_removed(Set* pSet, int removed_index);

Set* Set_create(HASH_FUNCTION hash_func, COMPARE_FUNCTION comp_func){
    assert(hash_func != NULL);
    Set* pSet = malloc(sizeof(Set));
    pSet->hash_func = hash_func;
    pSet->comp_func = comp_func;
    pSet->item_list = malloc(sizeof(void*)*SET_SIZE);
    memset(&pSet->item_list, '\0', sizeof(pSet->item_list));
    return pSet;
}

void Set_insert(Set* pSet, void* pItem){
    assert(pSet != NULL);
    unsigned long index = pSet->hash_func(pItem) % SET_SIZE;
    if(pSet->item_list[index] == NULL){
        // item not there
        pSet->item_list[index] = pItem;
    } else {
        // item already there
        if(pSet->comp_func(pItem, pSet->item_list[index])){
            // Same
            return;
        } else {
            ++index;
            // Linear probing
            while(pSet->item_list[index] != NULL){
                if(pSet->comp_func(pItem, pSet->item_list[index])){
                    return;
                }
                ++index;
                index %= SET_SIZE;
            }
            pSet->item_list[index] = pItem;
        }
    }
    return;
}

bool Set_remove(Set* pSet, void* pItem){
    assert(pSet != NULL);
    unsigned long index = pSet->hash_func(pItem) % SET_SIZE;
    while (pSet->item_list[index] != NULL){
        if(pSet->comp_func(pItem, pSet->item_list[index])){
            pSet->item_list[index] = NULL;
            fix_removed(pSet, index);
            return 0;
        }
    }
    return 1;
}

static void fix_removed(Set* pSet, int removed_index){
    int index = removed_index + 1 % SET_SIZE;
    if(pSet->item_list[index] == NULL){
        return;
    }
    while(pSet->item_list[index] != NULL){
        if(pSet->hash_func(pSet->item_list[index]) % SET_SIZE <= removed_index){
            pSet->item_list[removed_index] = pSet->item_list[index];
            pSet->item_list[index] = NULL;
            fix_removed(pSet, index);
        } else {
            ++index;
            index %= SET_SIZE;
        }
    }
}

bool Set_contains(Set* pSet, void* pItem){
    assert(pSet != NULL);
    unsigned long index = pSet->hash_func(pItem) % SET_SIZE;
    while(pSet->item_list[index] != NULL){
        if(pSet->comp_func(pItem, pSet->item_list[index])){
            return 1;
        }
        ++index;
        index %= SET_SIZE;
    }
    return 0;
}

void Set_free(Set* pSet){
    free(pSet);
}
