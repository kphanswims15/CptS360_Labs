/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: Header file for all functions and such
*/

#ifndef SHARED_H
#define SHARED_H

//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 1024

// typedef things for convenience
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc GD;
typedef struct ext2_dir_entry_2 DIR;
typedef struct ext2_inode INODE;

// pointers 
SUPER *sp;
GD *gp;
DIR *dp;
INODE *ip;

// shared functions between the files
void print(char *s, u32 x);
void get_block(int fd, int blk, char *buf);

// super block
void super(int fd);

// group descriptor
void group_descriptor(int fd);

// imap
void imap(int fd);

// bitmap
void bitmap(int fd);

// used for imap and bitmap
int test_bit(char *buf, int bit);
void printBits(char *buf, int size);

// dir entries
void inode(int fd);
void dir_entries(int fd);

#endif //SHARED_H
