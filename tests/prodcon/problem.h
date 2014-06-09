#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <assert.h>

#define BUFFER_SIZE 10
#define CONSUMER_SLEEP_SEC 3
#define PRODUCER_SLEEP_SEC 1
#define KEY 1011

//A structure to store BUFER and semaphores for synchronization
typedef struct
{
    int buff[BUFFER_SIZE];
    sem_t mutex, empty, full;

} MEM;

//Method for shared memory allocation
MEM *memory()
{
    key_t key = KEY;
    int shmid;
    size_t shmsize = sizeof(MEM);
    shmid = shmget(key, shmsize, IPC_CREAT | 0666);
    printf("ID:%d\n", shmid);

    MEM *m = shmat(shmid, NULL, 0);
    //make sure the shared memory segment is created successfully
    assert(m != (void *) -1);
    printf("Mem struct address:%p\n", m);

    return m;
}

void init()
{
    //Initialize structure pointer with shared memory
    MEM *M = memory();

    //Initialize semaphores
    sem_init(&(M->mutex),1,1);
    sem_init(&(M->empty),1,BUFFER_SIZE);
    sem_init(&(M->full),1,0);
}