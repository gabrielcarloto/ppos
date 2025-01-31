#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main() {
  const int BLOCKSIZE = 64, NUM_BLOCKS = 256;
  int fd = open("./disk.dat", O_RDWR | O_SYNC);
  unsigned char *buffer = NULL;

  buffer = malloc(BLOCKSIZE);

  for (int i = 0; i < NUM_BLOCKS; i++) {
    lseek(fd, i * BLOCKSIZE, SEEK_SET);
    read(fd, buffer, BLOCKSIZE);

    for (int j = 0; j < BLOCKSIZE; j++) {
      printf("%c", buffer[j]);
    }
  }

  return 0;
}
