# System Call Overhead

[FlexSC](https://www.usenix.org/conference/osdi10/flexsc-flexible-system-call-scheduling-exception-less-system-calls) provides a solution to avoid the over head of system calls (caused by cache misses and flushes of CPU pipeline) 
by moving the exeucution of system calls to another core and batching system calls. 

This project is a navie take on this idea by trying to rewrite specific system calls and run them on another thread using a shim library. 
All system calls are rewritten to be sent to a server thread to execute them and return the result. However, the overhead of synchronization in this case is 
more than the benefits of avoididng the system call. 



