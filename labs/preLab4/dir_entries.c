/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: prints the dir_entries of a directory, Exercise 4
*/

#include "shared.h"

void dir_entries(int fd)
{
    int size, iblock, i;
    char buf[MAX], block[MAX], *cp;

    get_block(fd, 1, buf);
    sp = (SUPER *)buf;
    size = MAX << sp->s_log_block_size;

    printf("******************** dir_entries *******************\n");
    printf("Check ext2 FS: ");
    if (sp->s_magic != 0xEF53)
    {
        printf("Not okay, NOT an EXT2 FS\n");
        exit(1);
    }
    else
    {
        printf("OK\n");
    }

    get_block(fd, 2, buf);
    gp = (GD *)buf;

    printf("GD info: %d %d %d %d %d %d\n", 
            gp->bg_block_bitmap, gp->bg_inode_bitmap, gp->bg_inode_table,
            gp->bg_free_blocks_count, gp->bg_free_inodes_count, 
            gp->bg_used_dirs_count);
    printf("inodes begin block = %d\n", gp->bg_inode_table);

    inode(fd);

    printf("***************************************************\n");
  
    get_block(fd, 2, buf);
    gp = (GD *)buf;

    iblock = gp->bg_inode_table;
    get_block(fd, iblock, buf);

    ip = (INODE *)buf;
    ip++;

    for (i = 0; i < (ip->i_size/size); i++)
    {
        get_block(fd, ip->i_block[i], block);
        cp = block;
        dp = (DIR *)block;

        printf("i_block[%d]\n", i);
        printf("---------------------------\n");
        printf("inode rec_len name_len name\n");
        printf("---------------------------\n");

        while (cp < (block + size))
        {
            printf("%3d %5d %3d %s\n", dp->inode, dp->rec_len, dp->name_len,
                    dp->name);
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
}
