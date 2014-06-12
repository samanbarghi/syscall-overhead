#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "syscall_struct.h"

syscall_page *syscall_page_init(){
    syscall_page *sp = malloc(sizeof(*sp));

    sp->request_head        = 0;
    sp->request_tail        = 0;
    sp->num_total_entries   = 0;
    sp->num_requests        = 0;
    sp->num_responses       = 0;    


    int i = 0;
    for( i = 0; i < SYSCALL_PAGE_SIZE; ++i) {
        sp->ready_cond[i] = malloc(sizeof(pthread_cond_t));
        pthread_cond_init(sp->ready_cond[i], NULL);
    }

    //TODO: check whether this succeed or not != 0 
    sp->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sp->mutex, NULL);

    sp->empty = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(sp->empty, NULL);
    sp->full = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(sp->full, NULL);

    return sp;
}
long syscall_page_enqueue_request(syscall_page *sp, syscall_entry *se){
    assert(sp != NULL);
    assert(se != NULL);

    int tmp_index;
    long return_code;

    //printf("mutex:%p, empty: %p, full: %p\n", sp->mutex, sp->empty, sp->full);
    //acquire the lock    
    
    pthread_mutex_lock(sp->mutex);    

    se->index = sp->request_head;
    while (sp->num_total_entries == SYSCALL_PAGE_SIZE) //queue is full 
        pthread_cond_wait(sp->full, sp->mutex);
    
    //Update the entry
    memcpy(&(sp->entries[sp->request_head]), se, sizeof(syscall_entry));  

    tmp_index = sp->request_head;
    //printf("Enqueue index: %d\n", se->index);

    sp->request_head=(sp->request_head+1) % SYSCALL_PAGE_SIZE;
    sp->num_total_entries++;
    sp->num_requests++;


   
    //signal waiting consumer 
    if(sp->num_requests == 1) pthread_cond_signal(sp->empty);

    //wait for this specific request to be ready 
    while( sp->entries[tmp_index].status != _SM_DONE) pthread_cond_wait(sp->ready_cond[tmp_index], sp->mutex);

    return_code = sp->entries[tmp_index].return_code;
    sp->num_responses--;
    sp->num_total_entries--;
    sp->response_tail++;

    pthread_mutex_unlock(sp->mutex);
    //pthread_mutex_unlock(sp->empty);

    return return_code;
}

syscall_entry *syscall_page_dequeue_request(syscall_page *sp){
    //printf("Consumer\n");    
    assert(sp != NULL);

    pthread_mutex_lock(sp->mutex);

    
    while (sp->num_requests == 0) pthread_cond_wait(sp->empty, sp->mutex);
    //remove from queue
    //memcpy(se, &(sp->entries[sp->request_tail]), sizeof(syscall_entry));
    syscall_entry *se = &(sp->entries[sp->request_tail]);

    sp->request_tail=(sp->request_tail + 1) % SYSCALL_PAGE_SIZE;
    
    //Update number of responses and requests
    sp->num_requests--;
    sp->num_responses++;

    if(sp->num_total_entries == SYSCALL_PAGE_SIZE-1) pthread_cond_signal(sp->full);
    pthread_mutex_unlock(sp->mutex);   

    return se;
}

void *update_entry(syscall_page *sp, int index, long return_code){
    
    pthread_mutex_lock(sp->mutex);
    //printf("dequeue index: %d\n", index);

    sp->entries[index].return_code = return_code;
    sp->entries[index].status = _SM_DONE;

    //result is ready to consume 
    pthread_cond_signal(sp->ready_cond[index]);

    pthread_mutex_unlock(sp->mutex);   
    

}

