#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "shim.h"


pthread_t test1_t, test2_t;

void *test_thread(void* threadid){    
	int i;
    for(i = 0; i < 500; ++i) {
    	/* code */
    	char output[128]; 
    	sprintf(output, "Hello, world!%i\n", i);
    	write(0, output, sizeof(output)/sizeof(char*)+1);
    	
    }
	pthread_exit(NULL);

}

int main(){
    
	//Initialize the shim lib
	shim_init();

	int rc = pthread_create(&test1_t, NULL, test_thread, NULL);

	rc = pthread_create(&test2_t, NULL, test_thread, NULL);
    
	pthread_join(test1_t, NULL);    
	pthread_join(test2_t, NULL);
    return 0;
}