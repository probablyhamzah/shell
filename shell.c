#include "shell.h"

// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).
void parseInput(char* str, char* delim, char** commands)
{
    char* token;
    int i = 0;
    while((token = strsep(&str,delim)) != NULL)
    {
        // Store the string only if it's not empty
        if (strcmp(token, "") != 0)
        {
            strcpy(commands[i], token);
            i++;
        }
            
    }
    commands[i] = NULL;
}

// Splits a command into an array of arguments
// Ex: "ls -a -l" -> {"ls", "-a", "-l"}
char** splitToArgs(char* command)
{
    
    char* delim = strdup(" ");

    char** arguments = (char**) malloc(ARG_SIZE * sizeof(char**));
    for (int i = 0; i < ARG_SIZE; i++)
    {
        arguments[i] = (char*) malloc(SIZE * sizeof(char*));
    }
    
    // Split the string with spaces, to convert the array to arguments.
    parseInput(command, delim, arguments);
    return arguments;
}

// Free the array of arguments.
void freeArgs(char** arguments)
{
    for (int i = 0; i < ARG_SIZE; i++)
    {
        free(arguments[i]);
    }
    free(arguments);
    
}

// This function will fork a new process to execute a command
void executeCommand(char* command, bool isSequential)
{
    char** arguments = splitToArgs(command);

    // Check if the command passed is cd, handle it separately with chdir()
    if (strcmp(arguments[0], "cd") == 0)
    {
        if (chdir(arguments[1]) < 0)
        {
            printf("Shell: Incorrect command\n");
        }
    }
    // Fork and call execvp, passing the arguments array as parameter.
    else 
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            
            if(execvp(arguments[0], arguments) < 0)
            {
                printf("Shell: Incorrect command\n");
            }
            freeArgs(arguments);
            exit(0);
        }
        else if (pid > 0 && isSequential)
        {
            wait(NULL);
        }
        else if (pid < 0)
        {
            printf("Shell: Incorrect command\n");
        }
    }
    freeArgs(arguments);
}

// This function will run multiple commands in parallel    
void executeParallelCommands(char** commands)
{
    for (int i = 0; commands[i] != NULL; i++)
    {
        executeCommand(commands[i], false);
    }

    // Wait for all childs to finish execution before moving on.
    // Not doing so will make the parent execute in parallel as well.
    pid_t id;
    int status = 0;

    while ((id = wait(&status)) > 0);
}

// This function will run multiple commands in parallel
void executeSequentialCommands(char** commands)
{	
    for (int i = 0; commands[i] != NULL; i++)
    {
        executeCommand(commands[i], true);
    }
}

// Trim the spaces from the front and back of a string
char* trimSpaces(char* command)
{
    while(*command == ' ')
    {
        command++;
    }
    char* tail = command + strlen(command) - 1;
    while(tail > command && *tail == ' ')
    {
        tail--;
    }
    
    *(tail + 1) = '\0';

    return command;
}


void executeCommandRedirection(char* command, char* filename)
{
    // This function will run a single command with output redirected to an output file specificed by user

    filename = trimSpaces(filename);

    int fileptr = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    int out = dup(1);
    dup2(fileptr, 1);

    executeCommand(command, true);

    dup2(out, 1);
    
}

