#include "ringbuff.h"

void ring_buffer_init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
}

uint8_t ring_buffer_write(RingBuffer *rb, char c) {
    uint32_t next_head = (rb->head + 1) % RING_BUFFER_SIZE;
    
    if (next_head == rb->tail) {
        return 0; 
    }
    
    rb->data[rb->head] = c;
    rb->head = next_head;
    return 1;
}

uint8_t ring_buffer_read(RingBuffer *rb, char *c) {
    if (rb->head == rb->tail) {
        return 0; 
    }
    
    *c = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    return 1;
}

uint8_t ring_buffer_available(RingBuffer *rb) {
    return (rb->head != rb->tail) ? 1 : 0;
}

uint32_t ring_buffer_count(RingBuffer *rb) {
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    } else {
        return RING_BUFFER_SIZE - rb->tail + rb->head;
    }
}