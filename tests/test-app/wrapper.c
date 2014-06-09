#include <stdio.h>
void *__real_malloc (size_t);

void *__wrap_malloc (size_t size){
    void *lptr = __real_malloc(size);
    printf("Malloc: %lu bytes @%p\n", size, lptr);
    return lptr;
}
