#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
// #include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include "shim.h"

static  ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;
static  ssize_t (*real_pwrite)(int fd, const void *buf, size_t count, off_t offset) = NULL;
static  ssize_t (*real_read)(int fd, void *buf, size_t count) = NULL;
static  ssize_t (*real_pread)(int fd, void *buf, size_t count, off_t offset) = NULL;
static int (*real_open)(const char *pathname, int flags);
static int (*real_close)(int fd);
static int (*real_puts)(const char* str);
FILE *log_fp;

bool initialized = false;

void *server_thread(void* threadid){    
     time_t t;
     srand((unsigned) time(&t));
    while(1)
    {
        
        //Consumer
        //syscall_entry *se = malloc(sizeof(syscall_entry));


        syscall_entry *se = syscall_page_dequeue_request(sp);       

        fprintf(log_fp, "System Call: %d\n", se->syscall);
        //printf("[CONSUMER]--->syscall: %d\n", se->syscall);
        
        if(se->syscall == _SM_SYSCALL_WRITE){
            
                int fd = *((int*) (se->args[0]));
                const void* buf = (const void*) (se->args[1]);
                size_t count = *((size_t*)(se->args[2]));                  

                real_write = dlsym(RTLD_NEXT, "write");
                update_entry(sp, se->index, real_write(fd, buf, count));
                //update_entry(sp, se->index, syscall(SYS_write, fd, buf, count));            
        }else if(se->syscall == _SM_SYSCALL_PWRITE){
            
                int fd = *((int*) (se->args[0]));
                const void* buf = (const void*) (se->args[1]);
                size_t count = *((size_t*)(se->args[2]));                  
                off_t offset = *((off_t*)(se->args[3]));

                real_pwrite = dlsym(RTLD_NEXT, "pwrite");
                update_entry(sp, se->index, real_pwrite(fd, buf, count, offset));
                //update_entry(sp, se->index, syscall(SYS_write, fd, buf, count));            

        }else if(se->syscall == _SM_SYSCALL_READ){
            
                int fd = *((int*) (se->args[0]));
                void* buf = (void*) (se->args[1]);
                size_t count = *((size_t*)(se->args[2]));                  

                real_read = dlsym(RTLD_NEXT, "read");
                update_entry(sp, se->index, real_read(fd, buf, count));
                // update_entry(sp, se->index, syscall(SYS_read, fd, buf, count));            
        }else if(se->syscall == _SM_SYSCALL_PREAD){           
                int fd = *((int*) (se->args[0]));
                void* buf = (void*) (se->args[1]);
                size_t count = *((size_t*)(se->args[2]));                  
                off_t offset = *((off_t*)(se->args[3]));

                real_pread = dlsym(RTLD_NEXT, "pread");
                update_entry(sp, se->index, real_pread(fd, buf, count, offset));
        }else if(se->syscall == _SM_SYSCALL_OPEN){           

            char* pathname = (char*) (se->args[0]);
            int flags = *( (int*) (se->args[1]));

            real_open = dlsym(RTLD_NEXT, "open");
            update_entry(sp, se->index, real_open(pathname, flags));
        }else if(se->syscall == _SM_SYSCALL_CLOSE){
            int fd = *((int*)(se->args[0]));
            real_close = dlsym(RTLD_NEXT, "close");
            update_entry(sp, se->index, real_close(fd));
        }else if(se->syscall == _SM_SYSCALL_PUTS){
            const char* str = (char*)(se->args[0]);
            real_puts = dlsym(RTLD_NEXT, "puts");
            update_entry(sp, se->index, real_puts(str));
        }

    }
    pthread_exit(NULL);
}

