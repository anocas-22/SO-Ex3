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

/* =============================================================================
 * waitForChild
 * -----------------------------------------------------------------------------
 * waits for the child to end getting the pid and the status, finds the child
 * in list children with that pid and adds the status to the child
 * =============================================================================
 */
void waitForChild();

/* =============================================================================
 * printChildren
 * -----------------------------------------------------------------------------
 * prints every child of list children with their pid, status and duration time
 * =============================================================================
 */
void printChildren();

/* =============================================================================
 * readPipeArguments
 * -----------------------------------------------------------------------------
 * reads the arguments given by the clent from the server pipe
 * =============================================================================
 */
int readPipeArguments(int pipe, char **argVector, int vectorSize, char *buffer, int bufferSize);

/* =============================================================================
 * findChild
 * -----------------------------------------------------------------------------
 * searches the list children and returns the child with the given pid
 * =============================================================================
 */
child_t* findChild(int pid);

/* =============================================================================
 * handleChildTime
 * -----------------------------------------------------------------------------
 * handles the signal SIGCHLD by finding the child in the list with the same
 * pid as the one that ended and adding the stopTime to the child
 * =============================================================================
 */
void handleChildTime(int sig, siginfo_t *si, void *context);

/* =============================================================================
 * messageClient
 * -----------------------------------------------------------------------------
 * opens the client pipe to write a message in it
 * =============================================================================
 */
void messageClient(char* pipeName, char* message);

/* =============================================================================
 * signalChildEnd
 * -----------------------------------------------------------------------------
 * prepares parent process to receive a signal when the child ends
 * =============================================================================
 */
void signalChildEnd();

/* =============================================================================
 * openPipe
 * -----------------------------------------------------------------------------
 * creates and opens pipe with the given name; returns the created pipe
 * =============================================================================
 */
int openPipe(char* pipeName);

/* =============================================================================
 * closePipe
 * -----------------------------------------------------------------------------
 * closes and unlinks the given pipe
 * =============================================================================
 */
void closePipe(int p, char* pipeName);

#endif /* CIRCUITROUTER_SHELL_H */
