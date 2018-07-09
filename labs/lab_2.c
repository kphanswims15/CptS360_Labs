#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 200
char path[MAX_SIZE];
char *myargv[MAX_SIZE];

// reterives the path
void getPath()
{
  strcpy(path, getenv("PATH"));
  printf("Path: \n");
  printf("PATH=%s\n", path);
}

// spilts the path into directories
void DestructStringsIntoDirs(char *dir[])
{
  char *token;
  int i = 0;

  token = strtok(path, ":");
  while (token != NULL) {
    printf("%s ", token);
    dir[i] = token;
    token = strtok(NULL, ":");
    i++;
  }
  printf("\n");
}

// gets the home directory
void getHomeDir()
{
  printf("HOME = %s\n", getenv("HOME"));
}

// gets the command from the input string from the user
void getCmdAndArgv(char *input)
{
    char *token;
    int i = 0;

    token = strtok(input, " ");
    while (token != NULL)
    {
      myargv[i] = token;
      token = strtok(NULL, " ");
      i++;
    }
}

void isRedirect(char *argv[])
{
  int i = 0;
  int redirectnum = 0;

  while (argv[i])
  {
    if(strcmp(argv[i], "<") == 0)
    { // in
      myargv[i] = NULL;
      close(0);
      open(argv[i+1], O_RDONLY);
    }
    else if(strcmp(argv[i], ">") == 0)
    { // out
      myargv[i] = NULL;
      close(1);
      open(argv[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
    }
    else if(strcmp(argv[i], ">>") == 0)
    { // append`
      myargv[i] = NULL;
      close(1);
      open(argv[i+1], O_WRONLY|O_APPEND);
    }
    i++;
  }
}

// executes the command
void executeCommand(char *cmd, char *argv[], char *env[], char *dir[])
{
  int i = 0;
  int redirect;
  char result[MAX_SIZE];

  // checks if it is a redirect
  isRedirect(argv);

  // searches every directory
  while (dir[i] != '\0')
  {
    strcpy(result, dir[i]);
    strcat(result, "/");
    strcat(result, cmd);
    execve(result, argv, env);
    i++;
  }

  // printf("%s is an invalid command\n", cmd);
}

int isPipe(char *head[], char *tail[])
{
  int i = 0;
  int j = 0;
  int k = 0;

  while(myargv[i])
  {
    if(strcmp(myargv[i], "|") == 0)
    { // has a pipe
      // assign head and tail
      while(strcmp(myargv[j], "|") != 0)
      {
        head[j] = myargv[j];
        j++;
      }
      j++;
      while(myargv[j] != '\0')
      {
        tail[k] = myargv[j];
        j++;
        k++;
      }
      return 1;
    }
    i++;
  }
  return 0;
}

void layPipe(char *head[], char *tail[], char *env[], char *dir[])
{
  int pd[2], pid = 0, i = 0, pid2 = 0, status = 0;
  char comH[MAX_SIZE], comT[MAX_SIZE];
  char *argH[MAX_SIZE], *argT[MAX_SIZE];

  strcpy(comH, head[0]);
  strcpy(comT, tail[0]);

  pipe(pd);

  pid = fork();

  if(pid < 0) // fork() may fail
  {
    perror("fork failed");
    exit(1);
  }

  if (!pid)
  {        // parent as pipe WRITER
    //strcpy(command, head[0]);
    pid2 = fork();
    if(!pid2)
    {
      printf("%s\n", comT);
      close(pd[1]); // READER MUST close pd[1]

      close(0);
      dup2(pd[0], 0);   // replace 0 with pd[0]
      executeCommand(comT, tail, env, dir);   // change image to cmd2
    }
    printf("%s\n", comH);
    close(pd[0]); // WRITER MUST close pd[0]

    close(1);     // close 1
    dup2(pd[1], 1);   // replace 1 with pd[1]
    executeCommand(comH, head, env, dir);  // change image to cmd1

  }
  else
  {            // child as pipe READER
      printf("Child1: %d\n", pid);

      /*pid = waitpid(pid, &status, 0);
      printf("child %d died %d :(\n", pid, status);
      pid = waitpid(pid2, &status, WNOHANG);
      if (pid == 0)
      {
        printf("Waiting for %d to die...\n", pid2);
        pid2 = waitpid(pid2, &status, 0);
      }
      else if (pid < 0)
        printf("Failed to wait\n");
      printf("child %d died %d :(\n", pid2, status);*/
    //  while (errno != ECHILD)
      //{
      wait(&status);
      exit(0);
      //  printf("Child %d died %d\n", pid, status);
      //}

  }
}

// does the processes to run the commands
void forkAChild(char *cmd, char *argv[], char *env[], char *dir[])
{
  int pid, status;
  char *head[MAX_SIZE];
  char *tail[MAX_SIZE];

  pid = fork();

  if(pid < 0) // fork() may fail
  {
    perror("fork failed");
    exit(1);
  }

  if(pid) // parent executes this part
  {
    pid = wait(&status);
    printf("Exit Status: %d\n", status);
  }
  else // child executes this part
  {
    if(isPipe(head, tail) == 1)
    {
      layPipe(head, tail, env, dir);
    }
    else
    {
      executeCommand(cmd, argv, env, dir);
    }
  }
}

// executes the commands
void execute(char *cmd, char *argv[], char *env[], char *dir[])
{
  if(strcmp(cmd, "cd") == 0)
  {
    if(myargv[1] != '\0' && chdir(myargv[1]) == 0)
    {
      chdir(myargv[1]);
    }
    else if(myargv[1] == '\0')
    {
      chdir(getenv("HOME"));
    }
    else
    {
      perror("directory not found\n");
    }
  }
  else if (strcmp(cmd, "exit") == 0)
  {
    exit(1);
  }
  else
  {
    forkAChild(cmd, argv, env, dir);
  }
}

// resets myargv back to '\0'
void resetArgv()
{
  int i = 0;
  while(myargv[i])
  {
    myargv[i] = '\0';
    i++;
  }
}

int main(int argc, char *argv[], char *env[])
{
  char input[MAX_SIZE], cmd[MAX_SIZE], result[MAX_SIZE], cwd[MAX_SIZE];
  char *dir[MAX_SIZE];

  printf("*****Welcome to Kimi Phan's sh!*****\n");

  // gets the path name
  getPath();

   // seperates the directories
  printf("Destructed strings:\n");
  DestructStringsIntoDirs(dir);

  // prints the home directory
  getHomeDir();

  printf("***** kpsh processing loop *****\n");
  while(1)
  {
    printf("kpsh %% %s: ", getcwd(cwd, sizeof(cwd)));

    fgets(input, MAX_SIZE, stdin);
    input[strlen(input) - 1] = 0; // kills \n

    if(strlen(input) != 0)
    {
      // gets the commands and arguments
      getCmdAndArgv(input);

      // gets the command
      strcpy(cmd, myargv[0]);

      // executes the command
      execute(cmd, myargv, env, dir);
    }

    // resets myargv to '\0'
    resetArgv();
  }
}