void shim_init(){
    //initializeing the sp 
   
    sp = syscall_page_init();    

    //Creating the log file
    log_fp=fopen("/tmp/flexsc-sm.log", "w");

    
    //Creating the server pthread    
    int rc = pthread_create(&server_t, NULL, server_thread, NULL);
    if(rc){
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    //TODO: place pthread_exit (destructor)


}

long sm_register(syscall_entry* entry){   
     
    if(initialized == false){
        initialized = true;
        shim_init();
        //TODO: is there any other way to handle this? 
    }

    while(sp==NULL); //make sure sp is not NULL

    //put the request in the queue
    return syscall_page_enqueue_request(sp, entry);

    //wait for the response

}


/***********************/
/*      Wrappers       */
/***********************/

ssize_t write(int fd, const void *buf, size_t count){

    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_WRITE;
    e->num_args     = 3;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    //copy the arguments
    e->args[0] = malloc(sizeof(int));    
    *(int*)(e->args[0]) = fd;
    //e->args[1] = malloc(sizeof(char)*count);    
    //memcpy(e->args[1], buf, count);
    e->args[1] = (void*)buf;
    e->args[2] = malloc(sizeof(size_t));
    *(size_t*)(e->args[2]) = count;
    e->args[3] = NULL;
    e->args[4] = NULL;
    e->args[5] = NULL;    

    return_code = sm_register(e);
    free(e);

    //printf("Return code: %ld\n", return_code);
    return (ssize_t)return_code;
}

int puts(const char* str){

    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_PUTS;
    e->num_args     = 1;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    //copy the arguments
    e->args[0] = (void*)str;    
    e->args[1] = NULL;
    e->args[2] = NULL;
    e->args[3] = NULL;
    e->args[4] = NULL;
    e->args[5] = NULL;    

    return_code = sm_register(e);
    free(e);

    //printf("Return code: %ld\n", return_code);
    return (int)return_code;
}
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset){

    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_PWRITE;
    e->num_args     = 4;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    //copy the arguments
    e->args[0] = malloc(sizeof(int));    
    *(int*)(e->args[0]) = fd;
    //e->args[1] = malloc(sizeof(char)*count);    
    //memcpy(e->args[1], buf, count);
    e->args[1] = (void *)buf;
    e->args[2] = malloc(sizeof(size_t));
    *(size_t*)(e->args[2]) = count;
    e->args[3] = malloc(sizeof(off_t));
    *(off_t*)(e->args[3]) = offset;
    e->args[4] = NULL;
    e->args[5] = NULL;    

    return_code = sm_register(e);
    free(e);

    //printf("Return code: %ld\n", return_code);
    return (ssize_t)return_code;
}

ssize_t read(int fd, void *buf, size_t count){

    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_READ;
    e->num_args     = 3;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    //copy the arguments
    e->args[0] = malloc(sizeof(int));    
    *(int*)(e->args[0]) = fd;
    //Second argument is a void* needs to point to same memory locaiton
    e->args[1] = buf;    
    //memcpy(e->args[1], buf, count);
    e->args[2] = malloc(sizeof(size_t));
    *(size_t*)(e->args[2]) = count;
    e->args[3] = NULL;
    e->args[4] = NULL;
    e->args[5] = NULL;    


    return_code = sm_register(e);
    free(e);

    //printf("Return code: %ld\n", return_code);
    return (ssize_t)return_code;
    // return real_read(fd, buf, count);
}
ssize_t pread (int fd, void *buf, size_t count, off_t offset){
    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_PREAD;
    e->num_args     = 4;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    //copy the arguments
    e->args[0] = malloc(sizeof(int));    
    *(int*)(e->args[0]) = fd;
    //Second argument is a void* needs to point to same memory locaiton
    e->args[1] = buf;    
    //memcpy(e->args[1], buf, count);
    e->args[2] = malloc(sizeof(size_t));
    *(size_t*)(e->args[2]) = count;
    e->args[3] = malloc(sizeof(off_t));
    *(off_t*)(e->args[3]) = offset;

    e->args[4] = NULL;
    e->args[5] = NULL;    


    return_code = sm_register(e);
    free(e);

    //printf("Return code: %ld\n", return_code);
    return (ssize_t)return_code;
    // return real_read(fd, buf, count);
}

int open (const char *pathname, int flags){
    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_OPEN;
    e->num_args     = 2;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    e->args[0] = malloc(sizeof(char*));
    e->args[0] = (void*)pathname;
    e->args[1] = malloc(sizeof(int));
    *(int*)(e->args[1]) = flags;
    e->args[2] = NULL;
    e->args[3] = NULL;
    e->args[4] = NULL;
    e->args[5] = NULL; 

    return_code = sm_register(e);
    free(e);


    return (int)return_code;
}

int close(int fd){
    long return_code;
    syscall_entry *e = malloc(sizeof(syscall_entry));

    e->syscall      = _SM_SYSCALL_CLOSE;
    e->num_args     = 1;
    e->return_code  = 0;
    e->status       = _SM_SUBMITTED;    
    e->index        = 0;

    e->args[0] = malloc(sizeof(int)); 
    *(int*)(e->args[0]) = fd;
    e->args[1] = NULL;
    e->args[2] = NULL;
    e->args[3] = NULL;
    e->args[4] = NULL;
    e->args[5] = NULL; 

    return_code = sm_register(e);
    free(e);


    return (int)return_code; 
}
