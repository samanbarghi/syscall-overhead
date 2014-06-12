#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include "shim.h"

void *server_thread(void* threadid){    

    while(1)
    {
        
        //Consumer
        //syscall_entry *se = malloc(sizeof(syscall_entry));

        syscall_entry *se = syscall_page_dequeue_request(sp);       

        //printf("[CONSUMER]--->syscall: %d\n", se->syscall);
        
        if(se->syscall == _SM_SYSCALL_WRITE){
            
                int fd = *((int*) (se->args[0]));
                const void* buf = (const void*) (se->args[1]);
                size_t count = *((size_t*)(se->args[2]));                  

                // printf("[CONSUMER]--->arg[0]->%d\n", fd);
                // printf("[CONSUMER]--->arg[1]->%p\n", buf);
                // printf("[CONSUMER]--->arg[2]->%lu\n", count);
                // printf("[CONSUMER]--->arg[3]->%d\n", index);
                //sleep(1);
                update_entry(sp, se->index, syscall(SYS_write, fd, buf, count));            
        }          

        

    }
    pthread_exit(NULL);
}

void shim_init(){
    //initializeing the sp 
   
    sp = syscall_page_init();    
    
    //Creating the server pthread    
    int rc = pthread_create(&server_t, NULL, server_thread, NULL);
    if(rc){
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    //TODO: place pthread_exit (destructor)


}

long sm_register(syscall_entry* entry){   
     
    //put the request in the queue
    return syscall_page_enqueue_request(sp, entry);

    //wait for the response

}


/***********************/
/*      Wrappers       */
/***********************/
static  ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;

ssize_t write(int fd, const void *buf, size_t count){

    long return_code;
    real_write = dlsym(RTLD_NEXT, "write");
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_WRITE;
    e->num_args     = 3;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    //copy the arguments
    e->args[0] = malloc(sizeof(int));    
    *(int*)(e->args[0]) = fd;
    e->args[1] = malloc(sizeof(count));    
    memcpy(e->args[1], buf, count);
    e->args[2] = malloc(sizeof(size_t));
    *(size_t*)(e->args[2]) = count;
    e->args[3] = NULL;
    e->args[4] = NULL;
    e->args[5] = NULL;    

    // printf("[PRODUCER]--->arg[0]->%d\n", fd);
    // printf("[PRODUCER]--->arg[1]->%p\n", buf);    
    // printf("[PRODUCER]--->arg[2]->%lu\n", count);
    // printf("[PRODUCER]--->arg[3]->%lu\n", index);


    return_code = sm_register(e);
    free(e);

    //printf("Return code: %ld\n", return_code);
    return (ssize_t)return_code;
}



