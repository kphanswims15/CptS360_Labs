/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: Prints out the inodes for the bitmap, Exercise 2
*/

#include "shared.h"

void bitmap(int fd)
{
    char buf[MAX];
    int iBitmapBlk, block_count;

    get_block(fd, 1, buf); // get superblock
    sp = (SUPER *)buf;

    printf("******************** bitmap ********************\n");
    
    block_count = sp->s_blocks_count; // get the block count
    printf("block count = %d\n", block_count);
    get_block(fd, 2, buf); // get the group descriptor
    gp = (GD *)buf;
    iBitmapBlk = gp->bg_block_bitmap;
    printf("ibitmapblk = %d\n", iBitmapBlk);
    get_block(fd, iBitmapBlk, buf);

    printBits(buf, block_count);
}
