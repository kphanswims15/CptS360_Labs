# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>

//Glenn Duncan
//9-1-17
//CptS 360

struct partition{
	uint8_t drive;
	uint8_t head;
	uint8_t sector;
	uint8_t cylinder;
	uint8_t sys_type;
	uint8_t end_head;
	uint8_t end_sector;
	uint8_t end_cylinder;
	uint32_t start_sector;
	uint32_t nr_sectors;
};

char buf[512];
int fd;
struct partition *p;

int rpd(int x)
{
	char* cTable = "0123456789";
	char c;
	if(x)
	{
		c = cTable[x % 10];
		rpd(x / 10);
		putchar(c);
	}
}
 
void rpu(uint16_t x)
{
	char* cTable = "0123456789";
	char c;
  	if(x)
  	{
    		c = cTable[x % 10];
    		rpu(x / 10);
    		putchar(c);
  	}
}

int rpo(uint32_t x)
{
	char* cTable = "01234567";
	char c;
  	if(x)
  	{
    		c = cTable[x % 8];
    		rpo(x / 8);
    		putchar(c);
  	}
}

int rpx(uint32_t x)
{
	char* cTable = "0123456789ABCEDF";
	char c;
  	if(x)
  	{
    		c = cTable[x % 16];
    		rpx(x / 16);
    		putchar(c);
  	}
}
 
void prints(char *x)
{
  	while(*x != '\0')
  	{
    		putchar(*x);
    		x++;
  	}
}
 
void printu(uint16_t x)
{
  	(x == 0) ? putchar('0') : rpu(x);
  	putchar(' ');
}

void printd(int x)
{
  	if (x < 0)
  	{
    		putchar('-');
		x *= -1;
		rpu(x);
		putchar(' ');
  	}

	else{
  	(x == 0) ? putchar('0') : rpu(x);
  	putchar(' ');
	}
}
 
int printo(uint32_t x)
{
  putchar('0');
  (x == 0) ? putchar('0') : rpo(x);
  putchar(' ');
}
 
int printx(uint32_t x)
{
  putchar('0');
  putchar('x');
  (x == 0) ? putchar('0') : rpx(x);
  putchar(' ');
}
 
int myprintf(char *fmt, ...)
{
  char *cp = fmt;                      
  uint16_t *ip = (uint16_t *)&fmt + 1; 
  uint32_t *up = (uint32_t *)&fmt + 1; 
  while (*cp)                          
  {
    if (*cp != '%')                   
    {
      putchar(*cp);
      if (*cp == '\n')                
        putchar('\r');                
      cp++; continue;
    }
    cp++;
    switch(*cp)                      
    {
      case 'c' : putchar(*up); break;
      case 's' : prints(*up); break;
      case 'u' : printu(*up); break;
      case 'd' : printd(*up); break;
      case 'o' : printo(*up); break;
      case 'x' : printx(*up); break;
    }
    cp++; up++; ip++;
  }
}
 
main(int argc, char *argv[], char *env[])
{
  int sector;
int i = 0;
   fd = open("vdisk", O_RDONLY);          // open disk iamge file for READ

   int r = read(fd, buf, 512);                // read FIRST 512 bytes into buf[ ]
   p = (struct partition *)(&buf[0x1BE]); // p points at Ptable in buf[ ]

   // p->cylinder, p->sector, etc.

	myprintf("Raw Form: \n %d", i); //RAW FORM

	while(i < 4)
	{
	myprintf("%d ", p->drive);
	myprintf("%d ", p->head);
	myprintf("%d ", p->sector);
	myprintf("%d ", p->cylinder);
	myprintf("%x ", p->sys_type);
	myprintf("%d ", p->end_head);
	myprintf("%d ", p->end_sector);
	myprintf("%d ", p->end_cylinder);
	myprintf("%d ", p->start_sector);
	myprintf("%d \n \n", p->nr_sectors);
	p ++;
	i++;
	}
	
	p = (struct partition *)(&buf[0x1BE]);
	i = 0;


	myprintf("Start End Size\n"); //FDISK FORM
	while(i < 4)
	{
	myprintf("%d ", p->start_sector);
	myprintf("%d ", (p->start_sector) + (p->nr_sectors) - 1);
	myprintf("%d \n \n", p->nr_sectors);
	if(p->sys_type == 5)
	{
		sector = p->start_sector;
	}
	p ++;
	i++;
	}

	

	const int sector1 = sector; //So sorry TA
	int start_sector = 0;
	int nr_sectors = 0;


	myprintf("Ext Partition 4 : Start Sector = %d\n", sector);
	
	do { ////////////////////////////////////////////////////////////////////////////////

	lseek(fd, (long)(sector*512), 0);          
   		read(fd, buf, 512);  
		p = (struct partition *)(&buf[0x1BE]);
	
		start_sector = p->start_sector;
		nr_sectors = p->nr_sectors;
		p++;
		myprintf("LOCAL MBR start_sector = %d ext partition start_sector = %d\n", sector, start_sector);

		

	myprintf("%d ", sector + start_sector);
	myprintf("%d ", sector + start_sector + nr_sectors - 1);
	myprintf("%d \n \n", nr_sectors);

	sector = sector1 + p->start_sector;
	}while(p->start_sector != 0);

}
