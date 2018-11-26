#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int main (int argc, char** argv) {
  int fserv/*, fcli*/;
  char buffer[BUFFER_SIZE];
  char* path = argv[1];
  char pipeName[32] = "";
  char* pid = (char*) malloc(32*sizeof(char));
  sprintf(pipeName, "Client%d.pipe", getpid());

  unlink(pipeName);
  if (mkfifo(pipeName, 0666) < 0) exit(EXIT_FAILURE);

  if ((fserv = open(path, O_WRONLY)) < 0) exit(EXIT_FAILURE);

  if ((fcli = open(pipeName, O_RDONLY|O_NONBLOCK)) < 0) exit(EXIT_FAILURE);

  while (1) {
    fgets(buffer, BUFFER_SIZE, stdin);

    char* tmp = strdup(buffer);
    sprintf(buffer, "%s%s", buffer, pipeName);
    //strcpy(buffer, pipeName);
    //strcat(buffer, tmp);

    write(fserv, buffer, BUFFER_SIZE);

    read(fcli, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
  }


  unlink(pipeName);
  close(fserv);
  close(fcli);

}
