#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void init() {
    setbuf(stdin, 0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    alarm(20);
}

unsigned int read_num() {
    unsigned char buffer[10];
    int i;
    for(i = 0;i < 9;i++) {
        buffer[i] = getchar();
        if(buffer[i] == '\n') {
            break;
        }
    }
    if(buffer[i] == '\n') {
        buffer[i] = 0;
    }
    return strtoul(buffer,0,0);
}

int main() {
    init();
    while(1) {
        printf("size: ");
        unsigned int size = read_num();
        if(size > 0x1000) {
            printf("too long\n");
            exit(1);
        }
        unsigned char* buffer = (unsigned char*) malloc(size);
        printf("string: ");
        gets(buffer);
        printf("result: ");
        printf(buffer);
    }
}

