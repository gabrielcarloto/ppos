CC = gcc
CFLAGS = -Wall -ggdb

OBJECTS_BASE=queue.o ppos-all.o 
OBJECTS_DISK=disk-driver.o ppos-disk-manager.o
SRC = ppos-core-aux.c 

all: semaphore mqueue racecond

semaphore: $(OBJECTS_BASE) $(SRC)
	@echo "\n\nCompilando $@\n\n"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_BASE) $(SRC) ./pingpong-semaphore.c

mqueue: $(OBJECTS_BASE) $(SRC)
	@echo "\n\nCompilando $@\n\n"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_BASE) $(SRC) ./pingpong-mqueue.c -lm

racecond: $(OBJECTS_BASE) $(SRC)
	@echo "\n\nCompilando $@\n\n"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_BASE) $(SRC) ./pingpong-racecond.c

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f semaphore mqueue racecond
