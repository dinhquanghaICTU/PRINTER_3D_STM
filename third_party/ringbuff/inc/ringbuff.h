#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

#define RING_BUFFER_SIZE 128

typedef struct {
    volatile char data[RING_BUFFER_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
} RingBuffer;

void ring_buffer_init(RingBuffer *rb);
uint8_t ring_buffer_write(RingBuffer *rb, char c);
uint8_t ring_buffer_read(RingBuffer *rb, char *c);
uint8_t ring_buffer_available(RingBuffer *rb);
uint32_t ring_buffer_count(RingBuffer *rb);

#endif // RINGBUFFER_H