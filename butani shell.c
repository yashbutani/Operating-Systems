#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void main(); // I need a main method to run everything 
void parseInput(); //method to parse input and call operations 
int executeIF(); //if statemnet code 
int executeCD(); //cd code 
void leave(); //handle exit
int redirection(); //redirection code 
void throwError(); //throw error 
void handlePath(); //path command implementation
void handleCommands(); //all other commands not manually implemented in this project

int batchMode = 0; //bactch mode not enabled by default
char *path[1024] = {"/bin",NULL}; 
int path_elem = 1;
int counter = 0;
char *arr[1024];

void handlePath()
{
    //printf("enters path method\n"); 
    for(int i = 0; i<path_elem;i++)
    {
        //printf("in loop 1 \n");
        path[i] = NULL; 
    }
    path_elem = 0; 
    for(int i = 1; i<counter;i++)
    {
        path[path_elem] = malloc(strlen(arr[i]));
        strcpy(path[path_elem],arr[i]);
        //printf("in loop 2\n");
        path_elem++; 
        
    }
    //printf("end of function\n");
    path[path_elem + 1] = NULL;
}

int executeIF() // then statement should consist of everything after then and before fi
{
    int nested = 0; // default is not nested
    char *if_statement[1024]; // = (char *) malloc(1024 * sizeof(char));
    char *then_statement[1024];   
    int i = 1; 
    int length = 0; 
    int j = 0; 
    char *equal;
    int comparison; 
    int returnValue; 

    // while(token[j] != NULL)
    // {

    //     length++; 
    // }
    
    while(arr[i] != NULL)
    {
        if(strcmp(arr[i], "==") == 0 || strcmp(arr[i], "!=") == 0)
        {
            if_statement[i] = malloc(1024 * sizeof(char));
            equal = arr[i]; 
            break; 
        }
        if_statement[i-1] = arr[i]; 
        i++;
       
    }

    if_statement[i] = '\0'; 

    if(arr[i] == NULL || strcmp(arr[i+1],"then") == 0 ||  strcmp(arr[i+2],"then") != 0 || strcmp(arr[counter-1],"fi") != 0) //if ls == 0 then printf("hello") fi
    {
        throwError(); 
        exit(0); 
        //return 0; 
    } 

    // if(strcmp(arr[counter-1],"fi") != 0)
    // {
    //     throwError(); 
    //     exit(1); 
    // }

    comparison = atoi(arr[i+1]); 
    pid_t  pid = fork(); 
    int status; 
    if(pid < 0 )
    {
        throwError(); 
        exit(1); 
    }
    if(pid  == 0)
    {
        char result[512]; 
        for(int i = 0; i < path_elem;i++)
        {
            strcpy(result,path[i]);
            strcat(result,"/");
            strcat(result,if_statement[0]);
            if(access(result, X_OK) == 0)
            {
                execv(result,if_statement);
                
            }
        }
        // execv(result,arr);
        throwError();
    }
    else
    {
        waitpid(-1,&status,0); 
        returnValue = WEXITSTATUS(status); 
    }
    int conditon = (comparison == returnValue) && (strcmp(equal,"==") == 0); 
    int condition2 = (comparison != returnValue) && (strcmp(equal,"!=") == 0); 
    if(conditon || condition2)
    {
        //execute then
        //arr[i+2] operation to be executed
        //char *then_statement =  malloc(1024 * sizeof(char));
        while(arr[i] != NULL)
        {
            if(strcmp(arr[i], "then") == 0)
            {
                break;
                // then_statement[i] = malloc(1024 * sizeof(char));
                // then_statement[0] = arr[i+1];
                // break; 
            }
            // then_statement[0] = arr[i-1];
            i++;
        }
        i++; 
        int c = 0; 
        while(strcmp(arr[i],"fi") != 0) // get everything between then and fi
        {
            then_statement[c] =  malloc(1024 * sizeof(char));
            then_statement[c] = arr[i];  
            i++; 
            c++; 
        }

        handleCommands(then_statement); 
        return 1;  
    }
        
    // if(comparison != returnValue && strcmp(equal,"!=") == 0)
    // {
    //     //execute then
    // }

    return 1; 
}

