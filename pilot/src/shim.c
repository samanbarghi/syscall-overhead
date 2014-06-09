#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include "header.h"


//shared memory
syscall_page *sp = NULL;

static  ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;

void sm_register(syscall_entry* entry){
    if(sp == NULL)
        sp = shared_syscall_page();

    int n;
    sem_wait(&sp->empty); // Semaphore down operation
    sem_wait(&sp->mutex);
    sem_getvalue(&sp->full,&n);


    (sp->entries)[n].syscall  = entry->syscall;
    (sp->entries)[n].num_args = entry->num_args;
    (sp->entries)[n].args[0]  = entry->args[0];
    (sp->entries)[n].status   = entry->status;

    printf("[PRODUCER] Placed item [%d]\n", n);
    sem_post(&sp->mutex);
    sem_post(&sp->full); //Semaphore up operation

}

ssize_t write(int fd, const void *buf, size_t count){

    real_write = dlsym(RTLD_NEXT, "write");
    printf("wriiiiiiiiiiiiiiiiiiitee\n");
    syscall_entry e;

    e.syscall = _SM_SYSCALL_WRITE;
    e.num_args = 1;
    e.args[0] = fd;
    e.status = _SM_SUBMITTED;


    sm_register(&e);
    return real_write(fd, buf, count);
}



