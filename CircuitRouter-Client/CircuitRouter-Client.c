#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int main (int argc, char** argv) {
  int fserv;
  char buffer[BUFFER_SIZE];
  char* path = argv[1];

  if ((fserv = open(path, O_WRONLY)) < 0) exit(EXIT_FAILURE);

  while (1) {
    fgets(buffer, BUFFER_SIZE, stdin);
    write(fserv, buffer, BUFFER_SIZE);
  }

  close(fserv);

}
