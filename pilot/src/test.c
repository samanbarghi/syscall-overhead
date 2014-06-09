#include <stdio.h>
#include <unistd.h>

int main(){
    write(0, "Hello, world!\n", 14);
    write(0, "Hello, world!\n", 14);
    printf("This is a test");
    return 0;
}