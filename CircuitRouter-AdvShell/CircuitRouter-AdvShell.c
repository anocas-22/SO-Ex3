
/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "lib/timer.h"
#include "CircuitRouter-AdvShell.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"
#define PIPENAME "AdvShell.pipe"

#define MAXARGS 3
#define BUFFER_SIZE 100

vector_t *children;

void waitForChild() {
    while (1) {
        int pid, status;
        pid = wait(&status);
        if (pid < 0) {
            if (errno == EINTR) {
                /* This error means that the signal was received and interrupted
                the wait for the end of the child process, so we wait again*/
                free(child);
                continue;
            } else {
                perror("Unexpected error while waiting for child.");
                exit (EXIT_FAILURE);
            }
        }
        child_t* child;
        child = findChild(pid);
        child->status = status;
        return;
    }
}

void printChildren() {
    for (int i = 0; i < vector_getSize(children); ++i) {
        child_t *child = vector_at(children, i);
        int status = child->status;
        pid_t pid = child->pid;
        double duration = TIMER_DIFF_SECONDS(child->startTime, child->stopTime);
        if (pid != -1) {
            const char* ret = "NOK";
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                ret = "OK";
            }
            printf("CHILD EXITED: (PID=%d; return %s; %f s)\n", pid, ret, duration);
        }
    }
    puts("END.");
}

int readPipeArguments(int pipe, char **argVector, int vectorSize, char *buffer, int bufferSize) {
  int numTokens = 0;
  char *s = " \r\n\t";
  int i;
  char *token;

  if (argVector == NULL || buffer == NULL || vectorSize <= 0 || bufferSize <= 0)
     return 0;

  if (read(pipe, buffer, bufferSize) < 0) {
    perror("Failed to read from server pipe");
    return -1;
  }

  /* get the first token */
  token = strtok(buffer, s);

  /* walk through other tokens */
  while( numTokens < vectorSize-1 && token != NULL ) {
    argVector[numTokens] = token;
    numTokens ++;

    token = strtok(NULL, s);
  }

  for (i = numTokens; i<vectorSize; i++) {
    argVector[i] = NULL;
  }

  return numTokens;
}

child_t* findChild(int pid) {
  for (int i = 0; i < vector_getSize(children); i++) {
    child_t* child = vector_at(children, i);
    if (child->pid == pid)
      return child;
  }
  return NULL;
}

void handleChildTime(int sig, siginfo_t *si, void *context) {
  TIMER_T stopTime;
  TIMER_READ(stopTime);
  child_t* child = findChild(si->si_pid);

  if (child != NULL)
    if(si->si_code == CLD_EXITED || si->si_code == CLD_KILLED)
      child->stopTime = stopTime;

  signalChildEnd();
}

void messageClient(char* pipeName, char* message) {
  int fcli;
  if ((fcli = open(pipeName, O_WRONLY)) < 0) {
    perror("Failed to open client pipe");
    exit(EXIT_FAILURE);
  }
  if (write(fcli, message, 22) < 0) {
    perror("Failed to write in client pipe");
  }
  close(fcli);
}

void signalChildEnd() {
  struct sigaction options;
  options.sa_flags = SA_SIGINFO;
  options.sa_sigaction = handleChildTime;
  if (sigaction(SIGCHLD, &options, NULL) == -1) {
    perror("Sigaction failed");
  }
}

int openPipe(char* pipeName) {
  int p;
  unlink(pipeName);

  if (mkfifo(pipeName, 0666) < 0) {
    perror("Error making server pipe");
    exit(EXIT_FAILURE);
  }

  if ((p = open(pipeName, O_RDONLY|O_NONBLOCK)) < 0) {
    perror("Failed to open server pipe");
    exit(EXIT_FAILURE);
  }
  return p;
}

void closePipe(int p, char* pipeName) {
  close(p);
  unlink(pipeName);
}

int main (int argc, char** argv) {
    int fserv, result, maxDescriptor;
    bool_t fromStdin = FALSE;
    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    int MAXCHILDREN = -1;
    int runningChildren = 0;

    if (argv[1] != NULL){
        MAXCHILDREN = atoi(argv[1]);
    }

    children = vector_alloc(MAXCHILDREN);

    printf("Welcome to CircuitRouter-AdvShell\n\n");

    fserv = openPipe(PIPENAME);

    //Sets up reading from select()
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(fileno(stdin), &readset);
    FD_SET(fserv, &readset);

    maxDescriptor = fileno(stdin) > fserv ? fileno(stdin) : fserv;

    signalChildEnd();
    
    while (1) {
        int numArgs;
        char ClientPath[64];

        //Receives input from stdin or fserv with select()
        result = select(maxDescriptor+1, &readset, NULL, NULL, NULL);

        if(result == -1) {
            perror("Select failed");
            continue;
        }
        else if(result) {
          if (FD_ISSET(fileno(stdin), &readset)) {
            numArgs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);
            strcpy(ClientPath, "");
            fromStdin = TRUE;
          }
          if (FD_ISSET(fserv, &readset)) {
            char* temp[MAXARGS+2];
            numArgs = readPipeArguments(fserv, temp, MAXARGS+2, buffer, BUFFER_SIZE);
            if (--numArgs != 0) {
              for (int i = 0; i < MAXARGS + 1; i++)
                args[i] = temp[i+1];
            }
            strcpy(ClientPath, "../CircuitRouter-Client/");
            strcat(ClientPath, temp[0]);
            fromStdin = FALSE;
          }
          FD_SET(fileno(stdin), &readset);
          FD_SET(fserv, &readset);
        } else
          continue;

        /* EOF (end of file) of stdin or command "exit"*/
        if (numArgs < 0 || (fromStdin && numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0))) {
            printf("CircuitRouter-AdvShell will exit.\n--\n");

            /* Waits for all children to end */
            while (runningChildren > 0) {
                waitForChild();
                runningChildren --;
            }

            printChildren();
            printf("--\nCircuitRouter-AdvShell ended.\n");
            break;
        }

        else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0){
            int pid;
            if (numArgs < 2) {
                if (!fromStdin) {
                  messageClient(ClientPath, "Command not supported");
                } else {
                  printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
                }
                continue;
            }
            if (MAXCHILDREN != -1 && runningChildren >= MAXCHILDREN) {
                waitForChild();
                runningChildren--;
            }


            pid = fork();
            if (pid < 0) {
                perror("Failed to create new process.");
                exit(EXIT_FAILURE);
            }

            if (pid > 0) {
                //start time and add child to vector children
                TIMER_T startTime;
                TIMER_READ(startTime);
                child_t *child = malloc(sizeof(child_t));
                if (child == NULL) {
                    perror("Error allocating memory");
                    exit(EXIT_FAILURE);
                }
                child->pid = pid;
                child->startTime = startTime;
                vector_pushBack(children, child);

                runningChildren++;
                printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
                continue;

            } else {
                char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
                char *newArgs[4] = {seqsolver, args[1], ClientPath, NULL};

                execv(seqsolver, newArgs);
                perror("Error while executing child process"); // Shouldn't get here
                exit(EXIT_FAILURE);
            }
        } else {
          if (!fromStdin) {
            messageClient(ClientPath, "Command not supported");
          } else {
            printf("Unknown command. Try again.\n");
          }
        }

    }

    closePipe(fserv, PIPENAME);

    for (int i = 0; i < vector_getSize(children); i++) {
        free(vector_at(children, i));
    }
    vector_free(children);

    return EXIT_SUCCESS;
}
