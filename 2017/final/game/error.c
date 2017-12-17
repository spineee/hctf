/*
 * Lat's create magic!
 */
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
void print_err(char* msg) {
    fprintf(stderr, msg); //format string attack here!!!
    _exit(EXIT_FAILURE);
}

unsigned int read_check(int fd, unsigned char* buffer, int size) {
    // todo: nonblock read
    int ret = recv(fd,buffer,size,0);
    if(ret < 0){
        _exit(1);
    }
    return ret;
}