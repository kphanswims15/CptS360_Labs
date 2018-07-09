/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: These are functions that are shared between all the files
*/

#include "shared.h"

// prints all the fields in a certain format
void print(char *s, u32 x)
{
    printf("%-30s = %8d\n", s, x);
}

// get the block you want to read and/or use
void get_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)(blk*MAX), 0);
    read(fd, buf, MAX);
}

// used in printing the imap and bitmap inodes
int test_bit(char *buf, int bit)
{
    int byte = bit/8;
    bit %= 8;

    if (buf[byte] & (1 << bit))
        return 1;
    else 
        return 0;
}

void printBits(char *buf, int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (i && (i % (8 * 8)) == 0)
            putchar('\n');
        else if (i && (i % 8) == 0)
            putchar(' ');

        (test_bit(buf, i)) ? putchar('1') : putchar ('0');
    }
    putchar('\n');
}
