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
    char* EncryptPass() {
        //  Get password
        char s1[128];
        char s2[128];
        char* temp; int i;
        do {
            temp = getpass("Encryption Password: ");
            i = 0;
            while(*temp !='\0') 
                s1[i++] = *(temp++);    //  Copy pass word;
            s1[i] = '\0';               // append null character.
            i = 0;
            temp = getpass("One More Time: ");
            while(*temp !='\0')
                s2[i++] = *(temp++);
            s2[i] = '\0';
            printf("Typed: %s & %s\n", s1, s2);
        } while (strncmp(s1, s2, 128) != 0);
        printf("Thanks\n");
        return s1;
    }

int main() {
    int fd;     // file descriptor
    int reader, writer; // # of bytes read/wrote from/to file.
    void *readPtr;  // read Buffer.

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
    reader = read(fd, readPtr, pgSize);
    if (reader<0) { // Failed to read.
        perror("read");
        close(fd);
        exit(3);
    }
    printf("Bytes read: %d\n", reader);
    for (int i=0;i<reader;i++) {
        printf("%c", *(char*) readPtr);
        readPtr++;
    }
    readPtr -= reader;  // reset readPtr
    printf("\n");
    
    //  get password
    char* pass = EncryptPass();
    printf("Password: %s\n", pass);
    
    //  3. Close file
    close(fd);

/////////////   Write       ///////////////////////////////////
    //  1. Open file for writing
    fd = open("out.txt",O_WRONLY | O_TRUNC);    // O_CREAT, 777
    if (fd<0) { // failed to open file for writing.
        perror("out.txt");
        exit(4);
    }

    //  2. Write to file
    writer = write(fd,readPtr,reader);
    if (writer<0) { // Failed to write.
        perror("write");
        exit(5);
    } 

    //  Clean up.
    close(fd);
    free(readPtr);
    exit (0);
}