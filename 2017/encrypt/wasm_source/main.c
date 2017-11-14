#include "mycrypt.h"
unsigned int outsize;

unsigned char* encrypt() {
    unsigned char* key = (unsigned char*)malloc(32);
    unsigned char* data = (unsigned char*)malloc(1024);
    unsigned char* pass = (unsigned char*)malloc(8);
    unsigned char* random = (unsigned char*)malloc(16);
    read_file("flag",key,32);
    int size = read_data(data);
    read_pass(pass);
    read_random(random);
    unsigned char* key_s = hash(key,32);
    free(key);
    for(int i = 0;i < 8;i++) {
        key_s[i] ^= pass[i];
    }
    free(pass);
    unsigned char* real_pass = hash(key_s, 16);
    free(key_s);
    for(int i = 0;i < 16;i++) {
        real_pass[i] ^= random[i];
    }
    unsigned char* output = malloc(size+16);
    for(int i = 0;i < 16;i++) {
        output[i] = random[i];
    }
    free(random);
    mycrypt(real_pass, data, output+16, size);
    outsize = size + 16;
    free(data);
    return output;
}


unsigned char* decrypt() {
    unsigned char* key = (unsigned char*)malloc(32);
    unsigned char* data = (unsigned char*)malloc(1024);
    unsigned char* pass = (unsigned char*)malloc(8);
    unsigned char* random = (unsigned char*)malloc(16);
    read_file("flag",key,32);
    int size = read_data(data);
    read_pass(pass);
    for(int i = 0;i < 16;i++) {
        random[i] = data[i];
    }
    unsigned char* key_s = hash(key,32);
    for(int i = 0;i < 8;i++) {
        key_s[i] ^= pass[i];
    }
    free(pass);
    unsigned char* real_pass = hash(key_s, 16);
    free(key_s);
    for(int i = 0;i < 16;i++) {
        real_pass[i] ^= random[i];
    }
    free(random);
    unsigned char* output = malloc(size);
    mycrypt(real_pass, data+16, output, size-16);
    outsize = size - 16;
    free(data);
    return output;
}

unsigned int out_size() {
    return outsize;
}
