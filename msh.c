/*
NAME: Samuel Diaz
ID:   1000833026

*/




    // The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.token[0],token[1],token[2],token[3],token[4],token[5],null

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define TRUE 1
#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports five arguments






//link list for keeping track of command line entries
struct hist
{
        char line[MAX_COMMAND_SIZE];
        struct hist *next;
        struct hist *prev;
};


//function to add nodes to end of the list
//will store command entries, head = head of linklist
//line = what was entered in the command line to add to link list
void append(struct hist** head, char *line)
{
    struct hist* new_line = (struct hist*)malloc(sizeof(struct hist));

    struct hist* last = *head;

    strcpy(new_line->line,line);

    new_line->next = NULL;

    if(*head == NULL)
    {
        new_line->prev = NULL;
        *head = new_line;
        return;
    }

    while(last->next != NULL)
        last = last->next;

    last->next = new_line;

    new_line->prev = last;

    return;
}




//function for catching signal to let parent know child is done
static void wait_signal (int sig)
{
    int status;

    waitpid(-1, &status, WNOHANG);

}


//function used by child to suspend
//handle the suspend singal
static void handle_signal (int sig )
{


    pid_t pid;

    pid = getpid();
    signal(SIGTSTP,SIG_DFL);
    kill(   pid, SIGTSTP);
}



//Attempted to open the file for reading in order to verify
//the existence of the file fname=command line input token[0]
//file being  searched for
int verify(char *fname)
{

    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
       return 0;

    return 1;
}


int main()
{

    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    int i;
    int *list = (int*)calloc(15,sizeof(int));
    int n = 0;
    struct hist* head = NULL;
    char hnum[2];


    int h = 0;



    struct sigaction act;

    /*
      Zero out the sigaction struct
    */
    memset (&act, '\0', sizeof(act));

    /*
      Set the handler for sigchld.
    */
    act.sa_handler = &wait_signal;

    //sigint and sigstp will be ignored in the parent.
    //will be reactivated in the child
    if (signal(SIGINT ,SIG_IGN) == SIG_ERR)
    {
        perror ("sigaction: ");
        return 1;
    }


    if (signal(SIGTSTP ,SIG_IGN) == SIG_ERR)
    {
        perror ("sigaction: ");
        return 1;
    }


    if (sigaction(SIGCHLD , &act, NULL) < 0)
    {
        perror ("sigaction: ");
        return 1;
    }



    while(TRUE)
    {
        // Print out the msh prompt
        printf ("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
         char *token[MAX_NUM_ARGUMENTS];
         int   token_count = 0;

        // Pointer to point to the token
        // parsed by strsep
         char *arg_ptr;




         //Access the linklist if needing something from history
         //replace the command line entry from history
         if(cmd_str[0] == '!' && h > 0)
         {
             for ( i = 1; i < strlen(cmd_str)-1;i++)
                 hnum[i-1] = cmd_str[i];

             if(atoi(hnum) < h)
                 {
                     struct hist* current = head;
                     for( i = 0; i < atoi(hnum);i++)
                         current = current->next;

                     strcpy(cmd_str,current->line);


                 }
         }







         //need to append the linklist, but also need a fifo if 15 max is hit
         if(strcmp(cmd_str,"\n") != 0)
         {

             if ( h < 15)
             {
                 h++;
                 append(&head,cmd_str);
             }
             else
             {
                 append(&head,cmd_str);
                 head = head->next;
             }
         }




         char *working_str = strdup( cmd_str );
         char *head_ptr = working_str;

         while ( ( ( arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL ) && (token_count < MAX_NUM_ARGUMENTS))
         {
           token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE);
           if( strlen( token[token_count] ) == 0 )
           {
             token[token_count] = NULL;
           }
           token_count++;
         }


        // Now print the tokenized input as a debug check
        // \TODO Remove this code and replace with your shell functionality





        char *pn;

        int found = 1;

        if (token[0] != NULL)
        {








            if (strcmp(token[0],"quit") == 0 || strcmp(token[0],"exit") == 0)
            {
                free( pn);
                free( head_ptr );
                free( list );
                return 0;
            }
            else if (strcmp(token[0],"cd") == 0)
            {
                chdir(token[1]);
            }
            else if (strcmp(token[0],"bg") == 0 )
            {
                kill(0,SIGCONT);

            }
            else if (strcmp(token[0],"listpids") == 0)
            {
                ;
                for( i = 0; i < n; i++)
                    printf("%d: %d\n",i,list[i]);
            }
            else if (strcmp(token[0],"history") == 0)
            {
                struct hist* current = head;
                for( i = 0; i < h; i++)
                {
                    printf("%d: %s",i,current->line);
                    current = current->next;
                }

            }

            else
            {


                pn = (char*)malloc(strlen(token[0]) + 3);

                strcpy(pn,"./");
                strcat(pn,token[0]);

                if (verify(pn) == 0 )
                {
                    pn = (char*)realloc(pn,5 + strlen(token[0]) + 1);
                    strcpy(pn,"/bin/");
                    strcat(pn,token[0]);

                    if(verify(pn) == 0)
                    {
                        pn = (char*)realloc(pn,15 + strlen(token[0]) + 1);
                        strcpy(pn,"/user/local/bin");
                        strcat(pn,token[0]);

                        if(verify(pn) == 0)
                        {
                            pn = (char*)realloc(pn,9 + strlen(token[0]) + 1);
                            strcpy(pn,"/usr/bin/");
                            strcat(pn,token[0]);

                             if (verify(pn) == 0)
                            {
                                printf("%s: Command not found!\n",token[0])  ;
                                found = 0;
                            }
                        }

                    }
                }



                if(found)
                {


                    pid_t child_pid = fork();





                    if( child_pid == 0 )
                    {



                        if (signal(SIGINT ,SIG_DFL) == SIG_ERR)
                        {
                            perror ("sigaction: ");
                            return 1;
                        }

                        act.sa_handler = &handle_signal;
                        if (sigaction(SIGTSTP , &act, NULL) < 0)
                        {
                            perror ("sigaction: ");
                            return 1;
                        }

                        execl(pn,token[0],token[1],token[2],token[3],token[4],token[5],token[6],token[7],token[8],token[9],token[10],NULL);


                    }
                    else
                    {

                        if( n < 15)
                        {
                           list[n] = (int)child_pid;
                           n++;


                        }
                        else
                        {
                            for( i = 0; i < 14; i++)
                                list[i] = list[i+1];
                                list[14] =  (int)child_pid;
                        }

                        pause(); //wait for child

                    }

                }
            }
            printf("\n");
        }

            free( pn);
            free( head_ptr );

      }
    free(list);
    return (0);
}
