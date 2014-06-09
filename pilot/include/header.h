#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>

#include "spec.h"

#ifndef _SM_HEADER_H_
#define _SM_HEADER_H_
#define SM_SHMKEY 1020

syscall_page *shared_syscall_page(){
    key_t key = SM_SHMKEY;
    int shmid;
    size_t shmsize = sizeof(syscall_page);
    shmid = shmget(key, shmsize, IPC_CREAT | 0666);
    printf("ID:%d\n", shmid);

    syscall_page *sp= shmat(shmid, NULL, 0);
    //make sure the shared memory segment is created successfully
    assert(sp != (void *) -1);
    printf("Mem struct address:%p\n", sp);

    return sp;
}

void init(){
    syscall_page *sp = shared_syscall_page();

    sem_init(&(sp->mutex),1,1);
    sem_init(&(sp->empty),1,SYSCALL_PAGE_SIZE);
    sem_init(&(sp->full),1,0);
}

#endif /* _SM_HEADER_H */
