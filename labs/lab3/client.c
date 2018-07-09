// The echo client client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h> // for system calls
#include <unistd.h> // for read and write
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 256

// Define variables
struct hostent *hp;
struct sockaddr_in  server_addr;

int server_sock, r;
int SERVER_IP, SERVER_PORT;


// clinet initialization code

int client_init(char *argv[])
{
  printf("======= clinet init ==========\n");

  printf("1 : get server info\n");
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  printf("2 : create a TCP socket\n");
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  printf("3 : fill server_addr with server's IP and PORT#\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  printf("4 : connecting to server ....\n");
  r = connect(server_sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("5 : connected OK to \007\n");
  printf("---------------------------------------------------------\n");
  printf("hostname=%s  IP=%s  PORT=%d\n",
          hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
  printf("---------------------------------------------------------\n");

  printf("========= init done ==========\n");
}

void getArgv(char *myArgv[], char *input)
{
  char *token;
  int i = 0;

  token = strtok(input, " ");
  while (token != NULL)
  {
    myArgv[i] = token;
    token = strtok(NULL, " ");
    i++;
  }
}

// finds a command in a list of strings
int findCmd(char *cmd [], char *command)
{
  int i = 0;

  while (cmd[i])
  {
    if (strcmp(command, cmd[i]) == 0)
      return i;  // returns the index of the found command
    i++;
  }
  return -1;
}

void myMkdir(char *dirName)
{
  if (mkdir(dirName, 0777) < 0)
  {
    printf("mkdir %s FAILED\n", dirName);
  }
  else
  {
    printf("mkdir %s okay\n", dirName);
  }
}

int main(int argc, char *argv[ ])
{
  int n, index = 0;
  char line[MAX], ans[MAX];
  char *myArgv[MAX];

  char *cmd[] = { "get", "put", "ls", "cd", "pwd", "mkdir", "rmdir", "rm",
                    "lcat", "lls", "llcd", "lpwd", "lmkdir", "lrmdir", "lrm", "exit", 0 };

  if (argc < 3){
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  client_init(argv);
  // sock <---> server

  while (1){
    printf("******************  Menu  ******************\n");
    printf("* get  put  ls  cd  pwd  mkdir  rmdir  rm  *\n");
    printf("* lcat     lls lcd lpwd lmkdir lrmdir lrm  *\n");
    printf("********************************************\n");

    printf("input a line : ");
    bzero(line, MAX);                // zero out line[ ]
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

    line[strlen(line)-1] = 0;        // kill \n at end
    if (line[0]==0)                  // exit if NULL line
       exit(0);

    getArgv(myArgv, line);
    index = findCmd(cmd, myArgv[0]);

    // executing commands
    switch (index) {
      case 0: // get
          printf("get\n");
          break;
      case 1: // put
          printf("put\n");
          break;
      case 2: // ls
          printf("ls\n");
          break;
      case 3: // cd
          printf("cd\n");
          break;
      case 4: // pwd
          printf("pwd\n");
          break;
      case 5: // mkdir
          printf("mkdir\n");
          break;
      case 6: // rmdir
          printf("rmdir\n");
          break;
      case 7: // rm
          printf("rm\n");
          break;
      case 8: // lcat
          break;
      case 9: // lls
          break;
      case 10: // lcd
          break;
      case 11: // lpwd
          break;
      case 12: // lmkdir
          myMkdir(myArgv[1]);
          break;
      case 13: // lrmdir
          break;
      case 14: // lrm
          break;
      case 15: // exit
          printf("exiting client\n");
          exit(1);
          break;
      default:
          printf("%s is an invalid command\n", myArgv[0]);
    }

    // Send ENTIRE line to server
    n = write(server_sock, line, MAX);
    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

    // Read a line from sock and show it
    n = read(server_sock, ans, MAX);
    printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
  }
}
