#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include "syscall_struct.h"

syscall_page *syscall_page_init(){
    syscall_page *sp = malloc(sizeof(*sp));

    sp->head    = 0;
    sp->tail    = 0;
    sp->numItem = 0;

    //TODO: check whether this succeed or not != 0 
    sp->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sp->mutex, NULL);
    sp->empty = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sp->empty, NULL);
    sp->full = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sp->full, NULL);

    return sp;
}
void syscall_page_enqueue(syscall_page *sp, syscall_entry *se){
    assert(sp != NULL);
    assert(se != NULL);

    printf("mutex:%p, empty: %p, full: %p\n", sp->mutex, sp->empty, sp->full);
    //acquire the lock    
    while (sp->numItem == SYSCALL_PAGE_SIZE) pthread_mutex_lock(sp->full);
    pthread_mutex_lock(sp->mutex);
    
    //Update the entry
    (sp->entries[sp->head]).syscall = se->syscall;    
    //update atomic variables
    sp->head=(sp->head+1)%SYSCALL_PAGE_SIZE;
    sp->numItem++;
    pthread_mutex_unlock(sp->mutex);
    pthread_mutex_unlock(sp->empty);
}

void syscall_page_dequeue(syscall_page *sp, syscall_entry *se){
     printf("Consumer\n");
    while (sp->numItem == 0) pthread_mutex_lock(sp->empty);
    pthread_mutex_lock(sp->mutex);

    //remove from queue
    se->syscall = (sp->entries[sp->tail]).syscall;

    sp->tail=(sp->tail + 1) % SYSCALL_PAGE_SIZE;
    sp->numItem--;
    pthread_mutex_unlock(sp->mutex);
    //TODO: this causes a futex syscall to kernel
    pthread_mutex_unlock(sp->empty);

}

