#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

struct directory
{
  char name[64];
  char type;
};

struct node
{
  struct directory info;
  struct node* childPtr;
  struct node* siblingPtr;
  struct node* parentPtr;
};

// global variables
struct node *pRoot, *cwd;                  // root and CWD pointers
char line[128];                           // user input line
char command[16], pathname[64];           // user inputs
char dirName[64], baseName[64];           // string holders
char dname[64], temp[128];
int children = 0;

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm", "reload",
                "save", "menu", "quit", 0};

struct node* search(char *name, struct node *newRoot);


struct node *makeNode(char *newName, char newType)
{
  struct node *temp = (struct node *) malloc(sizeof(struct node));
  strcpy(temp->info.name, newName);
  temp->info.type = newType;
  temp->childPtr = temp->siblingPtr = temp->parentPtr = NULL;
  return temp;
}

void initialize()
{
  struct node *root = makeNode("/", '\0');
  pRoot = root;
  cwd = root;
}

void insert(char *newName, char newType, struct node *insert)
{
  struct node *newNode = makeNode(newName, newType);
  struct node *current;

  current = insert;

  if (current != NULL)
  {
    // inserts the first childPtr
    if (current->childPtr == NULL)
    {
      printf("insering children\n");
      current->childPtr = newNode;
      newNode->parentPtr = current;
    }
    else
    {
      printf("inserting siblings\n");
      current = insert->childPtr;
      while(current->siblingPtr != NULL)
      {
        current = current->siblingPtr;
      }
      current->siblingPtr = newNode;
      newNode->parentPtr = current;
    }
  }
}

struct node* search(char *name, struct node *newRoot)
{
  struct node *pCur = NULL;

  printf("inside search\n");
  pCur = newRoot;

  if(pCur != NULL)
  {
    if(pCur->childPtr == NULL)
    {
      return NULL;
    }
    else
    {
      pCur = pCur->childPtr;
      while(pCur->childPtr != NULL && strcmp(pCur->info.name, name) != 0)
      {
        pCur = pCur->childPtr;
      }
      if (pCur != NULL)
      {
        while(pCur != NULL)
        {
          if(strcmp(pCur->info.name, name)== 0)
          {
            printf("found\n");
            return pCur;
            }
            pCur = pCur->siblingPtr;
        }
        return NULL;
      }
      printf("found\n");
      return pCur;
    }
  }
}

int findCmd(char *command)
{
  int i = 0;

  while(cmd[i])
  {
    if (strcmp(command, cmd[i]) == 0)
      return i;
    i++;
  }
  return -1;
}

// makes a directory if it does not alread exsists
void mkdir(char *pathname)
{
  char *token, *bname;
  struct node * pInsert;

  // get the directory name
  strcpy(temp, pathname);
  bname = strrchr(temp, '/');
  temp[strlen(temp)-strlen(bname)] = 0;
  strcpy(dirName, temp);

  // get the baseName
  strcpy(temp, pathname);
  strcpy(baseName, bname+1);
  //printf("%s\n", baseName);

  // checks if it is an absolute path
  if(pathname[0] == '/')
  {
    pInsert = pRoot;
    if(dirName[0] != 0)
    {
      token = strtok(dirName, "/");
      while(token != NULL)
      {
        printf("s = %s\n", token);
        pInsert = search(token, pInsert);
        token = strtok(NULL, "/");
      }
      if (pInsert != NULL)
      {
        insert(baseName, 'D', pInsert);
      }
    }
    else
    {
      insert(baseName, 'D', pInsert);
    }
  }
}

// removes a directory from your filesystem removes it if there are no files in it
int rmdir(char *pathname)
{

}

// list files in a given directory
int ls(char *pathname)
{
  struct node * pCur;

  pCur = cwd;

  pCur = pCur->childPtr;

  // printf("%s\n", pCur->info.name);
  if (pCur->childPtr == NULL)
  {
    while (pCur != NULL)
    {
      printf("%s\n", pCur->info.name);
      pCur = pCur->siblingPtr;
    }
  }
}

// used to change the current directory
int cd(char *pathname)
{
  char *token;
  struct node *pInsert;

  pInsert = pRoot;

  token = strtok(pathname, "/");
  while(token != NULL)
  {
    printf("s = %s\n", token);
    pInsert = search(token, pInsert);
    token = strtok(NULL, "/");
  }
  cwd = pInsert;
}

void pwd(struct node *cwd)
{
  struct node *temp;
  temp = cwd->parentPtr;
  rpwd(cwd);

  printf("%s\n", temp->info.name);
}

void rpwd(struct node *p)
{
  if (p->parentPtr != p)
  {
    rpwd(p->parentPtr);
    printf("/%s\n", p->info.name);
  }
}

void creat(char *pathname)
{
  char *token, *bname;
  int i;
  struct node * pInsert;

  // get the directory name
  strcpy(temp, pathname);
  bname = strrchr(temp, '/');
  temp[strlen(temp)-strlen(bname)] = 0;
  strcpy(dirName, temp);

  // get the baseName
  strcpy(temp, pathname);
  strcpy(baseName, bname+1);

  // checks if it is an absolute path
  if(pathname[0] == '/')
  {
    pInsert = pRoot;
    if(dirName[0] != 0)
    {
      token = strtok(dirName, "/");
      while(token != NULL)
      {
        printf("s = %s\n", token);
        pInsert = search(token, pInsert);
        token = strtok(NULL, "/");
      }
      if (pInsert != NULL)
      {
        insert(baseName, 'F', pInsert);
      }
    }
    else
    {
      insert(baseName, 'F', pInsert);
    }
  }
}

void menu()
{
  printf("This is the menu!\n");
  printf("Commands that you can do.\n");
  printf("mkdir: make a new directory for a given pathname\n");
  printf("rmdir: rm the directory, if it is empty. \n");
  printf("cd: change CWD to pathname, or to / if no pathname.\n");
  printf("ls: list the directory contents of pathname or CWD \n");
  printf("pwd: print the (absolute) pathname of CWD\n");
  printf("creat: create a FILE node. \n");
  printf("rm: rm a FILE node.\n");
  printf("save: save the current file system tree in a file \n");
  printf("reload: re-initialize the file system tree from a file \n");
  printf("menu: show a menu of valid commands \n");
  printf("quit: save the file system tree, then terminate the program.\n");
}

void quit()
{
  return;
}

int main()
{
  int index = 0;
  char line[128];
  char comd[32], pathname[64];

  initialize(); // initialize root node /, set both root, cwd point at /


  while(1)
  {
   // gets the command and username from the user
   printf("Enter command: ");
   fgets(line, 128, stdin);
   line[strlen(line) - 1] = 0; // kills the \n character at the end

    sscanf(line, "%s %s", comd, pathname);

    index = findCmd(comd);
    printf("%d\n", index);
    switch (index) {
      case 0 : mkdir(pathname);break;
  //    case 1 : rmdir(pathname); break;
      case 2 : ls(pathname); break;
      case 3 : cd(pathname); break;
    //  case 4 : pwd(cwd); break;
      case 5 : creat(pathname); break;
  //    case 6 : rm(); break;
  //    case 7 : relaod(); break;
  //    case 8 : save(); break;
      case 9 : menu(); break;
      case 10 : quit();
                return 0; break;
      default : printf("invalid command\n"); break;
    }
  }
}
