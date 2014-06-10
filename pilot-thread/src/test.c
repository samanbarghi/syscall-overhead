#include <stdio.h>
#include <unistd.h>
#include "shim.h"

int main(){
    
	//Initialize the shim lib
	shim_init();

    int i;
    for(i = 0; i < 50; ++i) {
    	/* code */
    	write(0, "Hello, world!\n", 14);
    	sleep(5);
    }
    
    
    //printf("This is a test");
    return 0;
}