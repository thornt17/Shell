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
  int ind = STDIN_FILENO;
  int outd = STDOUT_FILENO;
  int fd[2];
  char * myfilein = NULL;
  char * myfileout = NULL;
  char * argument_array[MAX_TOKENS];
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    memset(fd,0,sizeof(fd));
    memset(argument_array,0,sizeof(argument_array));
    int j;
    //int k = 0;

    //if(pipecount > 0)
    //{
      for(j = 0; j < tokenCount; j++)
      {
        if(toks[j].type == TTArgument || toks[j].type == TTCommand)
        {
          argument_array[j] = toks[j].command;
        }
      }
    //}
    /*else
    {
      for(j = 0; j < tokenCount; j++, k++)
      {
        if(toks[j].type == TTArgument || toks[j].type == TTCommand)
        {
          argument_array[k] = toks[j].command;
        }
        else
        {
          k--;
        }
      }
    }*/

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
        //close(STDOUT_FILENO);
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
        //close(STDIN_FILENO);
        dup2(ind, 0);
        close(ind);
        //count2++;
        //printf("2");
      }
    }

    if(pipecount == 0) // no piping
    {
      pid = fork();

      if(pid == -1)
      {
        perror("ERROR");
      }
      else if(pid == 0)
      {
        printf("%s %s %s \n", argument_array[0],argument_array[1],argument_array[2]);
        execvp(argument_array[0], &argument_array[0]); //break here to see arguments with gdb
        perror("ERROR");
      }
      else
      {
        //printf("waiting");
        if(backcount == 0)
        {
          wait(&status);
        }
      }
    }
    else //piping
    {

      if(pipe(fd) == -1)
      {
        perror("ERROR");
      }
      pid = fork();
      if(pid == -1)
      {
        perror("ERROR");
      }
      else if(pid == 0) //child
      {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execvp(argument_array[0],argument_array);
        perror("ERROR");
        return 2;
      }
      else
      {
          wait(&status);
        
      }
      newpid = fork();
      if(newpid == -1)
      {
        perror("ERROR");
      }
      else if(newpid == 0)
      {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        execvp(argument_array[0],argument_array);
        perror("ERROR");
        return 2;
      }
      else
      {
        wait(&status);
      }
      close(fd[0]);
      close(fd[1]);
      wait(&status);
      return 0;
    }
  
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

  while(1){ // main loop of shell
    int tokenCount = 0, totalCmds =0;
    struct TokenInfo tokens[MAX_TOKENS];

    fflush(stdin);
    fflush(stdout);


    if (print_prompt) {
      printf("my_shell: ");
      
    }
    else
    {
      printf("");
    }
    
    if (get_line(input, MAX_INPUT_LINE) < 0) {
      exit(-1);
    }
      
    tokenCount = parser(input, tokens, &totalCmds);
    if (tokenCount >= 0) {	
      executeLine(input, tokens, tokenCount);
    }

    memset(&tokens, 0, sizeof(tokens)); //clears the value of the struct so it doesn't remember old commands
    memset(&input, 0, sizeof(input));

  }

  return 0;
}
