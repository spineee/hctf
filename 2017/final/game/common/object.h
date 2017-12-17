/*
 * Lat's create magic!
 */

#ifndef HENGINE_OBJECT_H
#define HENGINE_OBJECT_H
#include "../error.h"
typedef char* bool_arr;

extern unsigned int global_id;
extern struct game_object* global_objects;
extern unsigned int global_resource_id;

struct object_detail {
    int type;
};

// the common game object
struct game_object {
    // the object id, this id is dynamic calculate in game;
    unsigned int id;
    // the resource id, this id is static;
    unsigned int resource_id;
    // the object raw is defined below
    struct object_detail* detail;
    // x, y ,z indict the pos of a object
    unsigned int x;
    unsigned int y;
    unsigned int z_index;
    // if the object is visiable
    unsigned int is_show;
    // father's object id
    unsigned int father;
    // for ite
    struct game_object* next;
    int is_updated;
    int is_added;
    int needed_removed;
};

/*
 * different type of object detail
 * object_detail: text | bit_image | animate | map | item | menu | user defined
 */

enum {
    TYPEBITMAP,
    TYPEMAP,
    TYPETEXT,
    TYPEITEM
};

// bit_image: w h has_color ((b_or_w[w*h])[+0](0) (pixels[w,h])[+0](1))

struct object_bitmap {
    int type;
    unsigned int width;
    unsigned int height;
    unsigned int has_color;
    unsigned char* pixels;
    bool_arr is_free;
};


// map: w h pixels[w*h] &is_free[w*h]
struct object_map {
    int type;
    unsigned int width;
    unsigned int height;
    unsigned int has_color;
    unsigned char* pixels;
    // if_free will be represented by bool in bitwise
    bool_arr if_free;
};

// text: unsigned char*
struct object_text {
    int type;
    unsigned char* text;
};

// item: type &entity &script_handle
struct object_item {
    int type;
    unsigned int id;
    // the entity can reference the bitmap or text
    struct object_detail* entity;
    // the script will be registered in global script array before the game start.
    unsigned int script_handle;
};


unsigned char* serialize_object(struct game_object* object, unsigned int* size);
struct game_object* unserialize_object(unsigned char* origin, unsigned int size);
int read_bit(bool_arr arr, int width, int height, int x, int y);
int write_bit(bool_arr arr, int width, int height, int x, int y, int v);




// for object serialize and unserialize
struct buffer_ref {
    unsigned char* source;
    unsigned int pos;
    unsigned int length;
};

struct buffer_ref* prepare_buffer(unsigned char* buffer, unsigned int length);
unsigned char* extract_buffer(struct buffer_ref* ref);

unsigned int read_int(struct buffer_ref* ref);

void write_int(struct buffer_ref* ref, unsigned int value);

void read_n(struct buffer_ref* ref, unsigned char* buffer, unsigned int length);

void write_n(struct buffer_ref* ref, unsigned char* buffer, unsigned int length);


// different types
unsigned char* serialize_bitmap(struct object_bitmap* detail, unsigned int* size);
unsigned char* serialize_map(struct object_map* detail, unsigned int* size);
unsigned char* serialize_text(struct object_text* detail, unsigned int* size);
unsigned char* serialize_item(struct object_item* detail, unsigned int* size);

struct object_bitmap* unserialize_bitmap(unsigned char* origin, unsigned int size);
struct object_map* unserialize_map(unsigned char* origin, unsigned int size);
struct object_text* unserialize_text(unsigned char* origin, unsigned int size);
struct object_item* unserialize_item(unsigned char* origin, unsigned int size);

// main
unsigned char* serialize_object_internal(struct object_detail* detail, unsigned int* size);
struct object_detail* unserialize_object_internal(unsigned char* origin, unsigned int size);


/*  object create and destory interface */
// todo

struct game_object* create_game_object(unsigned int x, unsigned int y, unsigned int z_index, struct object_detail* detail);
void destory_game_object(struct game_object* obj);
void insert_game_object(struct game_object* obj);
struct game_object* find_game_object(unsigned int id);
struct game_object* find_game_object_by_resource_id(unsigned int id);
void delete_game_object(unsigned int id);
struct object_map* wrap_map(unsigned int width, unsigned int height, unsigned int has_color, unsigned char* pixels);
struct object_item* warp_item(struct object_detail* entity, unsigned int script_handle);
struct object_text* wrap_text(unsigned char* text);
struct object_bitmap* wrap_bitmap(unsigned int width, unsigned int height, unsigned int has_color, unsigned char* pixels);

#endif //HENGINE_OBJECT_H