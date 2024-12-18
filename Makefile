CC = gcc
CFLAGS = -Wall -ggdb

OBJECTS_BASE=queue.o ppos-all.o 
OBJECTS_DISK=disk-driver.o ppos-disk-manager.o
SRC = ppos-core-aux.c 

all: semaphore mqueue racecond

test: semaphore-test mqueue-test racecond-test

semaphore: $(OBJECTS_BASE) $(SRC)
	@echo "Compilando $@"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_BASE) $(SRC) ./pingpong-semaphore.c

mqueue: $(OBJECTS_BASE) $(SRC)
	@echo "Compilando $@"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_BASE) $(SRC) ./pingpong-mqueue.c -lm

racecond: $(OBJECTS_BASE) $(SRC)
	@echo "Compilando $@"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_BASE) $(SRC) ./pingpong-racecond.c

racecond-test: racecond
	./racecond

%-test: %
	./$< > $<.txt && diff ./pingpong-$<.txt $<.txt
	@echo "\033[0;32mSUCESSO\033[0m"
	@sleep 3

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f semaphore mqueue racecond
