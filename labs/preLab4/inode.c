/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: Example to print out inode info
*/

#include "shared.h"

void inode(int fd)
{
    char buf[MAX]; 
    int iblock;

    get_block(fd, 2, buf);
    gp = (GD *)buf;

    iblock = gp->bg_inode_table;
    printf("******************** root inode information ********************\n");
    get_block(fd, iblock, buf);
    ip = (INODE *)buf;
    ip ++; // ip point at #2 INODE
    printf("mode = %4x ", ip->i_mode);
    printf("uid = %d gid = %d\n", ip->i_uid, ip->i_gid);
    printf("size = %d\n", ip->i_size);
    printf("ctime = %s\n", ctime((time_t *) &ip->i_ctime));
    printf("links = %d\n", ip->i_links_count);
    printf("i_block[0] = %d\n", ip->i_block[0]);
}
