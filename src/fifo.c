#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "fifo.h"

fifo* FIFO_create(){
    fifo* pFifo = malloc(sizeof(fifo));
    pFifo->front = NULL;
    pFifo->back = NULL;
    pFifo->size = 0;
    return pFifo;
}

void FIFO_enqueue(fifo* pFifo, void* pItem){
    assert(pFifo != NULL);
    if(pFifo->size == 0){
        pFifo->front = malloc(sizeof(FIFO_Node));
        if(pFifo->front == NULL){
            printf("Out of mem line:%u\n", __LINE__);
            exit(EXIT_FAILURE);
        }
        pFifo->front->pItem = pItem;
        pFifo->back = pFifo->front;
    } else {
        FIFO_Node* prev_back = pFifo->back;
        pFifo->back = malloc(sizeof(FIFO_Node));
        pFifo->back->next = NULL;
        pFifo->back->pItem = pItem;
        prev_back->next = pFifo->back;
    }
    ++pFifo->size;
}

void* FIFO_dequeue(fifo* pFifo){
    assert(pFifo != NULL);
    void* pItem = NULL;
    if(pFifo->size == 0){
        return NULL;
    } else {
        pItem = pFifo->front->pItem;
        FIFO_Node* prev_front = pFifo->front;
        pFifo->front = pFifo->front->next;
        if(pFifo->back == prev_front){
            pFifo->back = NULL;
            pFifo->front = NULL;
        }
        free(prev_front);
        --pFifo->size;
        return pItem;
    }
}

unsigned FIFO_get_size(fifo* pFifo){
    assert(pFifo != NULL);
    return pFifo->size;
}

void FIFO_free(fifo* pFifo){
    assert(pFifo != NULL);
    FIFO_Node* pNode = pFifo->front;
    while(pNode != NULL){
        FIFO_Node* prev = pNode;
        pNode = pNode->next;
        free(prev);
    }
    free(pFifo);
    pFifo = NULL;
}
