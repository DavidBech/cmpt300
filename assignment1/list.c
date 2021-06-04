#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

/* Static Function Declarations */
// O(n) setup function for lists called the first time List_create is called.
static void List_initialize();
// Add Node to empty list, same for all functions that add a node
static void List_add_to_empty(List* pList, void* pItem);
// Add Node to start when out of bounds at start
static void List_add_OOB_start(List* pList, void* pItem);
// Add Node to end when out of bounds at end
static void List_add_OOB_end(List* pList, void* pItem);

/* List Variables */
static bool List_initialized = false;

/* List Head Variables */
static List List_headArray[LIST_MAX_NUM_HEADS];
static List* List_next_head;

/* List Node Variables */
static Node List_nodeArray[LIST_MAX_NUM_NODES];
static Node* List_next_node;

/* Start of Function Definitions */

static void List_initialize(){
    // Set up stack for List heads
    for(int i=0; i<LIST_MAX_NUM_HEADS-1; ++i){
        List_headArray[i].pCur_Hn.nextHead = &List_headArray[i+1];
        List_headArray[i].inUse = false;
    }
    List_headArray[LIST_MAX_NUM_HEADS].pCur_Hn.nextHead = NULL;
    List_headArray[LIST_MAX_NUM_HEADS].inUse = false;
    List_next_head = &List_headArray[0];

    // Set up single linked indexes for List Nodes
    for(int i=0; i<LIST_MAX_NUM_NODES-1; ++i){
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

    List* pList = List_next_head;
    List_next_head = List_next_head->pCur_Hn.nextHead;
    pList->pCur_Hn.curNode = NULL;
    pList->lastNode = NULL;
    pList->lastNode = NULL;
    pList->boundCheck = LIST_EMPTY;
    pList->size = 0;
    pList->inUse = true;
    return pList;
}

int List_count(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    return pList->size;
}

/* Functions to fetch Nodes */

void* List_curr(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    if(pList->size == 0 || pList->boundCheck == LIST_OOB_END || pList->boundCheck == LIST_OOB_START){
        return NULL;
    }  
    return pList->pCur_Hn.curNode->item;
}

void* List_first(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    if(pList->size == 0){
        return NULL;
    }
    pList->pCur_Hn.curNode = pList->firstNode;
    pList->boundCheck = LIST_IN_BOUNDS;
    return pList->pCur_Hn.curNode->item;
}

void* List_last(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    if(pList->size == 0){
        return NULL;
    }
    pList->pCur_Hn.curNode = pList->lastNode;
    pList->boundCheck = LIST_IN_BOUNDS;
    return pList->pCur_Hn.curNode->item;
}

void* List_next(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    if(pList->size == 0 || pList->boundCheck == LIST_OOB_END){
        return NULL;
    }
    else if(pList->pCur_Hn.curNode == pList->lastNode){
        pList->boundCheck = LIST_OOB_END;
        pList->pCur_Hn.curNode = NULL;
        return NULL;
    } else if (pList->boundCheck == LIST_OOB_START){
        pList->pCur_Hn.curNode = pList->firstNode;
        pList->boundCheck = LIST_IN_BOUNDS;
        return pList->pCur_Hn.curNode->item;
    } 
    pList->pCur_Hn.curNode = pList->pCur_Hn.curNode->next;
    return pList->pCur_Hn.curNode->item;
}

void* List_prev(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    if(pList->size == 0 || pList->boundCheck == LIST_OOB_START){
        return NULL;
    }
    if(pList->pCur_Hn.curNode == pList->firstNode){
        pList->boundCheck = LIST_OOB_START;
        pList->pCur_Hn.curNode = NULL;
        return NULL;
    } else if (pList -> boundCheck == LIST_OOB_END){
        pList->pCur_Hn.curNode = pList->lastNode;
        pList->boundCheck = LIST_IN_BOUNDS;
        return pList->pCur_Hn.curNode->item;
    }
    pList->pCur_Hn.curNode = pList->pCur_Hn.curNode->prev;
    return pList->pCur_Hn.curNode->item;
}

/* Functions to Add Nodes to list */

int List_add(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    if(List_next_node == NULL){
        return LIST_FAIL;
    }

    if(pList->size == 0){
        // Empty List Case
        List_add_to_empty(pList, pItem);
    } else if(pList->boundCheck == LIST_IN_BOUNDS){
        // Generic case
        // before:              after:
        //     Nd0 -> Nd1           Nd0 -> NEW -> Nd1
        //      |                           |
        //     cur                         cur
        assert(pList->pCur_Hn.curNode != NULL);
        Node* Node0 = pList->pCur_Hn.curNode;
        Node* Node1 = pList->pCur_Hn.curNode->next;
        Node* NewNode = List_next_node;
        
        // Set up next node
        List_next_node = List_next_node->next;

        // Update Pointers        
        NewNode->prev = Node0;
        NewNode->next = Node1;
        Node0->next = NewNode;
        if(Node1 != NULL){
            Node1->prev = NewNode;
        } else {
            pList->lastNode = NewNode;
        }

        // Update fields
        NewNode->item = pItem;
        pList->pCur_Hn.curNode = NewNode;
        ++(pList->size);
    } else if(pList->boundCheck == LIST_OOB_START){
        // Current Node Before List
        List_add_OOB_start(pList, pItem);
    } else if(pList->boundCheck == LIST_OOB_END){
        // Current Node After List
        List_add_OOB_end(pList, pItem);
    } else {
        // Should never get here
        assert(false);
    }
    return LIST_SUCCESS;
}

int List_insert(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    if(List_next_node == NULL){
        return LIST_FAIL;
    } 

    if(pList->size == 0){
        // Empty List Case
        List_add_to_empty(pList, pItem);
    } else if(pList->boundCheck == LIST_IN_BOUNDS){
        // Generic Case
        //before:              after:
        //     Nd0 -> Nd1           Nd0 -> NEW -> Nd1
        //             |                    |
        //            cur                  cur

        assert(pList->pCur_Hn.curNode != NULL);
        Node* Node0 = pList->pCur_Hn.curNode->prev;
        Node* Node1 = pList->pCur_Hn.curNode;
        Node* NewNode = List_next_node;
        
        // Set up next node
        List_next_node = List_next_node->next;

        // Update pointers
        NewNode->prev = Node0;
        NewNode->next = Node1;
        Node1->prev = NewNode;
        if(Node0 != NULL){
            Node0->next = NewNode;
        } else {
            pList->firstNode = NewNode;
        }

        // Update Fields
        NewNode->item = pItem;
        pList->pCur_Hn.curNode = NewNode;
        ++(pList->size);
    } else if(pList->boundCheck == LIST_OOB_START){
        // Current Node Before List
        List_add_OOB_start(pList, pItem);   
    } else if(pList->boundCheck == LIST_OOB_END){
        // Current Node After List
        List_add_OOB_end(pList, pItem);
    } else {
        // Should never get here
        assert(false);
    }
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    if(List_next_node == NULL){
        return LIST_FAIL;
    } 
    if(pList->size == 0){
        List_add_to_empty(pList, pItem);
    } else {
        //before:           after:
        //     Nd0 -> NULL      Nd0 -> NEW -> NULL
        //      |                       |
        //     lst                   cur/lst
        assert(pList->lastNode != NULL);
        Node* Node0 = pList->lastNode;
        Node* NewNode = List_next_node;

        // Set up next node
        List_next_node = List_next_node->next;

        // Update Pointers
        NewNode->prev = Node0;
        NewNode->next = NULL;
        Node0->next = NewNode;

        // Update Fields
        NewNode->item = pItem;
        pList->lastNode = NewNode;
        pList->pCur_Hn.curNode = NewNode;
        pList->boundCheck = LIST_IN_BOUNDS;
        ++(pList->size);
    }
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    if(List_next_node == NULL){
        return LIST_FAIL;
    } 
    if(pList->size == 0){
        List_add_to_empty(pList, pItem);
    } else {
        //before:       after:
        //     Nd0          NEW -> Nd0 
        //      |            |
        //     fst        fst/cur
        assert(pList->firstNode != NULL);
        Node* Node0 = pList->firstNode;
        Node* NewNode = List_next_node;

        // Set up Next node
        List_next_node = List_next_node->next;

        // Update Pointers
        Node0->prev = NewNode;
        NewNode->next = Node0;
        NewNode->prev = NULL;

        // Update Fields
        NewNode->item = pItem;
        pList->firstNode = NewNode;
        pList->pCur_Hn.curNode = NewNode;
        pList->boundCheck = LIST_IN_BOUNDS;
        ++(pList->size);
    }
    return LIST_SUCCESS;
}

static void List_add_to_empty(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    assert(pList->size == 0);
    assert(pList->boundCheck == LIST_EMPTY);
    // Empty List Case
    Node* NewNode = List_next_node;

    // Set up next node
    List_next_node = List_next_node->next;
    
    // Set Node Values to be correct
    NewNode->item = pItem;
    NewNode->next = NULL;
    NewNode->prev = NULL;

    // Set List Values to be correct
    pList->boundCheck = LIST_IN_BOUNDS;
    pList->size = 1;
    pList->pCur_Hn.curNode = NewNode;
    pList->firstNode = NewNode;
    pList->lastNode = NewNode;
}

static void List_add_OOB_start(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    assert(pList->boundCheck == LIST_OOB_START);
    // before:              after:
    //        Nd0                   New -> Nd0
    //    |    |                     |
    //   cur  fst                 cur/fst   
    Node* Node0 = pList->firstNode;
    Node* NewNode = List_next_node;
    
    // Set up next node
    List_next_node = List_next_node->next;

    // Update Pointers
    NewNode->next = Node0;
    NewNode->prev = NULL;
    Node0->prev = NewNode;

    // Update Fields
    NewNode->item = pItem;
    pList->firstNode = NewNode;
    pList->pCur_Hn.curNode = NewNode;
    pList->boundCheck = LIST_IN_BOUNDS;
    ++(pList->size);
}

static void List_add_OOB_end(List* pList, void* pItem){
    assert(pList != NULL);
    assert(pList->inUse);
    assert(pList->boundCheck == LIST_OOB_END);
    // before:          after:
    //      Nd0             Nd0 -> New
    //       |    |                 |
    //      lst  cur             cur/lst  
    Node* Node0 = pList->lastNode;
    Node* NewNode = List_next_node;

    // Set up next node
    List_next_node = List_next_node->next;

    // Update Pointers
    Node0->next = NewNode;
    NewNode->prev = Node0;
    NewNode->next = NULL;
    
    // Update Fields
    NewNode->item = pItem;
    pList->lastNode = NewNode;
    pList->pCur_Hn.curNode = NewNode;
    pList->boundCheck = LIST_IN_BOUNDS;
    ++(pList->size);
}

/* Functions to Remove Nodes from List */

void* List_remove(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    if(pList->boundCheck == LIST_IN_BOUNDS){
        // before:                      after:
        //      Nd0 -> Nd1 -> Nd2           Nd0 -> Nd2
        //              |                           |
        //             cur                         cur
        // Note: cur may potentially be NULL after this operation
        assert(pList->pCur_Hn.curNode != NULL);
        Node* Node0 = pList->pCur_Hn.curNode->prev;
        Node* Node1 = pList->pCur_Hn.curNode;
        Node* Node2 = pList->pCur_Hn.curNode->next;
        void* pItem = pList->pCur_Hn.curNode->item;
        
        // Update Pointers
        if(Node0 != NULL && Node2 != NULL){
            Node0->next = Node2;
            Node2->prev = Node0;
        } else if(Node0 != NULL) { 
            // Node2 == NULL
            Node0->next = NULL;
            pList->lastNode = Node0;
            pList->boundCheck = LIST_OOB_END;
        } else if(Node2 != NULL){
            // Node0 == NULL
            Node2->prev = NULL;
            pList->firstNode = Node2;
        } else {
            // Node2 && Node0 == NULL
            pList->lastNode = NULL;
            pList->firstNode = NULL;
            pList->boundCheck = LIST_EMPTY;
            assert(pList->size == 1);
        }

        // Add Node1 Back into the pool of nodes
        Node1->prev = NULL;
        Node1->item = NULL;
        Node1->next = List_next_node;
        List_next_node = Node1;

        // Update Remaining Fields
        pList->pCur_Hn.curNode = Node2;
        --(pList->size);
        return pItem;
    }

    // Either empty or out of bounds
    return NULL;
}

void* List_trim(List* pList){
    assert(pList != NULL);
    assert(pList->inUse);
    // TODO if statement incorrect
    if(pList->boundCheck != LIST_EMPTY){
        // before:              after:
        //      Nd0 -> Nd1          Nd0
        //              |            |
        //             lst        cur/lst
        assert(pList->lastNode != NULL);
        Node* Node0 = pList->lastNode->prev;
        Node* Node1 = pList->lastNode;
        void* pItem = pList->lastNode->item;

        // Remove node from list and add to pool
        Node1->prev = NULL;
        Node1->item = NULL;
        Node1->next = List_next_node;
        List_next_node = Node1;
        
        if(Node0 != NULL){
            Node0->next = NULL;
            pList->boundCheck = LIST_IN_BOUNDS;
        } else {
            // Node1 is the only node in the list
            pList->firstNode = NULL;            
            pList->boundCheck = LIST_EMPTY;
        }
        pList->pCur_Hn.curNode = Node0;
        pList->lastNode = Node0;
        --(pList->size);
        return pItem;
    }
    // Empty list return null
    return NULL;
}

/* Functions Other */

void List_concat(List* pList1, List* pList2){
    assert(pList1 != NULL);
    assert(pList1->inUse);
    if(pList2 == NULL){
        return;
    } 
    assert(pList2->inUse);
    if(pList2->size == 0){
        // Pass
        // Concatination with nothing should be unchanged
    } else if (pList1->size == 0){
        pList1->firstNode = pList2->firstNode;
        pList1->lastNode = pList2->lastNode;
        pList1->size = pList2->size;

        // move current pointer to "before" list
        pList1->pCur_Hn.curNode = NULL;
        pList1->boundCheck = LIST_OOB_START;
    } else {
        pList1->lastNode->next = pList2->firstNode;
        pList2->firstNode->prev = pList1->lastNode;
        pList1->lastNode = pList2->lastNode;
        pList1->size += pList2->size;
    }
    // return pList2 to the pool of heads
    pList2->firstNode = NULL;
    pList2->lastNode = NULL;
    pList2->pCur_Hn.curNode = NULL;
    pList2->boundCheck = LIST_EMPTY;
    pList2->size = 0;
    pList2->inUse = false;
    pList2->pCur_Hn.nextHead = List_next_head;
    List_next_head = pList2;
}

void List_free(List* pList, FREE_FN pItemFreeFn){
    assert(pList != NULL);
    assert(pItemFreeFn != NULL);
    assert(pList->inUse);
    void* pItem;
    List_last(pList);
    while(pList->pCur_Hn.curNode != NULL){
        pItem = List_trim(pList);
        (*pItemFreeFn)(pItem);
    }
    assert(pList->firstNode == NULL);
    assert(pList->lastNode == NULL);
    assert(pList->pCur_Hn.curNode == NULL);
    assert(pList->boundCheck == LIST_EMPTY);
    assert(pList->size == 0);
    pList->inUse = false;
    pList->pCur_Hn.nextHead = List_next_head;
    List_next_head = pList;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    assert(pList != NULL);
    assert(pComparator != NULL);
    assert(pComparisonArg != NULL);
    assert(pList->inUse);
    if(pList->size == 0){
        return NULL;
    }

    if(pList->pCur_Hn.curNode == NULL && pList->boundCheck == LIST_OOB_START){
        pList->pCur_Hn.curNode = pList->firstNode;
    }
    while(pList->pCur_Hn.curNode != NULL){
        if(pComparator(pList->pCur_Hn.curNode->item, pComparisonArg) == true){
            pList->boundCheck = LIST_IN_BOUNDS;
            return pList->pCur_Hn.curNode->item;
        }
        List_next(pList);
    }
    pList->boundCheck = LIST_OOB_END;
    return NULL;
}
