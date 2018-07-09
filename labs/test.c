#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

int myMkdir(const char *pathname, mode_t mode)
{
  if (mkdir(pathname, mode) < 0)
  {
     printf("errno=%d : %s\n", errno, strerror(errno));
  }
  else
  {
      printf("mkdir %s ok\n", pathname);
  }
}

int main()
{
  myMkdir("newdir", 0777);
}
