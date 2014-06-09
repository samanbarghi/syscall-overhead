#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "shim.h"

void *server_thread(void* threadid){    

    while(1)
    {
        
        //Consumer
        syscall_entry *se = malloc(sizeof(syscall_entry));

        syscall_page_dequeue(sp, se);       

        printf("[CONSUMER]--->syscall: %d\n", se->syscall);
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

void sm_register(syscall_entry* entry){   
     
    syscall_page_enqueue(sp, entry);
}


/***********************/
/*      Wrappers       */
/***********************/
static  ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;

ssize_t write(int fd, const void *buf, size_t count){

    real_write = dlsym(RTLD_NEXT, "write");
    //printf("wriiiiiiiiiiiiiiiiiiitee\n");
    syscall_entry e;

    e.syscall = _SM_SYSCALL_WRITE;
    e.num_args = 1;
    e.args[0] = fd;
    e.status = _SM_SUBMITTED;


    sm_register(&e);
    return real_write(fd, buf, count);
}



