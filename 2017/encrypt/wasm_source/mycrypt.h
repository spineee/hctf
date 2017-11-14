#include "mem.h"
#include "mt.h"

unsigned char* hash(unsigned char* input, int size) {
    unsigned char* outbuf = (unsigned char*)malloc(16);
    int i;
    for(i = 0; i < 16; i++) {
        outbuf[i] = i;
    }
    for(i = 0; i < size; i++) {
        outbuf[i%16] ^= input[i];
        outbuf[i%16] += outbuf[(i+1)%16];
        outbuf[i%16] += outbuf[(i+2)%16];
        outbuf[i%16] ^= outbuf[(i+3)%16];
        outbuf[(i+4)%16] ^= input[i];
        outbuf[(i+4)%16] += outbuf[(i+5)%16];
        outbuf[(i+4)%16] += outbuf[(i+6)%16];
        outbuf[(i+4)%16] ^= outbuf[(i+7)%16];
        outbuf[(i+8)%16] ^= input[i];
        outbuf[(i+8)%16] += outbuf[(i+9)%16];
        outbuf[(i+8)%16] += outbuf[(i+10)%16];
        outbuf[(i+8)%16] ^= outbuf[(i+11)%16];
        outbuf[(i+12)%16] ^= input[i];
        outbuf[(i+12)%16] += outbuf[(i+13)%16];
        outbuf[(i+12)%16] += outbuf[(i+14)%16];
        outbuf[(i+12)%16] ^= outbuf[(i+15)%16];
    }
    return outbuf;
}


void mycrypt(unsigned char* real_pass, unsigned char* msg, unsigned char* output, int length) {
    int i,j;
    for(i = 0; i < 4; i++) {
        Initialize(*((unsigned int*)real_pass+i));
        for(j = 0; j < length/4; j++) {
            unsigned int rand = ExtractU32();
            *((unsigned int*)output+j) = *(((unsigned int*)msg)+j) ^ rand;
        }
        int k = length%4;
        unsigned int rand = ExtractU32();
        do {
            output[length-k] = msg[length-k] ^ ((rand >> (8*k))&0xff);
        } while(k--);
    }
}
