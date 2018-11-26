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
  if (mkfifo(pipeName, 0666) < 0) exit(EXIT_FAILURE);

  if ((fserv = open(path, O_WRONLY)) < 0) exit(EXIT_FAILURE);


  while (1) {
    fgets(outputBuffer, BUFFER_SIZE, stdin);

    char* tmp = strdup(outputBuffer);
    strcpy(outputBuffer, pipeName);
    strcat(outputBuffer, " ");
    strcat(outputBuffer, tmp);

    write(fserv, outputBuffer, BUFFER_SIZE);

    if ((fcli = open(pipeName, O_RDONLY)) < 0) exit(EXIT_FAILURE);

    read(fcli, inputBuffer, BUFFER_SIZE);
    printf("%s\n", inputBuffer);
  }


  unlink(pipeName);
  close(fserv);
  close(fcli);

}
