
/*
 * parsing functionality:
 *   TokenType defines the different tokens returned
 *   ...
 */

enum TokenType { //stores type as as unsigned integer 0,1,2,3,4,5 as in the order written
  TTArgument,  //0
  TTCommand, //1
  TTOutRedir, //2
  TTInRedir, //3
  TTBackground, //4
  TTPipe //5
};

#define MAX_SIZE_TOKEN 32
#define MAX_INPUT_LINE 512
#define MAX_TOKENS 15

struct TokenInfo {
  enum TokenType type; //stores the type from 0 to 5
  char command[MAX_SIZE_TOKEN]; //creates an char array for the individual commands to be stores in a string
  	/* type of token from enum above */
  /* other stuff we removed */
};
  
/*
 * what is the return value, and what are the different parameters
 * as well as what the contents of the different pointers are on return
 */

int pipecount; //number of pipes
int backcount; //number if background symbols

int checkspace(char in1); //checks if character is a space

int parser(char *input, struct TokenInfo *toks, int *totalCmdsPtr); //parser to break up command line into tokens