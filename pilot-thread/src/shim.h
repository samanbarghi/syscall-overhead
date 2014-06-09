#include <pthread.h>
#include "syscall_struct.h"


#ifndef _SHIM_H_
#define _SHIM_H_

//shared memory
syscall_page *sp;
pthread_t server_t;

#endif /* _SHIM_H_ */