#include <linux/sched.h>
#include <pthread.h>

#ifndef _SM_SYSCALL_STRUCT_H_
#define _SM_SYSCALL_STRUCT_H_

#define SYSCALL_PAGE_SIZE 128

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
#define _SM_SYSCALL_PUTS 19

typedef int bool;
#define true 1
#define false 0

typedef struct  {
        //unsigned char index;  //1 byte. for test purpose
        unsigned int syscall; //4 bytes
        unsigned int index; //4 bytes
        unsigned char num_args; //1 byte
        unsigned char status; //1 byte
        void* args[6]; //48 bytes
        long return_code; //8 byte
        
} syscall_entry;

typedef struct {
        syscall_entry   entries[SYSCALL_PAGE_SIZE];
        pthread_cond_t  *ready_cond[SYSCALL_PAGE_SIZE]; //condition variable for when the result is ready 

        int             request_head,
                        request_tail, //response_head = request_tail-1
                        response_tail,
                        num_total_entries,
                        num_requests,
                        num_responses;
        pthread_mutex_t *mutex;
        pthread_cond_t  *empty, *full;
        
} syscall_page;

syscall_page    *syscall_page_init();
syscall_entry   *syscall_page_dequeue_request(syscall_page *sp);
long            syscall_page_enqueue_request(syscall_page *sp, syscall_entry *se);
void            update_entry(syscall_page *sp, int index, long return_code);

#endif /* _SM_SYSCALL_STRUCT_H */