void parseInput(FILE *pointer)
{
    size_t buffersize = 1024;  
    char *buffer; 
    size_t size; 
    char *token; 
    buffer = malloc(buffersize * sizeof(char));
    if(buffer == NULL)
    {
        throwError(); 
    }
    size_t n; 
    if(batchMode == 0)
    {
        printf("wish> ");
    }
    while(getline(&buffer,&size,pointer) != -1)
    {
        //printf("Line: %s\n", buffer);
        // if(n == -1)
        // {
        //     break; 
        // }
       
        if(batchMode == 0)
        {
            printf("wish> ");
        }
        token = strtok(buffer,"\t\n ");
        
        char **args  = NULL;
        while(token != NULL)
        {
            arr[counter] = malloc(strlen(token) * sizeof(char));
            strcpy(arr[counter++],token); 
            token = strtok(NULL,"\t\n "); 
        }

        arr[counter] = NULL;

        if(arr[0] == NULL)
        {
            //printf("continues\n"); 
            continue;
        }

        if(strcmp(arr[0],"if") == 0)
        {
            executeIF(); 
            //return; 
        }
        else if(strcmp(arr[0],"exit")==0)
        {
            if(counter != 1)
            {
                throwError();
            }
            else
            {
                exit(0); 
            }  
        }
        else if(strcmp(arr[0],"path") == 0)
        {
            handlePath();
        }
        else if(strcmp(arr[0],"cd") == 0)
        {
            if(counter != 2)
            {
                throwError(); 
            }
            else
            {
                chdir(arr[1]);
                //return;  
            }
        }
        else
        {
            handleCommands(arr); 
            //return; 
        }
        for(int i = 0; i<counter;i++)
        {
            free(arr[i]);
        }
        counter = 0; 
    }


    // size_t n;
    // while(n = getline(&buffer,&buffersize,pointer) != -1)
    // {
    //     if(batchMode == 0)
    //     {
    //         printf("wish> "); 
    //     }
         
    //     token = strtok(buffer,"\t\n ");
    //     char **args  = NULL;
    //     while(token != NULL)
    //     {
    //         arr[counter] = malloc(strlen(token) * sizeof(char));
    //         strcpy(arr[counter++],token); 
    //         token = strtok(NULL,"\t\n "); 
    //     }
    //     arr[counter] = NULL;
    //     if(strcmp(arr[0],"if") == 0)
    //     {
    //         executeIF(); 
    //     }
    //     else if(strcmp(arr[0],"exit")==0)
    //     {
    //         leave(); 
    //     }
    //     else if(strcmp(arr[0],"path")==0)
    //     {
    //         handlePath();            
    //     }
    //     else if(strcmp(arr[0], "cd") == 0)
    //     {
    //         executeCD(); 
    //     }
    //     else
    //     {
    //         handleCommands(arr); 
    //     }
    //     for(int i = 0; i<counter;i++)
    //     {
    //         free(arr[i]);
    //     }
    //     counter = 0;
    // }

}

// int handleRedirection(char *arr[])
// {

// }

void handleCommands(char *arr[])
{
    if(arr[0]==NULL){
        return; 
    }

    if(strcmp(arr[0],"exit")==0)
    {
        if(counter != 1)
        {
            throwError();
        }
        else
        {
            exit(0); 
        }  
    }
    else if(strcmp(arr[0],"path") == 0)
    {
        handlePath();
    }
    else if(strcmp(arr[0],"cd") == 0)
    {
            chdir(arr[1]);
            //return;     
    }
    else
    {
    pid_t  pid = fork(); 
    int status; 
    if(pid < 0 )
    {
        throwError(); 
        exit(1); 
    }
    //printf("forks\n");
    if(pid  == 0)
    {
        char result[512]; 
        for(int i = 0; i < path_elem;i++)
        {
            strcpy(result,path[i]);
            strcat(result,"/");
            strcat(result,arr[0]);
            if(access(result, X_OK) == 0)
            { 
                execv(result,arr);
            }
        }
        // execv(result,arr);
        throwError();
        exit(1); 
    }
    else
    {
        //printf("wait pid\n");
        waitpid(pid,&status,0);
    }
    }
    //throwError(); 
}

void main(int argc, char *argv[])
{
    FILE *pointer;
    if(argc > 2 || argc < 1)
    {
        throwError(); 
        exit(1); 
    }
    else if(argc == 1)
    {
        pointer = stdin; 
        batchMode = 0; 
    }
    else if (argc == 2)
    {
        batchMode = 1; //argc == 2;
        if(!(pointer = fopen(argv[1],"r")))
        {
           throwError(); 
        }
    }
    parseInput(pointer); 
    //return 1; 
}

void throwError()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));  
}
