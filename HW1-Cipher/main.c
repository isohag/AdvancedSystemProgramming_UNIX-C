//  Assignment: HW1(Cipher)
//  Author: Rashedul Khan (108921821)
//  Class:  CSE 376 (Advanced Systems Programming in Unix/C).
//  Instructor: Erez Zadok

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>  //  Open
#include <unistd.h> //  Read
#include <pwd.h>    //  getpass(3)

//#DEFINE filename ("in.txt")

//  Helper methods
    char* PassInput() {
    	printf("Hello Makefile\n");
        //  Get password
        char s1[128];
        char* temp;
        do {
            temp = getpass("Encryption Password: ");
            strcpy(s1,temp);
            temp = getpass("One More Time: ");
        } while (strlen(s1)!=strlen(temp) || strcmp(s1, temp) != 0);
        printf("Thanks! Password to Decrypt: \"%s\"\n", temp);
        return temp;
    }

int main() {
    int fd;     		// file descriptor
    int bytes_read; 	// # of bytes read from file.
    int bytes_wrote;	// # of bytes wrote to file.
    void *readPtr;  	// read Buffer.
    char *passin;   	// Password Buffer from getpass(3)

    //  Allocate memory for read buffer
    int pgSize = getpagesize();         // 
    printf("Page size: %d\n", pgSize);
    readPtr = malloc(pgSize);   // generic pointer to read buffer
    if (readPtr == NULL) {
        perror("not enough memory\n");
        exit(1);
    }
    
    //  1. Open file for reading.
    fd = open("in.txt",O_RDONLY);
    if (fd<0) { // failed to open file for reading.
        perror("in.txt");
        exit(2);
    }
    
    //  2. Read file and store data in readPtr buffer.
    bytes_read = read(fd, readPtr, pgSize);
    if (bytes_read<0) { // Failed to read.
        perror("read");
        close(fd);
        exit(3);
    }
    printf("Bytes read: %d\n", bytes_read);

    //  3. Close file
    close(fd);

///////  Cipher Encryption Algorithm.  ////////////////////////////////////////
    //  get password
    passin = PassInput();  //  Retrieve user entered password.
    char * pass = malloc(strlen(passin) + 1);
    strcpy(pass, passin);

//////    Write              /////////////

    //  1. Open file for writing
    fd = open("out.txt",O_WRONLY | O_TRUNC);    // O_CREAT, 777
    if (fd<0) { // failed to open file for writing.
        perror("out.txt");
        exit(4);
    }
    //  Cipher algorithm.
    int len = strlen(pass);
    for (int i=0;i<bytes_read;i++) {
        *((char*)readPtr+i) = (*((char*)readPtr+i)) ^ (*(pass+(i%len)));
    }

    //  2. Write to file
    //	error checking (likely): ENOMEM, EIO
    //	error checking (more likely): ENOSPC, ENOQUOTA
    bytes_wrote = write(fd,readPtr,bytes_read);
    if (bytes_wrote<0) { // Failed to write.
        perror("write");
        exit(5);
    } 
    close(fd);
    printf("out.txt has been Encrypted!\n");

//////////  Cipher Decryption Algorithm.  //////////////////////////////////////////
    ///////     Check for Password     /////////////////
    passin = getpass("Password for Decryption: ");
    while (strlen(pass) != strlen(passin) || strncmp(passin, pass, strlen(passin)) != 0) {
        passin = getpass("Try again: ");
    }
    printf("Congratz! Decryption process started.\n");

    ////////    Open &  Write       /////////////////
    fd = open("out.txt",O_RDONLY);
    if (fd<0) { // failed to open file for writing.
        perror("out.txt for Decryption");
        exit(4);
    }
    bytes_read = read(fd, readPtr, pgSize);
    close(fd);  // File Descriptor no longer needed.

    //  Cipher Algorithm.
    for (int i=0;i<bytes_read;i++) {
        *((char*)readPtr+i) = (*((char*)readPtr+i)) ^ (*(pass+(i%len)));
    }

    free(pass); //  Password no longer needed.

    //  1. Open file for writing
    fd = open("out.txt",O_WRONLY | O_TRUNC);    // O_CREAT, 777
    if (fd<0) { // failed to open file for writing.
        perror("out.txt");
        exit(4);
    } 

    //  2. Write to file
    bytes_wrote = write(fd,readPtr,bytes_read);
    if (bytes_wrote<0) { // Failed to write.
        perror("write");
        exit(5);
    }
    close(fd);   // File Descriptor no longer needed. 
    printf("Decryption Complete!!!\n");

    //  Clean up.
    free(readPtr);
    //free(temp);
    exit (0);
}