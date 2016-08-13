#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H
#include <stdlib.h>
#include <stdint.h>
#define OBJECT_POOL_DEC(type)\
typedef union OP_##type##Item {\
    type object; \
    uint32_t nextFreeIndex;\
}OP_##type##Item; \
\
typedef struct OP_##type##Handle{\
   uint32_t index;\
   uint32_t counter;\
}OP_##type##Handle;\
\
typedef struct OP_##type {\
    OP_##type##Item *objects;\
    OP_##type##Handle *handles;\
    uint8_t* alive;\
    uint32_t size;\
    uint32_t nextFreeIndex;\
    uint32_t lastFreeIndex;\
    uint32_t counter;\
    uint32_t place;\
    uint32_t capacity;\
}OP_##type;\
\
OP_##type OP_##type##_create(uint32_t capacity);\
OP_##type##Handle OP_##type##_add(OP_##type* pool,type obj);\
type* OP_##type##_get(OP_##type* pool,OP_##type##Handle handle);\
type* OP_##type##_next(OP_##type* pool);\
int OP_##type##_remove(OP_##type* pool,OP_##type##Handle handle);\
void OP_##type##_destroy(OP_##type* pool);
#define OBJECT_POOL_DEF(type)\
OP_##type OP_##type##_create(uint32_t capacity){\
    OP_##type pool;\
    pool.objects = malloc(sizeof(OP_##type##Item)*capacity);\
    pool.handles = malloc(sizeof(OP_##type##Handle)*capacity);\
    pool.alive = malloc(sizeof(uint8_t) * capacity);\
    int i = 0;\
    OP_##type##Handle handle = {0,0};\
    for(i=0;i<capacity-1;i++){\
        pool.objects[i].nextFreeIndex = i+1;\
        pool.handles[i] = handle;\
        pool.alive[i] = 0;\
    }\
    pool.objects[capacity-1].nextFreeIndex = 0;\
    pool.handles[capacity-1] = handle;\
    pool.alive[capacity-1] = 0;\
    pool.size = 0;\
    pool.nextFreeIndex =0;\
    pool.lastFreeIndex =0;\
    pool.counter = 0;\
    pool.place = 0;\
    pool.capacity = capacity;\
    return pool;\
}\
\
OP_##type##Handle OP_##type##_add(OP_##type* pool,type obj){\
    int index = pool->nextFreeIndex;\
    pool->nextFreeIndex = pool->objects[index].nextFreeIndex;\
    pool->objects[index].object = obj;\
    OP_##type##Handle handle = {index,pool->counter};\
    pool->handles[index] = handle;\
    pool->alive[index] = 1;\
    pool->size++;\
    return handle;\
}\
type* OP_##type##_get(OP_##type* pool,OP_##type##Handle handle){\
    if(pool->handles[handle.index].counter == handle.counter && pool->alive[handle.index]){\
        return &pool->objects[handle.index].object;\
    }\
    return NULL;\
}\
type* OP_##type##_next(OP_##type* pool){\
    while(pool->place < pool->capacity){\
        if(pool->alive[pool->place]){\
            return &pool->objects[pool->place++].object;\
        }\
        pool->place++;\
    }\
    pool->place = 0;\
    return NULL;\
}\
int OP_##type##_remove(OP_##type* pool,OP_##type##Handle handle){\
    if(pool->handles[handle.index].counter == handle.counter && pool->alive[handle.index]){\
        pool->alive[handle.index] = 0;\
        pool->objects[pool->lastFreeIndex].nextFreeIndex = handle.index;\
        pool->lastFreeIndex = handle.index;\
        pool->size--;\
        if(!pool->counter){\
            pool->counter++;\
            pool->lastFreeIndex = handle.index;\
            pool->nextFreeIndex = handle.index;\
            return 1;\
        }else{\
            pool->counter++;\
            pool->objects[pool->lastFreeIndex].nextFreeIndex = handle.index;\
            return 1;\
        }\
    }\
    return 0;\
}\

#endif
