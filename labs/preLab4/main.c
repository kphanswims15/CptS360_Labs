/*
Programmer: Kimi Phan
ID: 11466435
Class: CptS 360, Fall 2017
Description: main function that runs the whole program
*/

#include "shared.h"

int main(int argc, char *argv[])
{
    char *device = "mydisk"; // default device name
    char *device2 = "mydisk2";
    int fd;
    int gd;
    
    if (argc > 1)
    {
        device = argv[1];
    }

    fd = open(device, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed\n", device);
        exit(1);
    }

    super(fd);
    group_descriptor(fd);
    imap(fd);
    bitmap(fd);
    dir_entries(fd);

    gd = open(device2, O_RDONLY);
    if (gd < 0)
    {
        printf("open %s failed\n", device2);
        exit(1);
    }

    dir_entries(gd);
    
    // close the file
    close(fd);
}
