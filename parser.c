#include <stdio.h>
#include <string.h>
#include "parser.h"

int checkspace(char in1) //checks if the character is a space, 1 if a space, 0 otherwise
{
	if(in1 == ' ')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int parser(char *input, struct TokenInfo *toks, int *totalCmdsPtr)
{
	int size = strlen(input); //saves size of command string
	int comnum = 0; //number of cmannds
	int strnum = 0; //number of characters in a string
	pipecount = 0; //counts how many times we are piping
	backcount = 0; //counts how many times we use a background call
	int commandnumber = 0; //return value
	int metacount = 0; //counts to make sure two meta characters aren't used in a row
	
	int i;
	for(i = 0; i < size-1; i++)
	{
		if(metacount > 1)
		{
			metacount = 0;
			perror("ERROR: Invalid Input");
		}
		if(checkspace(input[i]) == 0) // executes if not a space
		{
			if(input[i] == '>')
			{
				if(i == 0)
				{
					perror("ERROR: Invalid Command");
				}
				if(pipecount > 0) //prints error if > is called after pipe
				{
					perror("ERROR: Cannot allow Output Redirect after piping");
				}
				toks[comnum].type = TTOutRedir; //sets the type to Ouput Redirect
				strnum = 0; //resets to beginning of a new string
				toks[comnum].command[strnum] = input[i]; //character is stored in tokens.command
				if(checkspace(input[i+1]) == 0) //if the next character is not a space, then we start a new command
				{
					comnum++;
				}
				metacount++;
			}
			else if(input[i] == '<') //functions the same as > if block
			{
				if(i == 0)
				{
					perror("ERROR: Invalid Command");
				}
				if(pipecount > 0)
				{
					perror("ERROR: Cannot allow Intput Redirect after piping");
				}
				toks[comnum].type = TTInRedir;
				strnum = 0;
				toks[comnum].command[strnum] = input[i];
				if(checkspace(input[i+1]) == 0)
				{
					comnum++;
				}
				metacount++;
			}
			else if(input[i] == '|') //functions the same as > if block
			{
				if(i == 0)
				{
					perror("ERROR: Invalid Command");
				}
				pipecount++;
				toks[comnum].type = TTPipe;
				strnum = 0;
				toks[comnum].command[strnum] = input[i];
				if(checkspace(input[i+1]) == 0)
				{
					comnum++;
				}
				metacount++;
			}
			else if(input[i] == '&') //functions the same as > if block
			{
				if(pipecount > 0)
				{
					perror("ERROR: Cannot execute command in background after piping");
				}
				if(i == 0)
				{
					perror("ERROR: Invalid Command");
				}
				backcount++; //adds to check if we have more than one &
				toks[comnum].type = TTBackground;
				strnum = 0;
				toks[comnum].command[strnum] = input[i];
				if(checkspace(input[i+1]) == 0)
				{
					comnum++;
				}
				metacount++;
			}
			else //if a normal character
			{
				
				if(comnum > 0 && toks[comnum-1].type == TTPipe) //if we just piped sets  the type to command
				{
					toks[comnum].type = TTCommand;
				}
				else if (comnum == 0) //if first command then it is set to TTCommand 
				{
					toks[comnum].type = TTCommand;
				}
				else //sets to argument otherwise
				{
					toks[comnum].type = TTArgument; 
				}
				toks[comnum].command[strnum] = input[i]; //adds charcter to token string
				strnum++; //adds 1 to the string index
				if(input[i+1] == '>' || input[i+1] == '<' || input[i+1] == '|' || input[i+1] == '&')
				{
					comnum++; //if one of the operators is not next we stay on the same word
				}
				metacount = 0;
			}
		}
		else //if a space, spaces are never saved
		{
			if(i == 0)
			{
				perror("ERROR: Invalid Command");
			}
			if(backcount > 0) //stops 2 functions from running in the background
			{
				perror("ERROR: Only 1 function may run in the background and must be at the end of the line");
			}
			if(i > 0 && checkspace(input[i-1]) == 0) //if previous character was a space
			{
				comnum++; //new command token
				strnum = 0; //reset string
			}
		}
	}
	

	/*for(int i = 0; i <= comnum; i++)
	{

		int newsize = strlen(toks[i].command);
		printf("%s  Length: %d Type:%u\n",toks[i].command,newsize,toks[i].type);
	}

	printf("Parser Command number is %d \n", comnum);*/

	if(comnum >= 0 && size > 1)
	{
		commandnumber = comnum + 1;
	}
	


	fflush(stdout);
	fflush(stdin);


	return commandnumber;
}
