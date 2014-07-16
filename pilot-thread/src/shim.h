#include <pthread.h>
#include "syscall_struct.h"


#ifndef _SHIM_H_
#define _SHIM_H_

//shared memory
syscall_page *sp;
pthread_t server_t;
FILE *log_fp;

#endif /* _SHIM_H_ */