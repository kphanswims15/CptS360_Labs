/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: Superblock example
*/

#include "shared.h"

void super(int fd)
{
    char buf[MAX];
    int blksize;

    get_block(fd, 1, buf);

    sp = (SUPER *)buf;  // as a super block structure

    printf("*************** superblock ***************\n");

    // checks for EXT2 FS magic number:
    printf("%-30s = %8x ", "s_magic", sp->s_magic);
    if (sp->s_magic != 0xEF53)
    {
        printf("NOT an EXT2 FS\n");
        exit(1);
    }

    printf("EXT2 FS OK\n");
    print("s_inodes_count", sp->s_inodes_count);
    print("s_blocks_count", sp->s_blocks_count);
    print("s_r_blocks_count", sp->s_r_blocks_count);
    print("s_free_inodes_count", sp->s_free_inodes_count);
    print("s_free_blocks_count", sp->s_free_blocks_count);
    print("s_first_data_block", sp->s_first_data_block);
    print("s_log_block_size", sp->s_log_block_size);
    print("s_blocks_per_group", sp->s_blocks_per_group);
    print("s_inodes_per_group", sp->s_inodes_per_group);
    print("s_mnt_count", sp->s_mnt_count);
    print("s_max_mnt_count", sp->s_max_mnt_count);
    printf("%-30s = %8x\n", "s_magic", sp->s_magic);
    printf("s_mtime = %s", ctime((time_t *)&sp->s_mtime));
    printf("s_wtime = %s", ctime((time_t *)&sp->s_wtime));
    blksize = 1024 * (1 << sp->s_log_block_size);
    print("block size", blksize);
    print("s_inode_size", sp->s_inode_size);
}
