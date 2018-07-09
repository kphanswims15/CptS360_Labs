/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: prints the group descriptor, Exercise 1
*/

#include "shared.h"

void group_descriptor(int fd)
{
    char buf[MAX];

    get_block(fd, 2, buf);

    gp = (GD *)buf; // as a group desciptor block

    printf("*************** group descriptor ***************\n");
    print("bg_block_bitmap", gp->bg_block_bitmap); // Bmap block number
    print("bg_inode_bitmap", gp->bg_inode_bitmap); // Imap block number
    print("bg_inode_table", gp->bg_inode_table);   // Inodes begin block number
    print("bg_free_blocks_count", gp->bg_free_blocks_count); 
    print("bg_free_inodes_count", gp->bg_free_inodes_count);
    print("bg_used_dirs_count", gp->bg_used_dirs_count);
}
