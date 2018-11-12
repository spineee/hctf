#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
char buffer[]="hello world";
int main() {
    setbuf(stdout,0);
    puts("Welcome to babyprintf_v2.0");
    puts("heap is too dangrous for printf :(");
    printf("So I change the buffer location to %p\n",buffer);
    puts("Have fun!");
    FILE* out = stdout;
    void* vtable=*(void **)((char*)out+sizeof(struct _IO_FILE));
    while(1){
        volatile int i;
        volatile char tmp;
        for(i=0;i<0x200;i++) {
            read(0,&tmp,1);
            buffer[i]=tmp;
            if(buffer[i] == '\n') {
                buffer[i] = 0;
                break;
            }
        }
        out=stdout;
        if(*(void **)((char*)out+sizeof(struct _IO_FILE)) != vtable) {
            write(1,"rewrite vtable is not permitted!\n",33);
            *(void **)((char*)out+sizeof(struct _IO_FILE)) = vtable;
        }
        printf(buffer,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef);
//        fflush(stdout);
    }
}
