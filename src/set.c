#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "set.h"

// This function recursively fixes the linear probing errors after setting
//  removing an element
static void fix_removed(Set* pSet, int removed_index);

Set* Set_create(HASH_FUNCTION hash_func, COMPARE_FUNCTION comp_func){
    if(hash_func == NULL || comp_func == NULL){
        errno = EINVAL;
        return NULL;
    }
    Set* pSet = malloc(sizeof(Set));
    if(pSet == NULL){
        errno = ENOMEM;
        return NULL;
    }
    pSet->item_list = calloc(SET_SIZE, sizeof(void*));
    if(pSet->item_list == NULL){
        free(pSet);
        errno = ENOMEM;
        return NULL;
    }
    pSet->hash_func = hash_func;
    pSet->comp_func = comp_func;
    return pSet;
}

void Set_insert(Set* pSet, void* pItem){
    assert(pSet != NULL);
    if(pItem == NULL){
        return;
    }
    unsigned long index = pSet->hash_func(pItem) % SET_SIZE;
    if(pSet->item_list[index] == NULL){
        // item not there
        pSet->item_list[index] = pItem;
    } else {
        // item already there
        if(pSet->comp_func(pItem, pSet->item_list[index])){
            // Same item
            return;
        } else {
            ++index;
            index %= SET_SIZE;
            // Linear probing
            while(pSet->item_list[index] != NULL){
                if(pSet->comp_func(pItem, pSet->item_list[index])){
                    // Same Item
                    return;
                }
                ++index;
                index %= SET_SIZE;
            }
            // First NULL entry after hash index
            pSet->item_list[index] = pItem;
        }
    }
    return;
}

void Set_remove(Set* pSet, void* pItem){
    assert(pSet != NULL);
    if(pItem == NULL){
        return;
    }
    unsigned long index = pSet->hash_func(pItem) % SET_SIZE;
    while (pSet->item_list[index] != NULL){
        // Find item
        if(pSet->comp_func(pItem, pSet->item_list[index])){
            pSet->item_list[index] = NULL;
            fix_removed(pSet, index);
            return;
        }
        ++index;
        index %= SET_SIZE;
    }
    return;
}

static void fix_removed(Set* pSet, int removed_index){
    int index = (removed_index + 1) % SET_SIZE;
    while(pSet->item_list[index] != NULL){
        // Get the expected index
        unsigned long index_hash = pSet->hash_func(pSet->item_list[index]) % SET_SIZE;
        // Find distance from expected index to the removed index
        unsigned long distance_removed = removed_index>=index_hash ? removed_index-index_hash : removed_index+SET_SIZE-index_hash;
        // Find distance from expected index to actual index
        unsigned long distance_hash = index>=index_hash ? index-index_hash : index+SET_SIZE-index_hash;

        // If the removed_index would place the item closer to its hash move it
        if(distance_removed < distance_hash){
            pSet->item_list[removed_index] = pSet->item_list[index];
            pSet->item_list[index] = NULL;
            // Created new hole in array, need to fix
            fix_removed(pSet, index);
            return;
        } else {
            ++index;
            index %= SET_SIZE;
        }
    }
}

bool Set_contains(Set* pSet, void* pItem){
    assert(pSet != NULL);
    if(pItem == NULL){
        return 0;
    }
    unsigned long index = pSet->hash_func(pItem) % SET_SIZE;
    while(pSet->item_list[index] != NULL){
        if(pSet->comp_func(pItem, pSet->item_list[index])){
            // Item in set
            return 1;
        }
        ++index;
        index %= SET_SIZE;
    }
    return 0;
}

void Set_free(Set* pSet){
    free(pSet->item_list);
    free(pSet);
}
