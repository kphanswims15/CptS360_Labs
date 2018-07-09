all: main

main: main.c shared.c super.c group_descriptor.c imap.c
	gcc -m32 -Wall -o main.out main.c shared.c super.c group_descriptor.c imap.c bitmap.c inode.c dir_entries.c

clean:
	rm main.out
