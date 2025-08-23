#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_PAGE_SIZE 0x1000  //2^12 -> 0x1000 -> 4096

typedef struct mempool_s {
    int blocksize;
    int freecount;
    char *free_ptr;
    char *mem;
} mempool_t;


// sdk -->
// 2^n, page_size 4096, block_size: 16, 32, 64, 128
int memp_create(mempool_t *m, int block_size) {
    if (!m) return -1;

    m->blocksize = block_size;
    m->freecount = MEM_PAGE_SIZE;

    m->mem = (char *)malloc(MEM_PAGE_SIZE);
    if (!m->mem) return -2;

    memset(m->mem, 0, MEM_PAGE_SIZE);  // Initialize memory to zero

    m->free_ptr = m->mem;

    int i = 0;
    char *ptr = m->mem;
    for (i = 0; i < m->freecount; i++) {
        *(char **)ptr = ptr + block_size;
        ptr += block_size;
    }
    *(char **)ptr = NULL;

    return 0;
}

void memp_destroy(mempool_t *m) {
    if (!m) return;

    free(m->mem);
}

void *memp_alloc(mempool_t *m) {
    if (!m || m->freecount == 0) return NULL;

    void *ptr = m->free_ptr;
    m->free_ptr = *(char **)m->free_ptr;  // Move to next free block
    m->freecount--;

    return ptr;
}

void memp_free(mempool_t *m, void *ptr) {
    *(char**)ptr = m->free_ptr;
    m->free_ptr = (char*)ptr;
    m->freecount++;
}

int main() {
    mempool_t m;
    memp_create(&m, 32);

    void *p1 = memp_alloc(&m);
    printf("memp_alloc: %p\n", p1);

    void *p2 = memp_alloc(&m);
    printf("memp_alloc: %p\n", p2);

    void *p3 = memp_alloc(&m);
    printf("memp_alloc: %p\n", p3);

    memp_free(&m, p2);
    printf("memp_free: %p\n", p2);

    return 0;
}