#include <stdbool.h>

#ifndef __SET_H
#define __SET_H

// This is just some arbitrary prime
#define SET_SIZE 11047

// Implemenation of hash of items in set
typedef unsigned long (*HASH_FUNCTION)(void* pItem);

// Implementation of comparison of items in set
//  returns 1 on match
typedef bool (*COMPARE_FUNCTION)(void* pItem0, void* pItem1);

typedef struct set_s Set;
struct set_s{
    void** item_list;
    HASH_FUNCTION hash_func;
    COMPARE_FUNCTION comp_func;
};

// Creates new set
//  returns pointer to set
Set* Set_create(HASH_FUNCTION hash_func, COMPARE_FUNCTION comp_func);

// Inserts new item in set
void Set_insert(Set* pSet, void* pItem);

// Removes item from set
//  return 0 if item was removed 
//  return 1 otherwise
bool Set_remove(Set* pSet, void* pItem);

// Checks if set contains item
//  return 1 if set contains the value
//  return 0 otherwise 
bool Set_contains(Set* pSet, void* pItem);

// Frees set
void Set_free(Set* pSet);

#endif
