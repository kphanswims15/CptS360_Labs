// Programmer: Kimi Phan, 11466435
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

// global variables
char *ctable = "0123456789ABCDEF";
uint32_t decimalBase = 10;
uint32_t OCTalBase = 8;
uint32_t HEXBase = 16;
uint16_t unsignedBase = 10;
char buf[512];
int fd;
struct partition *p;

struct partition {
  uint8_t drive;              // 0x80 - active
  uint8_t head;               // starting head
  uint8_t sector;             // starting sector
  uint8_t cylinder;           // starting cylinder
  uint8_t sys_type;           // partition type
  uint8_t end_head;           // end head
  uint8_t end_sector;         // end sector
  uint8_t end_cylinder;       // end cylinder
  uint32_t starting_sector;   // starting sector counting from 0
  uint32_t nr_sectors;        // numbers in partition
};

int rpu(uint32_t x, uint32_t BASE) // creates decimal, Octal, and HEX numbers
{
  char c;
  if(x)
  {
    c = ctable[x % BASE];
    rpu(x / BASE, BASE);
    putchar(c);
  }
}

int rpuUnit16 (uint16_t x, uint16_t BASE) // creates unsigned short integers
{
  char c;
  if(x)
  {
    c = ctable[x % BASE];
    rpuUnit16(x / BASE, BASE);
    putchar(c);
  }
}

void prints(char *ch) // prints a string
{
  while(*ch != '\0')
  {
    putchar(*ch);
    ch++;
  }
}

void printd(int x) // prints an integer (maybe negative)
{
  if (x < 0)
  {
    x = -x;
    putchar('-');
  }
  (x == 0) ? putchar('0') : rpu(x, decimalBase);
  putchar(' ');
}

void printu(uint16_t x) // prints an unsigned short integer
{
  (x == 0) ? putchar('0') : rpuUnit16(x, unsignedBase);
  putchar(' ');
}

void printo(uint32_t x) // print x in OCTal as 0
{
  putchar('0');
  (x == 0) ? putchar('0') : rpu(x, OCTalBase);
  putchar(' ');
}

void printx(uint32_t x) // print x in HEX as 0x
{
  putchar('0');
  putchar('x');
  (x == 0) ? putchar('0') : rpu(x, HEXBase);
  putchar(' ');
}

int myprintf(char *fmt, ...) // some C compilers requires the three dots
{
  char *cp = fmt;                       // cp points to the fmt string
  uint16_t *ip = (uint16_t *)&fmt + 1;  // ip points to the first item
  uint32_t *up = (uint32_t *)&fmt + 1;  // for accessing long parameters on stack
  while (*cp)                           // scan format string
  {
    if (*cp != '%')                    // spit out ordinary chars
    {
      putchar(*cp);
      if (*cp == '\n')                 // for each '\n'
        putchar('\r');                 // print a '\r'
      cp++; continue;
    }
    cp++;
    switch(*cp)                       // print item by %FORMAT symbol
    {
      case 'c' : putchar(*up); break; // char
      case 's' : prints((char *)*up); break;  // string
      case 'u' : printu(*up); break;  // unsigned integer
      case 'd' : printd(*up); break;  // integer
      case 'o' : printo(*up); break;  // unsigned integer in OCT
      case 'x' : printx(*up); break;  // unsigned integer HEX
    }
    cp++; up++; ip++; // advance pointers
  }
}

int main(int argc, char *argv[], char *env[])
{
  int sector;
  int orginalSector;
  int fd = open("vdisk", O_RDONLY);       // open disk iamge file for READ

  int r = read(fd, buf, 512);             // read FIRST 512 bytes into buf[]
  int x = 0, a = 0;

  // prints number of arguments passed
  myprintf("argc = %d\n", argc);

  // prints arguments passed in
  while(*argv != NULL)
  {
    myprintf("argv[ %d] = %s\n", a, *argv);
    argv++;
    a++;
  }

  // prints enviroment pointers
  while(*env != NULL)
  {
      myprintf("env[ %d] = %s\n", x, *env);
      env++;
      x++;
  }

  p = (struct partition *)(&buf[0x1BE]);    // p points at Ptable in buf[]

  myprintf("--------------- RAW FORM ---------------\n");
  for(int x = 0; x < 4; x++)
  {
    myprintf("%d %d %d %d %x %d %d %d %d %d\n",
      p->drive, p->head, p->sector, p->cylinder, p->sys_type, p->end_head, p->end_sector, p->end_cylinder, p->starting_sector, p->nr_sectors);
        p++;
  }

  p = (struct partition *)(&buf[0x1BE]);
  myprintf("---------- Linux fdisk form -----------\n");
  for(int x = 0; x < 4; x++)
  {
      myprintf("%d  %d  %d\n", p->starting_sector, (p->nr_sectors + p->starting_sector - 1), p->nr_sectors);
      p++;
  }

  myprintf("********** Look for Extend Partition ***********\n");

  // checks if partition 4 is an extend partition
  p = (struct partition *)(&buf[0x1BE]) + 3;
  if(5 != p->sys_type)
  {
    return 0;
  }

  myprintf("Ext Partition 4: start_sector = %d\n", p->starting_sector);
  sector = p->starting_sector;
  orginalSector = p->starting_sector;
  myprintf("start   end   size\n");

  // goes until the end
  while(p->starting_sector != 0)
  {
    lseek(fd, (long)(sector*512), 0);         // seek to sector 10
    read(fd, buf, 512);                       // read sector 10 into buf[], etc.

    p = (struct partition *)(&buf[0x1BE]);
    myprintf("LOCAL MBR start_sector = %d ext partition start_sector = %d\n", sector, p->starting_sector);
    myprintf("%d %d %d\n",
      (sector + p->starting_sector), (((sector + p->starting_sector) + p->nr_sectors) - 1), p->nr_sectors);

    // gets the next entry of the partition table to tell you where the next partition is
    p++;
    sector = p->starting_sector + orginalSector;
  }
  close(fd);
}
