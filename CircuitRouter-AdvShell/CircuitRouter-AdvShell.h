#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "lib/vector.h"
#include <sys/types.h>
#include <signal.h>

typedef struct {
    pid_t pid;
    int status;
    TIMER_T startTime;
    TIMER_T stopTime;
} child_t;

void waitForChild();
void printChildren();
int readPipeArguments(int pipe, char **argVector, int vectorSize, char *buffer, int bufferSize);
child_t* findChild(int pid);
void handleChildTime(int sig, siginfo_t *si, void *context);
void messageClient(char* pipeName, char* message);

#endif /* CIRCUITROUTER_SHELL_H */
