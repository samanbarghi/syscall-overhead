#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include "shim.h"

ssize_t ready_queue[SYSCALL_PAGE_SIZE];
int ready_queue_index; 

void *server_thread(void* threadid){    

    while(1)
    {
        
        //Consumer
        syscall_entry *se = malloc(sizeof(syscall_entry));

        syscall_page_dequeue(sp, se);       

        printf("[CONSUMER]--->syscall: %d\n", se->syscall);
        
        if(se->syscall == _SM_SYSCALL_WRITE){
            
                int fd = *((int*) (se->args[0]));
                const void* buf = (const void*) (se->args[1]);
                size_t count = *((size_t*)(se->args[2]));   

                int index = *((int*) (se->args[3]));    

                printf("[CONSUMER]--->arg[0]->%d\n", fd);
                printf("[CONSUMER]--->arg[1]->%p\n", buf);
                printf("[CONSUMER]--->arg[2]->%lu\n", count);
                printf("[CONSUMER]--->arg[3]->%d\n", index);

                ready_queue[index] = syscall(SYS_write, fd, buf, count);
            
        }          

        

    }
    pthread_exit(NULL);
}

void shim_init(){
    //initializeing the sp 
   
    sp = syscall_page_init();

    //Remove this
    int i = 0; 
    for(i = 0; i < SYSCALL_PAGE_SIZE; i++)
        ready_queue[i] = NULL;
    ready_queue_index = 0;
    
    //Creating the server pthread    
    int rc = pthread_create(&server_t, NULL, server_thread, NULL);
    if(rc){
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    //TODO: place pthread_exit (destructor)


}

void sm_register(syscall_entry* entry){   
     
    syscall_page_enqueue(sp, entry);
}


/***********************/
/*      Wrappers       */
/***********************/
static  ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;

ssize_t write(int fd, const void *buf, size_t count){

    real_write = dlsym(RTLD_NEXT, "write");
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall = _SM_SYSCALL_WRITE;
    e->num_args = 3;
    

    //copy the arguments
    e->args[0] = malloc(sizeof(int));    
    *(int*)(e->args[0]) = fd;
    e->args[1] = malloc(sizeof(count));    
    memcpy(e->args[1], buf, count);
    e->args[2] = malloc(sizeof(size_t));
    *(size_t*)(e->args[2]) = count;

    int index = ready_queue_index;
    //TODO: remove this and ready_queue
    e->args[3] = malloc(sizeof(int)); 
    *(int*)(e->args[3]) = index;   

    ready_queue_index++;
    e->status = _SM_SUBMITTED;

    printf("[PRODUCER]--->arg[0]->%d\n", fd);
    printf("[PRODUCER]--->arg[1]->%p\n", buf);    
    printf("[PRODUCER]--->arg[2]->%lu\n", count);
    printf("[PRODUCER]--->arg[3]->%lu\n", index);




    sm_register(e);
    free(e);

    while(ready_queue[index] == NULL){
        //wait
    }

    return ready_queue[index];
}



