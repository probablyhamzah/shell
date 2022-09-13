#include "shell.h"
int main()
{
    // Initial declarations
    size_t size = 100;
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    while(1)    // This loop will keep your shell running until user exits.
    {
        char currDir[size];
        getcwd(currDir, size);
            
        // Print the prompt in format - currentWorkingDirectory$
        printf("%s$", currDir);
        
        // accept input with 'getline()'
        char* input = (char*) malloc(size);
        getline(&input, &size, stdin);

        /*
         Getline by default keeps \n in the input string as well.
         We will remove it
        */
        input[strcspn(input, "\n")] = 0;
        
        if(strcmp(input, "exit") == 0)	// When user uses exit command.
        {
            printf("Exiting shell...\n");
            break;
        }

        
        // Initialise the array of delimiters
         
        char** delims = (char**) malloc(DELIM_SIZE * sizeof(char**));
        delims[0] = strdup("&&");
        delims[1] = strdup("##");
        delims[2] = strdup(">");

        /*
          We keep an array of arrays of strings, one array of strings for each of 
          &&, ## and >. First we split the input keeping each of these as a delimiter
          and store it in categories[i]. If the input contains the given delimiter, then 
          the output of parseInput() will contain more than one string. We can use this
          to determine which delimiter the input contains in a general way.
        */
        char*** categories = (char***) malloc(DELIM_SIZE * sizeof(char***));

        // Allocate memory to the 'categories' array...
        for (int i = 0; i < DELIM_SIZE; i++)
        {
            categories[i] = (char**) malloc(size * sizeof(char**));
            for (int j = 0; j < size; j++)
            {
                categories[i][j] = (char*) malloc(size * sizeof(char*));
            }
        }

        // ...and parse input with 'strsep()' for different symbols (&&, ##, >).
        for (int i = 0; i < DELIM_SIZE; i++)
        {
            parseInput(input, delims[i], categories[i]);
        }
        
        /*
          As previously mentioned, the output of parseInput will contain more than one
          string if the delimiter exists in the string, otherwise it will keep the input
          in categories[i][0], and categories[i][1] would be NULL.
          Therefore, if categories[i][1] is not NULL, then the corresponding delimiter exists
          in the string and we can proceed to perform the operations accordingly.
         */
        if(categories[0][1] != NULL)
        {
            executeParallelCommands(categories[0]);      // This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
        }
        else if(categories[1][1] != NULL)
        {
            executeSequentialCommands(categories[1]);    // This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
        }
        else if(categories[2][1] != NULL)
        {
            executeCommandRedirection(categories[2][0], categories[2][1]);    // This function is invoked when user wants redirect output of a single command to and output file specificed by user
        }
        else
        {
            executeCommand(categories[2][0], true);        // This function is invoked when user wants to run a single commands
        }

        // Free the arrays
        for (int i = 0; i < DELIM_SIZE; i++)
        {
            for (int j = 0; j < size; j++)
            {
                free(categories[i][j]);
            }
            free(categories[i]);
        }
        free(categories);

        free(input);
        free(delims);
	}

    return 0;
}
