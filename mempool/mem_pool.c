#include <stdlib.h>
#include <string.h>

// small memory pool allocation
typedef struct mp_node_s {
    unsigned char *last;
    unsigned char *end;
    struct mp_node_s *next;
}mp_node_t;

// large memory pool allocation
typedef struct mp_large_s {
    struct mp_large_s *next;
    void *alloc;
} mp_large_t;

typedef struct mp_pool_s {
    size_t max;
    struct mp_node_s *head;
    struct mp_large_s *large;
}mp_pool_t;


int mp_create(mp_pool_t *pool, size_t size);
void mp_destroy(mp_pool_t *pool);
void *mp_alloc(mp_pool_t *pool, size_t size);
void mp_free(mp_pool_t *pool, void *ptr);


// size: 4096
int mp_create(mp_pool_t *pool, size_t size) {
    if (!pool || size <= 0) {
        return -1;
    }

    void *mem = malloc(size);

    struct mp_node_s *node = (struct mp_node_s *)mem;
    node->last = (char *)mem + sizeof(struct mp_node_s);
    node->end = (char *)mem + size;
    node->next = NULL;

    pool->head = node;
    pool->max = size;
    pool->large = NULL;

    return 0;
}

// destroy small and large memory pools
// free all memory allocated
// free all nodes in small memory pool
// free all large allocations
// free the pool itself
void mp_destroy(mp_pool_t *pool) {
    // free large allocations   
    mp_large_t *l;
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }
    pool->large = NULL;

    mp_node_t *node = pool->head;
    while (node) {
        mp_node_t *tmp = node->next;
        free(node);
        node = tmp;
    }
    pool->head = NULL;
    free(pool);
}

// allocate a new block for small memory pool
// add the new block to the end of the linked list
// return pointer to the allocated memory
static void *mp_alloc_block(mp_pool_t *pool, size_t size) {
    if (!pool || size <= 0) {
        return NULL;
    }

    // allocate a new block
    void *mem = malloc(size);
    struct mp_node_s *node = (struct mp_node_s *)mem;
    node->last = (char *)mem + sizeof(struct mp_node_s);
    node->end = (char *)mem + size;
    node->next = NULL;

    // allocate memory from the new block
    void *ptr = node->last;
    node->last += size;

    mp_node_t *iter = pool->head;
    while (iter->next) {
        iter = iter->next;
    }
    iter->next = node;

    return ptr;
}

// allocate large memory
// if there is a free large allocation, use it
// otherwise, create a new large allocation and add it to the front of the linked list
static void *mp_alloc_large(mp_pool_t *pool, size_t size) {
    if (!pool) {
        return NULL;
    }

    void *ptr = malloc(size);
    if (!ptr) {
        return NULL;
    }

    // if there is a free large allocation, use it
    mp_large_t *l;
    for (l = pool->large; l; l = l->next) {
        if (l->alloc == NULL) {
            l->alloc = ptr;
            return ptr;
        }
    }

    // create a new large allocation and add it to the front of the linked list
    l = mp_alloc(pool, sizeof(mp_large_t));
    if (!l) {
        free(ptr);
        return NULL;
    }
    l->alloc = ptr;
    l->next = pool->large;
    pool->large = l;

    return ptr;
}


void *mp_alloc(mp_pool_t *pool, size_t size) {
    // allocate from large memory pool
    if (size > pool->max) {
        return mp_alloc_large(pool, size);
    }

    // size <= pool->max
    // allocate from small memory pool
    mp_node_t *node = pool->head;
    void *ptr = NULL;

    // find a node with enough space
    do {
        if (node->end - node->last > size) {
            ptr = node->last;
            node->last += size;
            return ptr;
        }
        node = node->next;
    } while (node);
 
    // no enough space in current node, allocate a new block
    return mp_alloc_small(pool, size);
}

// free large memory allocation
// if ptr is found in large allocations, free it and set alloc to NULL
void mp_free(mp_pool_t *pool, void *ptr) { 
    mp_large_t *l;
    for (l = pool->large; l; l = l->next) {
        if (l->alloc == ptr) {
            l->alloc = NULL;
            return;
        }
    }
    
    // 
}

int main() {

}