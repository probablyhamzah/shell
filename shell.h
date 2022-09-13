#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>	        // exit()
#include <unistd.h>	        // fork(), getpid(), exec()
#include <sys/wait.h>       // wait()
#include <signal.h>	        // signal()
#include <fcntl.h>          // close(), open()

#define ARG_SIZE 10
#define DELIM_SIZE 3
#define SIZE 100

void parseInput(char* str, char* delim, char** commands);
char** splitToArgs(char* command);
void freeArgs(char** arguments);
void executeCommand(char* command, bool isSequential);
void executeParallelCommands(char** commands);
void executeSequentialCommands(char** commands);
char* trimSpaces(char* command);
void executeCommandRedirection(char* command, char* filename);
