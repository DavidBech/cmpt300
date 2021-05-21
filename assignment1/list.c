#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

/* Static Function Declarations */
static void List_initialize();
static void List_add_to_empty(List* pList, void* pItem);
static void List_add_OOB_start(List* pList, void* pItem);
static void List_add_OOB_end(List* pList, void* pItem);

/* List Variables */
static bool List_initialized = false;

/* List Head Variables */
static List List_headArray[LIST_MAX_NUM_HEADS];

typedef struct List_adj_t List_adj;
struct List_adj_t {
    List* head;
    List_adj* next;
};
static List_adj List_head_next_list[LIST_MAX_NUM_HEADS];
static List_adj* List_next_head;

/* List Node Variables */
static Node List_nodeArray[LIST_MAX_NUM_NODES];
static Node* List_next_node;

/* Start of Function Definitions */

static void List_initialize(){
    // Set up single linked indexes for List heads
    for(int i=0; i<LIST_MAX_NUM_HEADS-2; ++i){
        List_head_next_list[i].head = &List_headArray[i];
        List_head_next_list[i].next = &List_head_next_list[i+1];
    }
    List_head_next_list[LIST_MAX_NUM_HEADS-1].head = &List_headArray[LIST_MAX_NUM_HEADS-1];
    List_head_next_list[LIST_MAX_NUM_HEADS-1].next = NULL;
    List_next_head = &List_head_next_list[0];

    // Set up single linked indexes for List Nodes
    for(int i=0; i<LIST_MAX_NUM_NODES-2; ++i){
        List_nodeArray[i].next = &List_nodeArray[i+1];
    }
    List_nodeArray[LIST_MAX_NUM_NODES-1].next = NULL;
    List_next_node = &List_nodeArray[0];
}

List* List_create(){
    if(!List_initialized){
        List_initialize();
        List_initialized = true;
    }

    if(List_next_head == NULL){
        return NULL;
    }

    List* pList = List_next_head->head;
    List_next_head = List_next_head->next;
    pList->curNode = NULL;
    pList->lastNode = NULL;
    pList->lastNode = NULL;
    pList->boundCheck = LIST_EMPTY;
    pList->size = 0;
    return pList;
}

int List_count(List* pList){
    assert(pList != NULL);
    return pList->size;
}

/* Functions to fetch Nodes */

void* List_curr(List* pList){
    assert(pList != NULL);
    if(pList->size == 0 || pList->boundCheck == LIST_OOB_END || pList->boundCheck == LIST_OOB_START){
        return NULL;
    }  
    return pList->curNode->item;
}

void* List_first(List* pList){
    assert(pList != NULL);
    if(pList->size == 0){
        return NULL;
    }
    pList->curNode = pList->firstNode;
    return pList->curNode->item;
}

void* List_last(List* pList){
    assert(pList != NULL);
    if(pList->size == 0){
        return NULL;
    }
    pList->curNode = pList->lastNode;
    return pList->curNode->item;
}

void* List_next(List* pList){
    assert(pList != NULL);
    if(pList->size == 0 || pList->boundCheck == LIST_OOB_END){
        return NULL;
    }
    else if(pList->curNode == pList->lastNode){
        pList->boundCheck = LIST_OOB_END;
        pList->curNode = NULL;
        return NULL;
    } else if (pList->boundCheck == LIST_OOB_START){
        pList->curNode = pList->firstNode;
        return pList->curNode->item;
    } 
    pList->curNode = pList->curNode->next;
    return pList->curNode->item;
}

void* List_prev(List* pList){
    assert(pList != NULL);
    if(pList->size == 0 || pList->boundCheck == LIST_OOB_START){
        return NULL;
    }
    if(pList->curNode == pList->firstNode){
        pList->boundCheck = LIST_OOB_START;
        pList->curNode = NULL;
        return NULL;
    } else if (pList -> boundCheck == LIST_OOB_END){
        pList->curNode = pList->lastNode;
        return pList->curNode->item;
    }
    pList->curNode = pList->curNode->prev;
    return pList->curNode->item;
}

/* Functions to Add Nodes to list */

int List_add(List* pList, void* pItem){
    assert(pList != NULL);
    if(List_next_node == NULL){
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, added to list when out of nodes", __FILE__, __LINE__);    
        #endif
        return LIST_FAIL;
    }

    if(pList->size == 0){
        // Empty List Case
        List_add_to_empty(pList, pItem);
    } else if(pList->boundCheck == LIST_IN_BOUNDS){
        // Generic case
        assert(pList->curNode == NULL);
        Node* previous = pList->curNode;
        pList->curNode = List_next_node;

        // Set up next node
        List_next_node = List_next_node->next;

        // Update Pointers        
        pList->curNode->prev = previous;
        pList->curNode->next = previous->next;
        previous->next = pList->curNode;
        if(pList->curNode->next != NULL){
            pList->curNode->next->prev = pList->curNode;
        } else {
            pList->lastNode = pList->curNode;
        }

        // Update fields
        pList->curNode->item = pItem;
        ++(pList->size);
    } else if(pList->boundCheck == LIST_OOB_START){
        // Current Node Before List
        List_add_OOB_start(pList, pItem);
    } else if(pList->boundCheck == LIST_OOB_END){
        // Current Node After List
        List_add_OOB_end(pList, pItem);
    } else {
        // Should never get here
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, got to invalid line", __FILE__, __LINE__);    
        #endif
        assert(false);
    }
    return LIST_SUCCESS;
}

