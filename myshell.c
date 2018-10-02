#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "parser.h"

/* 
 * input values are the line typed in by user, information deliminating the 
 * different tokens, ...
 * function returns ...
 */

int executeLine(char *input, struct TokenInfo *toks, int tokenCount)
{
  pid_t pid, newpid;
  int status;
  int ind;// = STDIN_FILENO;
  int outd;// = STDOUT_FILENO;
  int fd[2];
  char * myfilein = NULL;
  char * myfileout = NULL;
  char * argument_array[MAX_TOKENS];
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; //for ouput redirect

    memset(fd,0,sizeof(fd)); //clears fd
    memset(argument_array,0,sizeof(argument_array)); //clears argumentss
    int j;

   for(j = 0; j < tokenCount; j++)
   {
    if(toks[j].type == TTCommand || toks[j].type == TTArgument)
    {
        argument_array[j] = toks[j].command;
    }
   }

    argument_array[tokenCount] = NULL;

    int i;
    for(i = 0; i < tokenCount; i++)
    {
      if(toks[i].type == TTOutRedir)
      {
        argument_array[i] = NULL;
        myfileout = argument_array[i+1];
        outd = creat(myfileout,mode);
        if(outd == -1)
        {
          perror("ERROR");
        }
        close(STDOUT_FILENO);
        dup2(outd, 1);
        close(outd);
        //count1++;
        //printf("2");
      }
      else if(toks[i].type == TTInRedir)
      {
        argument_array[i] = NULL;
        myfilein = argument_array[i+1];
        ind = open(myfilein, O_RDONLY, 0);
        if(ind == -1)
        {
          perror("ERROR");
        }
        close(STDIN_FILENO);
        dup2(ind, 0);
        close(ind);
        //count2++;
        //printf("2");
      }
    }

    if(pipecount == 0) // no piping
    {
      pid = fork();

      if(pid == -1) //error fork
      {
        perror("ERROR");
      }
      else if(pid == 0) //child
      {
        //printf("%s %s %s %s %s\n", argument_array[0],argument_array[1],argument_array[2],argument_array[3],argument_array[4]);
        execvp(argument_array[0], argument_array); //break here to see arguments with gdb
        perror("ERROR");
      }
      else //if parent
      {
        //printf("waiting");
        if(backcount == 0) //if not background running process
        {
          wait(&status);
        }
      }
    }
    else //piping
    {
      if(pipe(&(fd[0])) == -1)//set up pipes and error check
      {
        perror("ERROR");
      }
      pid = fork(); //child process
      if(pid == -1)
      {
        perror("ERROR");
      }
      else if(pid == 0) //child
      {
        //printf("%s %s %s %s %s\n", argument_array[0],argument_array[1],argument_array[2],argument_array[3],argument_array[4]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execvp(argument_array[0],argument_array);
        perror("ERROR");
        return 2;
      }
      else //parent
      {
          wait(&status);
        
      }
      newpid = fork(); //second child process
      if(newpid == -1) //error
      {
        perror("ERROR");
      }
      else if(newpid == 0) //child, this is not always working
      {
        //printf("%s %s %s %s %s\n", argument_array[0],argument_array[1],argument_array[2],argument_array[3],argument_array[4]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        execvp(argument_array[0],argument_array);
        perror("ERROR");
        return 2;
      }
      else
      {
        wait(&status); //waits for child
      }
      close(fd[0]);
      close(fd[1]);
      wait(&status);
      return 0;
    }
  
  //close(ind);
  //close(outd);

  //myfilein = NULL;
  //myfileout = NULL;
  fflush(stdin);
  fflush(stdout);


  return 2;       /* bogus */
}

int get_line(char *input, int max)
{
  
  // read input line
  if(fgets(input, MAX_INPUT_LINE, stdin)==NULL){
    perror("failed read");
    return -1;
  }
  return 0; 
}



int main(int argc, char* argv[])
{
  
  int print_prompt = 1;	/* start out printing the prompt */
  char input[MAX_INPUT_LINE];
  // process the input arguments
  if(argc > 1)
  {
    if(strcmp(argv[1], "-n") == 0)
    {
      print_prompt = 0;
    }
  }

  while(((int)input) != EOF){ // main loop of shell, breKS WHEN CTRL-D IS ENTERED
    int tokenCount = 0, totalCmds =0;
    struct TokenInfo tokens[MAX_TOKENS];

    fflush(stdin);
    fflush(stdout);

    memset(&input, 0, sizeof(input)); //RESETTING INPUT

    if (print_prompt) {
      printf("my_shell: ");
      
    }
    else //prints nothing if -n is entered as an argument
    {
      printf("");
    }
    

    if (get_line(input, MAX_INPUT_LINE) < 0) {
      exit(-1);
    }
      
    tokenCount = parser(input, tokens, &totalCmds); //parses input into tokens
    if (tokenCount >= 0) {	
      executeLine(input, tokens, tokenCount); //executes tokens
    }

    memset(&tokens, 0, sizeof(tokens)); //clears the value of the struct so it doesn't remember old commands
    memset(&input, 0, sizeof(input));

  }

  return 0;
}
