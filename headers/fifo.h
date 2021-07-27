#ifndef __FIFO_H
#define __FIFO_H

typedef struct Node_s FIFO_Node;
struct Node_s{
    FIFO_Node* next;
    void* pItem;
};

typedef struct fifo_s fifo;
struct fifo_s{
    FIFO_Node* front;
    FIFO_Node* back;
    unsigned size;
};

fifo* FIFO_create();

void FIFO_enqueue(fifo* pFifo, void* item);

void* FIFO_dequeue(fifo* pFifo);

unsigned FIFO_get_size(fifo* pFifo);

void FIFO_free(fifo* pFifo);

#endif
