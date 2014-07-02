SRCS_DIR:=src
INCLUDE_DIR:=include
BUILD_DIR:=bin


CC=gcc
CFLAGS=-Wall -g
INCLUDE += -I$(INCLUDE_DIR)/
DEPS=$(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/*.c)
CCDEPS=$(wildcard $(INCLUDE_DIR)/*.o)

CCSOURCES=$(wildcard $(INCLUDE_DIR)/*.c $(SRCS_DIR)/*.c)
CCOBJS=$(CCSOURCES:%.c=%.o)


all:  syscall_struct shim libshim.so test


syscall_struct: $(INCLUDE_DIR)/syscall_struct.c
	$(CC) $(CFLAGS) -fPIC -c $(INCLUDE_DIR)/syscall_struct.c -o $(BUILD_DIR)/syscall_struct.o

shim: $(SRCS_DIR)/shim.c
	$(CC) $(CFLAGS) -c -fPIC $(INCLUDE) $(SRCS_DIR)/shim.c -o $(BUILD_DIR)/shim.o -pthread -ldl

libshim.so: $(BUILD_DIR)/syscall_struct.o $(BUILD_DIR)/shim.o
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -shared  -o $(BUILD_DIR)/libshim.so $^  -pthread -ldl

test: $(SRCS_DIR)/test.c
	$(CC) $(CFLAGS) $(INCLUDE) -L/home/saman/Programming/research-flexsc/pilot-thread/bin $(SRCS_DIR)/test.c -o $(BUILD_DIR)/test -lshim -pthread

clean:
	rm -rf bin/* include/*.o