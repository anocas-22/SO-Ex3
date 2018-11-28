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
 * waits for a child process to finish and adds it's status to the children vector
 * =============================================================================
 */
void waitForChild();

/* =============================================================================
 * printChildren
 * -----------------------------------------------------------------------------
 * prints every child on the children vector with their pid, status and duration
 * =============================================================================
 */
void printChildren();

/* =============================================================================
 * readPipeArguments
 * -----------------------------------------------------------------------------
 * reads the arguments given by a pipe
 * =============================================================================
 */
int readPipeArguments(int pipe, char **argVector, int vectorSize, char *buffer, int bufferSize);

/* =============================================================================
 * findChild
 * -----------------------------------------------------------------------------
 * searches the vector children and returns the child with the given pid
 * =============================================================================
 */
child_t* findChild(int pid);

/* =============================================================================
 * handleChildTime
 * -----------------------------------------------------------------------------
 * handles the signal SIGCHLD by adding the stopTime of the terminating child
 * to the children vector
 * =============================================================================
 */
void handleChildTime(int sig, siginfo_t *si, void *context);

/* =============================================================================
 * messageClient
 * -----------------------------------------------------------------------------
 * opens the given pipe and writes the message on it
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
 * creates and opens a pipe with the given name; returns the created pipe
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
