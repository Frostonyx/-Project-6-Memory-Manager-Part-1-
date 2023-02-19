#include <sys/mman.h>
#include <stddef.h>
#include <stdio.h>

#define BLOCK_SIZE sizeof(struct block)
#define PADDED_SIZE(size) ((size + 7) & ~7)

struct block {
    struct block *next;
    int size;
    int in_use;
};

static struct block *head = NULL;
static void *heap_end = NULL;

void *myalloc(int size) {
    if (size <= 0) {
        return NULL;
    }

    // If this is the first call, mmap() to get some space and build the linked list node
    if (head == NULL) {
        void *heap = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        if (heap == MAP_FAILED) {
            return NULL;
        }

        head = (struct block*)heap;
        head->next = NULL;
        head->size = 1024 - BLOCK_SIZE;
        head->in_use = 0;
        heap_end = heap + 1024;
    }

    // Find the first block that is not in-use and has enough space
    struct block *curr = head;
    while (curr != NULL) {
        if (!curr->in_use && (curr->size >= PADDED_SIZE(size))) {
            // Mark the block as in-use and return a pointer to the user data
            curr->in_use = 1;
            void *data = (void*)((char*)curr + BLOCK_SIZE);
            return data;
        }
        curr = curr->next;
    }

    // No block found, return NULL
    return NULL;
}

void print_data(void)
{
    struct block *b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {

        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

void main(){
    void *p;

    print_data();
    p = myalloc(16);
    print_data();
    p = myalloc(16);
    printf("%p\n", p);
}