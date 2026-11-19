#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stdbool.h>
#include <stdint.h>

#define EVENT_QUEUE_SIZE 32

typedef enum{
    EV_INVALID,
    EV_INIT_COMPLETE,
    EV_WATER_LEVEL_OK, 
    EV_WATER_LEVEL_LOW,
    EV_MOISTURE_LEVEL_OK,
    EV_MOISTURE_LEVEL_LOW,
    EV_WATERING,
    EV_WATERING_TIMEOUT,
    EV_SLEEP,
    EV_SLEEP_TIMEOUT
} Events_t;

typedef struct {
    Events_t Buffer[EVENT_QUEUE_SIZE];
    volatile uint32_t Head; // Write index (modified by producers)
    volatile uint32_t Tail; // Read index (modified by consumer)
    volatile uint32_t Empty;
    volatile uint32_t Full;
    volatile uint32_t Elements;
} EventQueue_t;

void queue_init(EventQueue_t *queue);
bool is_empty(const EventQueue_t *queue);
bool is_full(const EventQueue_t *queue);
bool queue_push(EventQueue_t *queue, Events_t in);
bool queue_pop(EventQueue_t *queue, Events_t *out);

#endif