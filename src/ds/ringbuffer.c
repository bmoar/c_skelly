#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ds/debug.h>
#include <ds/ringbuffer.h>

RingBuffer *RingBuffer_create(int length) {
    check(length > 0, "Tried to create a buffer with len < 1");

    RingBuffer *buffer = malloc(sizeof(RingBuffer));
    check_mem(buffer);
    buffer->length = length + 1;
    buffer->start = 0;
    buffer->end = 0;
    buffer->buffer = malloc(buffer->length);

    return buffer;
error:
    return NULL;
}

void RingBuffer_destroy(RingBuffer *buffer) {
    if(buffer) {
        free(buffer->buffer);
        free(buffer);
    }
}

int RingBuffer_read(RingBuffer *buffer, char *target, int amount) {
    check(buffer, "Tried to get from a NULL RingBuffer");
    check(target, "Tried to write to a NULL target");
    check(amount <= buffer->length, "Tried to read more data than buffer can hold"); 

    check_debug(amount <= RingBuffer_available_data(buffer),
            "Not enough data in buffer: has %d, needs %d",
            RingBuffer_available_data(buffer), amount);

    void *result = memcpy(target, RingBuffer_starts_at(buffer), amount);
    check(result != NULL, "Failed to write buffer into data");

    RingBuffer_commit_read(buffer, amount);

    if(buffer->end == buffer->start) {
        buffer->start = buffer->end = 0;
    }

    return amount;

error:
    return -1;
}

int RingBuffer_write(RingBuffer *buffer, char *data, int length) {
    check(buffer, "Tried to get from a NULL RingBuffer");
    check(length > 0, "Tried to write less than 1 byte of data to buffer");

    if(RingBuffer_available_data(buffer) == 0) {
        buffer->start = buffer->end = 0;
    }

    check(length <= RingBuffer_available_space(buffer),
            "Not enough space in buffer: %d available, %d requested",
            RingBuffer_available_space(buffer), length);

    void *result = memcpy(RingBuffer_ends_at(buffer), data, length);
    check(result != NULL, "Failed to write data into buffer.");

    RingBuffer_commit_write(buffer, length);

    return length;
error:
    return -1;
}

bstring RingBuffer_gets(RingBuffer *buffer, int amount) {
    check(buffer, "Tried to get from a NULL RingBuffer");
    check(amount <= buffer->length, "Tried to get more data than buffer can hold");
    check(amount > 0, "Need more than 0 for gets, you gave: %d", amount);
    check_debug(amount <= RingBuffer_available_data(buffer),
            "Not enough data in buffer.");

    bstring result = blk2bstr(RingBuffer_starts_at(buffer), amount);
    check(result != NULL, "Failed to create gets result.");
    check(blength(result) == amount, "Wrong result length.");

    RingBuffer_commit_read(buffer, amount);
    assert(RingBuffer_available_data(buffer) >= 0 && "Error in read commit.");

    return result;

error:
    return NULL;
}
