#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

/* Static Function Declarations */
static void List_initialize();

/* List Variables */
static bool List_initialized = false;

/* List Head Variables */
static int List_number_in_use_heads = 0;
static List List_headArray[LIST_MAX_NUM_HEADS];

typedef struct List_adj_t List_adj;
struct List_adj_t {
    List* head;
    List_adj* next;
};
static List_adj List_head_next_list[LIST_MAX_NUM_HEADS];
static List_adj* List_next_head;

/* List Node Variables */
static int List_number_in_use_nodes = 0;
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

    if(List_number_in_use_heads >= 10){
        return NULL;
    }

    List* pList = List_next_head->head;
    List_next_head = List_next_head->next;
    ++List_number_in_use_heads;
    return pList;
}

int List_count(List* pList){
    assert(pList != NULL);
    return pList->size;
}

void* List_curr(List* pList){
    assert(pList != NULL);
    return pList->curNode->item;
}

/* Functions to fetch Nodes */
void* List_first(List* pList){
    assert(pList != NULL);
    pList->curNode = pList->firstNode;
    return pList->curNode->item;
}

void* List_last(List* pList){
    assert(pList != NULL);
    pList->curNode = pList->lastNode;
    return pList->curNode->item;
}

void* List_next(List* pList){
    assert(pList != NULL);
    if(pList->boundCheck == LIST_OOB_END || pList->curNode == pList->lastNode){
        // TODO - at node [last-1, last]
        assert(false);
    } else if (pList->boundCheck == LIST_OOB_START){
        // TODO - at node [-1]
        assert(false);
    } 
    pList->curNode = pList->curNode->next;
    return pList->curNode->item;
}

void* List_prev(List* pList){
    assert(pList != NULL);
    if(pList->boundCheck == LIST_OOB_START || pList->curNode == pList->firstNode){
        // TODO - at node [-1, 0]
        assert(false);
    } else if (pList -> boundCheck == LIST_OOB_END){
        // TODO - at node last
        assert(false);
    }
    pList->curNode = pList->curNode->prev;
    return pList->curNode->item;
}

/* Functions to Add Nodes to list */
int List_add(List* pList, void* pItem){
    assert(pList != NULL);
    if(List_number_in_use_nodes >= LIST_MAX_NUM_NODES){
        #ifdef DEBUG
            fprintf(stderr, "Error: in &s, at line &d, added to list when out of nodes", __FILE__, __LINE__);    
        #endif
        return LIST_FAIL;
    }

    if(pList->size == 0){
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

    } else if(pList->boundCheck == LIST_IN_BOUNDS){
        // Generic case
        Node* previous = pList->curNode;
        pList->curNode = List_next_node;

        // Set up next node
        List_next_node = List_next_node->next;

        // Update Pointers        
        pList->curNode->prev = previous;
        pList->curNode->next = previous->next;
        previous->next = pList->curNode;

        // Update Node
        pList->curNode->item = pItem;

        // Update List
        ++(pList->size);
        // Set last node if necessary
        if(previous == pList->lastNode){
            pList->lastNode = pList->curNode;
        }
    } else if(pList->boundCheck == LIST_OOB_START){
        // Current Node "Before" List
        Node* previous = pList->firstNode;
        pList->curNode = List_next_node;

        // Set up next node
        List_next_node = List_next_node->next;

        pList->firstNode = pList->curNode;
        pList->curNode->next = previous;
        pList->curNode->prev = NULL;
        previous->prev = pList->curNode;
        ++(pList->size);
    } else if(pList->boundCheck == LIST_OOB_END){
        // Current Node "After" List
        Node* previous = pList->lastNode;
        pList->curNode = List_next_node;

        // Set up next node
        List_next_node = List_next_node->next;

        pList->lastNode = pList->curNode;
        pList->curNode->prev = previous;
        pList->curNode->next = NULL;
        previous->next = pList->curNode;
        ++(pList->size);
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
    // TODO
    assert(false);
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem){
    assert(pList != NULL);
    // TODO
    assert(false);
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem){
    assert(pList != NULL);
    // TODO
    assert(false);
    return LIST_SUCCESS;
}

void List_concat(List* pList1, List* pList2){
    assert(pList1 != NULL);
    assert(pList2 != NULL);
    // TODO
    assert(false);
}

/* Functions to Remove Nodes from List */
void* List_remove(List* pList){
    assert(pList != NULL);
    // TODO
    assert(false);
    return NULL;
}

void List_free(List* pList, FREE_FN pItemFreeFn){
    assert(pList != NULL);
    assert(pItemFreeFn != NULL);
    // TODO
    assert(false);
}

void* List_trim(List* pList){
    assert(pList != NULL);
    // TODO
    assert(false);
    return NULL;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    assert(pList != NULL);
    assert(pComparator != NULL);
    assert(pComparisonArg != NULL);
    // TODO
    assert(false);
    return NULL;
}


