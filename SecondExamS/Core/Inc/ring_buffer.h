#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include <stdint.h>

typedef struct ring_buffer_
{
    // Pointer to the memory array where data will be stored.
    uint8_t *buffer;
    // where the next byte of data will be written.
    uint8_t head;
    // Index from where the next byte of data will be read.

    uint8_t tail;
    uint8_t is_full;

    uint8_t capacity;

} ring_buffer_t;
// Initializes the ring buffer by assigning the memory and setting initial values for the head, tail, and flags.
void ring_buffer_init(ring_buffer_t *rb, uint8_t *mem_add, uint8_t capacity);
// Resets the buffer, clearing all data, and resetting head, tail, and is_full flag.
void ring_buffer_reset(ring_buffer_t *rb);
// Returns the current number of elements stored in the buffer.
uint8_t ring_buffer_size(ring_buffer_t *rb);
uint8_t ring_buffer_is_full(ring_buffer_t *rb);
uint8_t ring_buffer_is_empty(ring_buffer_t *rb);
// Writes data into the buffer. If the buffer is full, the oldest data is overwritten.
void ring_buffer_write(ring_buffer_t *rb, uint8_t data);
// Reads data from the buffer. Returns 1 if data was successfully read, 0 if the buffer is empty.
uint8_t ring_buffer_read(ring_buffer_t *rb, uint8_t *byte);

#endif /* INC_RING_BUFFER_H_ */
