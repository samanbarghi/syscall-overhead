SRCS_DIR:=src
INCLUDE_DIR:=include
BUILD_DIR:=bin

CC=gcc
CFLAGS=
INCLUDE += -I$(INCLUDE_DIR)/
DEPS=$(INCLUDE_DIR)/spec.h $(INCLUDE_DIR)/header.h

all: consumer test shim

consumer: $(SRCS_DIR)/consumer.c $(DEPS)
	$(CC) $(INCLUDE) $(SRCS_DIR)/consumer.c -o $(BUILD_DIR)/consumer -pthread

shim: $(SRCS_DIR)/shim.c $(DEPS)
	$(CC) -Wall -fPIC -shared  $(INCLUDE) $(SRCS_DIR)/shim.c -o $(BUILD_DIR)/shim.so  -pthread -ldl

test: $(SRCS_DIR)/test.c
	$(CC) $(SRCS_DIR)/test.c -o $(BUILD_DIR)/test

clean:
	rm -rf bin/*