int List_insert(List* pList, void* pItem){
    assert(pList != NULL);
    if(List_next_node == NULL){
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, added to list when out of nodes", __FILE__, __LINE__);    
        #endif
        return LIST_FAIL;
    } 

    if(pList->size == 0){
        // Empty List Case
        List_add_to_empty(pList, pItem);
    } else if(pList->boundCheck == LIST_IN_BOUNDS){
        // Generic Case
        assert(pList->curNode == NULL);
        Node* previous = pList->curNode;
        pList->curNode = List_next_node;

        // Set up next node
        List_next_node = List_next_node->next;

        // Update pointers
        pList->curNode->prev = previous->prev;
        pList->curNode->next = previous;
        previous->prev = pList->curNode;
        if(pList->curNode->prev != NULL){
            pList->curNode->prev->next = pList->curNode;
        } else {
            pList->firstNode = pList->curNode;
        }

        // Update Fields
        pList->curNode->item = pItem;
        ++(pList->size);
    } else if(pList->boundCheck == LIST_OOB_START){
        // Current Node Before List
        List_add_OOB_start(pList, pItem);   
    } else if(pList->boundCheck == LIST_OOB_END){
        // Current Node After List
        List_add_OOB_end(pList, pItem);
    } else {
        // Should never get here
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, got to invalid line", __FILE__, __LINE__);    
        #endif
        assert(false);
    }
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem){
    assert(pList != NULL);
    if(List_next_node == NULL){
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, added to list when out of nodes", __FILE__, __LINE__);    
        #endif
        return LIST_FAIL;
    } 
    if(pList->size == 0){
        List_add_to_empty(pList, pItem);
    } else {
        Node* previous = pList->lastNode;
        pList->curNode = List_next_node;
        pList->lastNode = pList->curNode;

        // Set up next node
        List_next_node = List_next_node->next;

        // Update Pointers
        pList->curNode->prev = previous;
        pList->curNode->next = NULL;
        previous->next = pList->curNode;

        // Update Fields
        pList->curNode->item = pItem;
        ++(pList->size);
    }
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem){
    assert(pList != NULL);
    if(List_next_node == NULL){
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, added to list when out of nodes", __FILE__, __LINE__);    
        #endif
        return LIST_FAIL;
    } 
    if(pList->size == 0){
        List_add_to_empty(pList, pItem);
    } else {
        Node* previous = pList->firstNode;
        pList->curNode = List_next_node;
        pList->firstNode = pList->curNode;

        // Set up Next node
        List_next_node = List_next_node->next;

        // Update Pointers
        pList->curNode->prev = NULL;
        pList->curNode->next = previous;
        previous->prev = pList->curNode;

        // Update Fields
        pList->curNode->item = pItem;
        ++(pList->size);
    }
    return LIST_SUCCESS;
}

static void List_add_to_empty(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->size == 0);
    assert(pList->boundCheck == LIST_EMPTY);
    // Empty List Case
    pList->curNode = List_next_node;

    // Set up next node
    List_next_node = List_next_node->next;
    
    // Set List Values to be correct
    pList->boundCheck = LIST_IN_BOUNDS;
    pList->size = 1;
    pList->firstNode = pList->curNode;
    pList->lastNode = pList->curNode;

    // Set Node Values to be correct
    pList->curNode->item = pItem;
    pList->curNode->next = NULL;
    pList->curNode->prev = NULL;
}

static void List_add_OOB_start(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->boundCheck == LIST_OOB_START);
    Node* previous = pList->firstNode;
    pList->curNode = List_next_node;

    // Set up next node
    List_next_node = List_next_node->next;

    // Update Pointers
    pList->firstNode = pList->curNode;
    pList->curNode->next = previous;
    pList->curNode->prev = NULL;
    previous->prev = pList->curNode;

    // Update Fields
    pList->curNode->item = pItem;
    ++(pList->size);
}

static void List_add_OOB_end(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->boundCheck == LIST_OOB_END);
    Node* previous = pList->lastNode;
    pList->curNode = List_next_node;

    // Set up next node
    List_next_node = List_next_node->next;

    // Update Pointers
    pList->lastNode = pList->curNode;
    pList->curNode->prev = previous;
    pList->curNode->next = NULL;
    previous->next = pList->curNode;

    // Update Fields
    pList->curNode->item = pItem;
    ++(pList->size);
}

/* Functions to Remove Nodes from List */

void* List_remove(List* pList){
    assert(pList != NULL);
    // TODO
    assert(false);
    return NULL;
}

void* List_trim(List* pList){
    assert(pList != NULL);
    // TODO
    assert(false);
    return NULL;
}

/* Functions Other */

void List_concat(List* pList1, List* pList2){
    assert(pList1 != NULL);
    assert(pList2 != NULL);
    // TODO
    assert(false);
}

void List_free(List* pList, FREE_FN pItemFreeFn){
    assert(pList != NULL);
    assert(pItemFreeFn != NULL);
    // TODO
    assert(false);
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    assert(pList != NULL);
    assert(pComparator != NULL);
    assert(pComparisonArg != NULL);
    // TODO
    assert(false);
    return NULL;
}


