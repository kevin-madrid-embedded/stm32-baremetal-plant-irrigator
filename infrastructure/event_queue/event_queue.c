#include "event_queue.h"
#include "stddef.h"


void queue_init(EventQueue_t *queue){
    if(queue == NULL)return;

    queue->Head = 0;
    queue->Tail = 0;
    queue->Empty = 1;
    queue->Full = 0;
    queue->Elements = 0;
}

bool is_empty(const EventQueue_t *queue){
    if (queue->Empty == 1)
        return true;
    return false;
}
bool is_full(const EventQueue_t *queue){
    if (queue->Full == 1)
        return true;
    return false;
}
bool queue_push(EventQueue_t *queue, Events_t in){
    if(queue == NULL)return false;
    if(queue->Full != 0)return false;

    queue->Buffer[queue->Head] = in; 
    queue->Head = (queue->Head + 1) % queue->Elements;
    if(queue->Head == queue->Tail){
        queue->Full = 1; 
    }
    queue->Empty = 0;
    return true;
}
bool queue_pop(EventQueue_t *queue, Events_t *out){

    if(queue == NULL)return false;
    if(queue->Empty != 0)return false;

    Events_t *output = out;

    *output = queue->Buffer[queue->Tail];
    queue->Tail = (queue->Tail +1)% queue->Elements;//If(Tail+1 == Elements)tail = 0; else tail++;
    if(queue->Tail == queue->Head){
        queue->Empty = 1; 
    }
    queue->Full = 0;
    return true;
}


