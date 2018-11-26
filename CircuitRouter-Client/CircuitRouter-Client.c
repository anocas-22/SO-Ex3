#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int main (int argc, char** argv) {
  int fserv, fcli;
  char buffer[BUFFER_SIZE];
  char* path = argv[1];
  char pipeName[32];
  char* pid = (char*) malloc(32*sizeof(char));
  sprintf(pid, "%d", getpid());
  strcat(pipeName, "/tmp/Client");
  strcat(pipeName, pid);
  strcat(pipeName, ".pipe ");

  unlink(pipeName);
  if (mkfifo(pipeName, 0666) < 0) exit(EXIT_FAILURE);

  if ((fserv = open(path, O_WRONLY)) < 0) exit(EXIT_FAILURE);

  while (1) {
    fgets(buffer, BUFFER_SIZE, stdin);

    char* tmp = strdup(buffer);
    strcpy(buffer, pipeName);
    strcat(buffer, tmp);

    write(fserv, buffer, BUFFER_SIZE);

    printf("1\n");
    printf("%s\n", pipeName);
    if ((fcli = open(pipeName, O_RDONLY)) < 0) {
      perror("Failed to open Client pipe");
      exit(EXIT_FAILURE);
    }

    printf("2\n");

    read(fcli, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
  }


  unlink(pipeName);
  close(fserv);
  close(fcli);

}
