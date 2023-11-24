#include "../include/memAllocations.h"

#include <string.h>
#include <stdio.h>
#include "../include/memAllocations_cfg.h"

void* dynArrCalloc(void* arr, unsigned int* capacity,
                              unsigned int* freeIndex, size_t elemSize)
{
    if (*capacity < *freeIndex)
    {
        return NULL;
    }

    if (*freeIndex == *capacity)
    {
        // FIXME: fuck this idk it's unfixable
        return NULL;
        // unsigned int newCapacity = 0;

        // if (*capacity == 0)
        //     newCapacity = DYN_ARR_DEFAULT_CAPACITY;
        // else
        //     newCapacity = *capacity * DYN_ARR_CAPACITY_GROWTH_FACTOR;

        // void* tempArr = realloc(arr, elemSize * newCapacity);
        // if (tempArr == NULL)
        //     return NULL;
        // arr = tempArr;

        // memset((void*)((size_t)arr + elemSize * *capacity), 0,
        //         elemSize * (newCapacity - *capacity));
        // *capacity = newCapacity;
    }

    return (void*)((size_t)arr + elemSize * (*freeIndex)++);
}


void* dynArrCtor(unsigned int* capacity, size_t elemSize)
{
    *capacity = DYN_ARR_DEFAULT_CAPACITY;
    void* arr = calloc(*capacity, elemSize);

    return arr;
}