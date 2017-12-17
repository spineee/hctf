/*
 * Lat's create magic!
 */

#ifndef HENGINE_PROTOCOL_H_H
#define HENGINE_PROTOCOL_H_H

#include "object.h"

#define PROTOSTART 0
#define PROTOEND 1
#define PROTOSTOCADD 2
#define PROTOSTOCDEL 3
#define PROTOSTOCUPDATE 4
#define PROTOSTOCCLR 5
#define PROTOCTOSKEY 6
#define PROTOCTOSTEXT 7
#define PROTOCTOSMOD 666
static int client_fd;

struct section {
    int type;
    unsigned int size;
    unsigned char* data;
    int crc32;
};

struct message {
    int type;
    void* detail;
};

// type 2, server to client add object message
struct s2c_add_object {
    unsigned int number; //number of the object to add
    struct game_object** objs;
};

// type 3, server to client delete object message
struct s2c_delete_object {
    unsigned int number; //number of the object to delete
    unsigned int* ids;
};

// type 4, server to client update object message
struct s2c_update_object {
    unsigned int number; // number of the object to update
    unsigned int* ids;
    struct game_object** objs;
};

struct c2s_key {
    unsigned char keycode;
};

struct c2s_text {
    unsigned int length;
    unsigned char* text;
};

// type 666, mod
struct c2s_mod {
    unsigned int resource_id;
    struct object_detail* detail;
    unsigned int length;
};

//different protocol message
struct section* serialize_section(struct message* msg);

struct message* unserialize_section(struct section* sec);

//recv and send
int recv_section(int fd, struct section* sec);
void send_section(int fd, struct section* sec);
// message handle
struct message* main_loop(int fd, void (*handle)(struct message* message));
void init_all();

void render_all();

void send_message(struct message* message);

#endif //HENGINE_PROTOCOL_H_H