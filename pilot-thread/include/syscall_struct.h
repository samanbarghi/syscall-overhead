#include <linux/sched.h>
#include <pthread.h>

#ifndef _SM_SYSCALL_STRUCT_H_
#define _SM_SYSCALL_STRUCT_H_

#define SYSCALL_PAGE_SIZE 64

//Overrride values 
#define _SM_FREE 0
#define _SM_SUBMITTED 1
#define _SM_DONE 2
#define _SM_RESERVED 4

#define _SM_SYSCALL_WRITE 1
#define _SM_SYSCALL_READ 0
#define _SM_SYSCALL_OPEN 2
#define _SM_SYSCALL_CLOSE 3
#define _SM_SYSCALL_PREAD 17
#define _SM_SYSCALL_PWRITE 18

typedef struct  {
        //unsigned char index;  //1 byte. for test purpose
        unsigned int syscall; //4 bytes
        unsigned char num_args; //1 byte
        unsigned char status; //1 byte
        void* args[6]; //48 bytes
        long return_code; //8 byte
} syscall_entry;


typedef struct {
        syscall_entry entries[SYSCALL_PAGE_SIZE];
        int head, tail, numItem;
        pthread_mutex_t *mutex, *empty, *full;
        
} syscall_page;

syscall_page *syscall_page_init();
void syscall_page_dequeue(syscall_page *sp, syscall_entry *se);
void syscall_page_enqueue(syscall_page *sp, syscall_entry *se);



#endif /* _SM_SYSCALL_STRUCT_H */
