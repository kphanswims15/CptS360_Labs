/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: Prints the inodes bitmap, Exercise 2.
*/

#include "shared.h"

void imap(int fd)
{
    char buf[MAX];
    int inodes, imapblk;

    get_block(fd, 1, buf); // get superblock
    sp = (SUPER *)buf;
    
    printf("******************** imap *********************\n");

    inodes = sp->s_inodes_count; // get inodes_count
    printf("inodes = %d\n", inodes);
    get_block(fd, 2, buf); // get the group descriptor
    gp = (GD *)buf;
    imapblk = gp->bg_inode_bitmap; // get imap block number
    printf("imapblk = %d\n", imapblk);
    get_block(fd, imapblk, buf); // get imap block into buf[]
    
    printBits(buf, inodes);
}
