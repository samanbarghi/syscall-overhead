#include <header.h>

void consumer()
{
    int n;
    syscall_page *sp = shared_syscall_page();

    while(1)
    {

        sem_wait(&sp->full); // Semaphore down operation
        sem_wait(&sp->mutex); // Semaphore for mutual exclusion
        sem_getvalue(&sp->full,&n); //Assign value of semphore full, to integer n
        printf("[CONSUMER] Removed item [%d]\n", (sp->entries)[n].syscall);
        printf("[CONSUMER]              [%d]->num_args\n", (sp->entries)[n].num_args);
        sem_post(&sp->mutex); // Mutex up operation
        sem_post(&sp->empty); // Semaphore up operation
    }
}

int main()
{
    init();
    consumer();
}
