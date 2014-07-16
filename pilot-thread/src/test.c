#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include "shim.h"


pthread_t test1_t, test2_t;

void *test_thread(void *threadid)
{
	int *tid = (int *) threadid;
    int fd, output_fd;
    ssize_t read_file;
    char *buf = (char *)malloc(50);


    fd = open ("/tmp/test.txt", O_RDONLY);
    output_fd = open("/tmp/output.test.txt", O_WRONLY | O_CREAT, 0644);

    if (fd == -1){
    	perror("open"); 
    	exit(2);
    }

    char out[10];
    sprintf(out, "\nTID:%d", *tid);

 	pread(fd, buf, 30, 0);
	write(0, buf, 30);
    while((read_file = read(fd, buf, 30)) >0){
    	//write(0, out, 10);
    	write(output_fd, buf, 30);
   		write(0, buf, 30);
    }
    // printf("Thread\n");
    pwrite(output_fd, "\nSSSSSSSSSS\n", 10, 100);


    close(fd);
    sleep(1000000000);
    pthread_exit(NULL);

}

int main()
{

	int first = 1;
	int second = 2;


    int rc = pthread_create(&test1_t, NULL, test_thread, (void*)&first);

    rc = pthread_create(&test2_t, NULL, test_thread, (void*)&second);

    pthread_join(test1_t, NULL);
    pthread_join(test2_t, NULL);

    return 0;
}
