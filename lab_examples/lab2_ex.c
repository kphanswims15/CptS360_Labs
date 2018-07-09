#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int pid, status, pd[2], ARGC = 0;

char line[128], command[64];
char *PATH[128];
char HOME[128];
char CWD[128];
char USER[32];
char *ARGV[64], **ENV;

int main(int argc, char *argv[ ], char *env[])
{
  env = ENV;
  getHome();
  getPath();
  getUsername();


  while(1)
  {
    input();
    execute();
    resetArgv();
  }
}

int getHome()
{
  char *home = getenv("HOME");
  char *token = strtok(home, ":");
  strcpy(HOME, token);
}

int getPath()
{
  // Grab PATH environment variable
  char *path = getenv("PATH");
  // Get each path with : as delim
  char *token = strtok(path, ":");
  int i = 0;
  while(token != NULL)
  {
    PATH[i] = token;
    i++;
    token = strtok(NULL, ":");
  }
}

int getUsername()
{
  char *user = getenv("LOGNAME");
  char *token = strtok(user, ":");
  strcpy(USER, token);
}

int resetArgv()
{
  // reset argv
  int i = 0;
  while(ARGV[i])
  {
    //memset(ARGV[i], 0, 64);
    ARGV[i] = NULL;
    i++;
  }
  ARGC = 0;
}


int input()
{
  // prompt
  printf("%s@%s > ", USER, CWD);
  gets(line);

  // get command
  char *token = strtok(line, " ");
  if (token == NULL)return;
  strcpy(command, token);
  ARGV[0] = malloc(sizeof(char)*64);
  strcpy(ARGV[0], token);
  ARGC++;

  // get other arguments
  int i = 1, concatenating = 0;
  token = strtok(NULL, " ");
  while(token != NULL)
  {

    if(concatenating == 0){
      ARGV[i] = malloc(sizeof(char)*64);
      strcat(ARGV[i], token);
      ARGC++;
    }
    // concatenating string
    if (token[0] == '\'' || token[0] == '\"')
    {
      strcat(ARGV[i], " ");
      concatenating = 1;
      --i;
      if (token[strlen(token)-1] == '\'' || token[strlen(token)-1] == '\"')
      {// base case: single word
        ARGV[i][strlen(ARGV)-1] = 0;
        concatenating = 0;
      }
    }
    else if (token[strlen(token)-1] == '\'' || token[strlen(token)-1] == '\"')
    { // case: end of string
      // don't add duplicates
      if (strcmp(ARGV[i], token))
      {
        strcat(ARGV[i], token);
        // get rid of quotes
        int k = 0, len = strlen(ARGV[i]);
        for (k; k < len; k++)
        {
          ARGV[i][k] = ARGV[i][k+1];
        }
        ARGV[i][len-2] = 0;
      }
      concatenating = 0;
    }
    else if (concatenating)
    {// case: middle words
      // don't add duplicates


       strcat(ARGV[i], " ");
       strcat(ARGV[i], token);
      --i;
    }
    i++;
    token = strtok(NULL, " ");
  }
}

int execute()
{
  if(strcmp(command, "cd") == 0)
  {
    // get destination of cd command
    if(ARGV[1] != NULL && chdir(ARGV[1]) == 0)
    {
      strcpy(CWD, ARGV[1]);
    }
    else if(ARGV[1] == NULL || strcmp(ARGV[1], "") == 0)
    {
      chdir(HOME);
      strcpy(CWD, HOME);
    }
    else perror("directory not found\n");
  }
  else if(strcmp(command, "exit") == 0){ exit(1); }
  else
  {
    forkAChild();
  }
}

int forkAChild()
{
  pid = fork();

  if (pid < 0){   // fork() may fail. e.g. no more PROC in Kernel
    perror("fork failed");
    exit(1);
  }
  if (pid)
  { // PARENT EXECUTES THIS PART
    pid = wait(&status);
  }
  else
  { // child executes this part
    // get head and tail
    char *head[64], *tail[64];
    if(isPipe(head, tail))
    {

      layPipe(head, tail);
    }
    else
    {

      // convert string array to string
      /*char argv[128] = {0};
      int i = 0;
      while(ARGV[i])
      {
        strcat(argv, ARGV[i]);
        argv[strlen(argv)] = ' ';
        i++;
      }
      argv[strlen(argv)-1] = 0;*/

      exec(ARGV);
    }
  }
}

int isPipe(char *head[], char *tail[])
{
  int i = 0;
  while(ARGV[i])
  {
    if (strcmp(ARGV[i], "|") == 0)
    { // it has pipe
      //assign head and tail
      int j = 0;
      while(ARGV[j])
      {
        if (j < i)
        { // head
          head[j] = ARGV[j];
        }
        else if (j > i)
        { // tail
          tail[j] = ARGV[j];
        }
        j++;
      }
      return 1;
    }
    i++;
  }
  return 0;
}

int layPipe(char *head[], char *tail[])
{
  int pd[2], lpid;

  pipe(pd);

  if (fork() == 0)
  { // child
    close(pd[0]);
    dup2(pd[1], 1);
    close(pd[1]);
    exec(head);
  }
  else
  { // parent
    close(pd[1]);
    dup2(pd[0], 0);
    close(pd[0]);
    exec(tail);
  }
}

int exec(char *argv[])
{
  //Check each bin folder for command by concatenating pathname with input[0]
  int i = 0;
  char temp[1024];

  // redirection
  if (isRedirect(argv) != -1)
  {
    argv[ARGC-2] = NULL;
  }


  // check each path in PATH variable
  while(PATH[i] != NULL)
  {
      // construct new path
      strcpy(temp, PATH[i]);
      strcat(temp, "/");
      strcat(temp, command);
      // this will execute command if it's found

      execve(temp, argv, ENV);
      i++;
  }
}


int isRedirect(char *argv[])
{

  int i = 0;
  while(argv[i])
  {
    if (strcmp(argv[i], "<") == 0)
    { // in
      redirect(1, argv[i+1]);

      return 1;
    }
    else if (strcmp(argv[i], ">") == 0)
    { // out
      redirect(2, argv[i+1]);
      return 2;
    }
    else if (strcmp(argv[i], ">>") == 0)
    { // append
      redirect(3, argv[i+1]);
      return 3;
    }
    i++;
  }
  return -1;
}

int redirect(int direction, char filename[])
{
  if (direction == 1)
  {
    return createIn(filename);
  }
  else if(direction == 2)
  {
    return createOut(filename);
  }
  else if(direction == 3)
  {
    return createApp(filename);
  }
  return 1;
}

int createApp(char *file)
{
  // close file descriptor 1
  close(1);
  return open(file, O_WRONLY|O_APPEND, 0644);
}

int createOut(char *file)
{
  // close file descriptor 1
  close(1);
  return open(file, O_WRONLY|O_CREAT, 0644);
}

int createIn(char *file)
{
  // close file descriptor 0
  close(0);
  return open(file, O_RDONLY);
}

int scan(char *head, char *tail)
{
  int i = 0, r = -1;
  for(i; i < strlen(line); i++)
  {
    if('|' == line[i])
    {
      // Update this when doing multiple pipes
      strncpy(head, line, i);
      memcpy(tail, line+i, strlen(line));
      r = 0;
    }
  }
  return r;
}
