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
  char inputBuffer[BUFFER_SIZE];
  char outputBuffer[BUFFER_SIZE];
  char* path = argv[1];
  char pipeName[32] = "";
  sprintf(pipeName, "Client%d.pipe", getpid());

  unlink(pipeName);
  if (mkfifo(pipeName, 0666) < 0) {
    perror ("Error making client pipe");
    exit(EXIT_FAILURE);
  }

  if ((fserv = open(path, O_WRONLY)) < 0) {
    perror("Failed to open AdvShell pipe");
    exit(EXIT_FAILURE);
  }

  while (1) {
    fgets(outputBuffer, BUFFER_SIZE, stdin);

    char* tmp = strdup(outputBuffer);
    strcpy(outputBuffer, pipeName);
    strcat(outputBuffer, " ");
    strcat(outputBuffer, tmp);

    if (write(fserv, outputBuffer, BUFFER_SIZE) < 0) {
      perror("Failed to write in AdvShell pipe");
    }

    if ((fcli = open(pipeName, O_RDONLY)) < 0) {
      perror("Failed to open client pipe");
      exit(EXIT_FAILURE);
    }

    if (read(fcli, inputBuffer, BUFFER_SIZE) < 0) {
      perror("Failed to read from client pipe");
    }
    printf("%s\n", inputBuffer);
  }


  unlink(pipeName);
  close(fserv);
  close(fcli);

}
