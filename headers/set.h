#include <stdbool.h>
/*
    Author: David Bechert

    This module defines a set supporting insert, remove, and contains.
    When a set is created it is passed two functions, one to calculate item hashes
        the other to compare equality of items.
    The set assumes it never fills up and no handling is done when this occurs.
    The set assumes items will never be modified once inserted.
    The set does not free items placed in it when it is freed.    
    The set uses linear probing for items with equal hashes.
    An Item of NULL may not be added to the set.
*/

#ifndef __SET_H
#define __SET_H
// Set assumes it never fills up
// Set assumes the items added will not change once added

// This is just some arbitrary prime
#define SET_SIZE 1021

// Implemenation of hash of items in set returning an unsigned long item hash
typedef unsigned long (*HASH_FUNCTION)(void* pItem);

// Implementation of comparison of items in set returns 1 on match 0 otherwise
typedef bool (*COMPARE_FUNCTION)(void* pItem0, void* pItem1);

typedef struct set_s Set;
struct set_s{
    void** item_list;
    HASH_FUNCTION hash_func;
    COMPARE_FUNCTION comp_func;
};

// Creates new set
//  returns pointer to set or NULL with errno set
Set* Set_create(HASH_FUNCTION hash_func, COMPARE_FUNCTION comp_func);

// Inserts new item in set
void Set_insert(Set* pSet, void* pItem);

// Removes item from set
void Set_remove(Set* pSet, void* pItem);

// Checks if set contains item
//  return 1 if set contains the value
//  return 0 otherwise 
bool Set_contains(Set* pSet, void* pItem);

// Frees set
void Set_free(Set* pSet);

#endif
