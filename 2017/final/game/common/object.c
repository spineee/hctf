/*
 * Lat's create magic!
 */
#include "object.h"
#include <string.h>
#include <stdlib.h>

unsigned int global_id;
struct game_object* global_objects;
unsigned int global_resource_id;

int read_bit(bool_arr arr,int w, int h,int x, int y) {
    if(x >= w || y >= h) {
        return -1;
    }
    return (arr[x/8+y*w]&(1<<(x%8))) == 0 ? 0 : 1;
}

int write_bit(bool_arr arr,int w, int h, int x, int y, int v) {
    if(x >= w || y >= h) {
        return -1;
    }
    if(v == 1) {
        arr[x/8+y*w] &= (1<<(x%8));
        return 0;
    }
    if (v == 0) {
        arr[x/8+y*w] &= (!(1<<(x%8)));
        return 0;
    }
    return -1;
}

unsigned int read_int(struct buffer_ref* ref) {
    if(ref->pos <= ref->length - 4) {
        unsigned int result = *(unsigned int *) (ref->source + ref->pos);
        ref->pos += 4;
        return result;
    }
    return 0;
};

void write_int(struct buffer_ref* ref, unsigned int value) {
    if(ref->pos <= ref->length - 4) {
        *(unsigned int *) (ref->source + ref->pos) = value;
        ref->pos += 4;
    }
};

void read_n(struct buffer_ref* ref, unsigned char* buffer, unsigned int length) {
    if(ref->pos <= ref->length - length) {
        for(int i = 0; i < length; i++) {
            buffer[i] = *(ref->source + ref->pos++);
        }
    }
}

void write_n(struct buffer_ref* ref, unsigned char* buffer, unsigned int length) {
    if(ref->pos <= ref->length - length) {
        for(int i = 0; i < length; i++) {
            *(ref->source + ref->pos++) = buffer[i];
        }
    }
}

struct buffer_ref* prepare_buffer(unsigned char* buffer, unsigned int length) {
    struct buffer_ref* ref = malloc(sizeof(struct buffer_ref));
    ref->source = buffer;
    ref->pos = 0;
    ref->length = length;
    return ref;
}

unsigned char* extract_buffer(struct buffer_ref* ref) {
    unsigned char* result = ref->source;
    free(ref);
    return result;
}

unsigned char* serialize_object(struct game_object* object, unsigned int* size) {
    unsigned int length;
    unsigned char* bytes = serialize_object_internal(object->detail, &length);
    unsigned char* result = malloc(5*sizeof(int) + length);
    *(unsigned int*)result = object->id;
    *((unsigned int*)result + 1) = object->resource_id;
    *((unsigned int*)result + 2) = object->x;
    *((unsigned int*)result + 3) = object->y;
    *((unsigned int*)result + 4) = object->z_index;
    *((unsigned int*)result + 5) = object->is_show;
    memcpy(result + 6*sizeof(int), bytes, length);
    *size = 6*sizeof(int) + length;
    return result;
}


struct game_object* unserialize_object(unsigned char* origin, unsigned int size) {
    struct game_object* object = malloc(sizeof(struct game_object));
    object->id              = *(unsigned int*)origin;
    object->resource_id     = *((unsigned int*)origin + 1);
    object->x               = *((unsigned int*)origin + 2);
    object->y               = *((unsigned int*)origin + 3);
    object->z_index         = *((unsigned int*)origin + 4);
    object->is_show         = *((unsigned int*)origin + 5);
    struct object_detail* detail = unserialize_object_internal(origin + 6*sizeof(int), size - 6*sizeof(int));
    object->detail = detail;
    return object;
}


/*
 * struct object_bitmap {
    int type;
    unsigned int width;
    unsigned int height;
    unsigned int has_color;
    unsigned char* pixels;
    bool_arr is_free;
    };
 */
unsigned char* serialize_bitmap(struct object_bitmap* bitmap, unsigned int* size) {
    // count of the size
    unsigned int count = 4 * sizeof(int) + bitmap->width * bitmap->height/8;
    unsigned int pixels_len;
    if(bitmap->has_color) {
        pixels_len = bitmap->width * bitmap->height * 4;

    }
    else {
        pixels_len = bitmap->width * bitmap->height / 8;
    }
    count += pixels_len;
    *size = count;
    unsigned char* buffer = malloc(count);
    struct buffer_ref* ref = prepare_buffer(buffer, count);
    write_int(ref, bitmap->type);
    write_int(ref, bitmap->has_color);
    write_int(ref, bitmap->width);
    write_int(ref, bitmap->height);
    write_n(ref, bitmap->is_free, bitmap->width * bitmap->height/8);
    write_n(ref, bitmap->pixels, pixels_len);
    return extract_buffer(ref);
}

unsigned char* serialize_map(struct object_map* detail, unsigned int* size) {
    return serialize_bitmap((struct object_bitmap*)detail, size);
}

/*
 * struct object_text {
    int type;
    unsigned char* text;
};
 */
unsigned char* serialize_text(struct object_text* detail, unsigned int* size) {
    unsigned char* result = (unsigned char*)malloc(sizeof(int)+strlen(detail->text));
    *(unsigned int*)(result) = detail->type;
    memcpy(result + sizeof(int),detail->text,strlen(detail->text));
    *size = strlen(detail->text)+sizeof(int);
    return result;
}


/*
 * struct object_item {
    int type;
    unsigned int id;
    // the entity can reference the bitmap or text
    struct object_detail* entity;
    // the script will be registered in global script array before the game start.
    unsigned int script_handle;
};
 */
unsigned char* serialize_item(struct object_item* detail, unsigned int* size) {
    unsigned char* result;
    if(detail->id == TYPEBITMAP || detail->id == TYPETEXT) {
        unsigned int* len;
        unsigned char* r = serialize_object_internal(detail->entity, len);
        *size = 2*sizeof(int) + *len;
        struct buffer_ref* ref = malloc(*size); //bug
        write_int(ref, detail->type);
        write_n(ref, r ,*len);
        write_int(ref, detail->script_handle);
        free(r);
        return extract_buffer(ref);
    }
    return NULL;
}

struct object_bitmap* unserialize_bitmap(unsigned char* origin, unsigned int size) {
    unsigned int pixels_len;
    struct object_bitmap* bitmap = malloc(sizeof(struct object_bitmap));
    struct buffer_ref* ref = prepare_buffer(origin, size);
    bitmap->type = read_int(ref);
    bitmap->has_color = read_int(ref);
    bitmap->width = read_int(ref);
    bitmap->height = read_int(ref);
    bitmap->is_free = malloc(bitmap->width * bitmap->height/8);
    read_n(ref, bitmap->is_free , bitmap->width * bitmap->height/8);
    if(bitmap->has_color) {
        pixels_len = bitmap->width * bitmap->height * 4;

    }
    else {
        pixels_len = bitmap->width * bitmap->height/8;
    }
    bitmap->pixels = malloc(pixels_len);
    read_n(ref, bitmap->pixels, pixels_len);
    free(ref);
    return bitmap;
}
struct object_map* unserialize_map(unsigned char* origin, unsigned int size) {
    return (struct object_map*)unserialize_bitmap(origin, size);
}

struct object_text* unserialize_text(unsigned char* origin, unsigned int size) {
    struct object_text* result = malloc(sizeof(struct object_text));
    result->type = *(unsigned int*)origin;
    if(size < sizeof(int)) {
        return NULL;
    }
    result->text = malloc(size-sizeof(int));
    memcpy(result->text,origin+sizeof(int),size-sizeof(int));
    return result;

}

struct object_item* unserialize_item(unsigned char* origin, unsigned int size) {
    struct buffer_ref* ref = prepare_buffer(origin, size);
    struct object_item* detail = malloc(sizeof(struct object_item));

    detail->type = read_int(ref);
    detail->id = read_int(ref);
    if(detail->id == TYPEBITMAP || detail->id == TYPETEXT) {
        unsigned char* tmp = malloc(size - 3 * (sizeof(int)));
        read_n(ref, tmp, size - 3 * sizeof(int));
        detail->entity = unserialize_object_internal(tmp,size - 3 * sizeof(int));
        detail->script_handle = read_int(ref);
        free(ref);
        return detail;
    }
    return NULL;
}


unsigned char* serialize_object_internal(struct object_detail* detail, unsigned int* size) {
    switch(detail->type) {
        case TYPEBITMAP:
            return serialize_bitmap((struct object_bitmap*)detail, size);
        case TYPEMAP:
            return serialize_map((struct object_map*)detail, size);
        case TYPETEXT:
            return serialize_text((struct object_text*)detail, size);
        case TYPEITEM:
            return serialize_item((struct object_item*)detail, size);
        default:
            print_err("type unknown");
    }
}

struct object_detail* unserialize_object_internal(unsigned char* origin, unsigned int size) {
    switch(*(unsigned int*)origin) {
        case TYPEBITMAP:
            return (struct object_detail*)unserialize_bitmap(origin, size);
        case TYPEMAP:
            return (struct object_detail*)unserialize_map(origin,size);
        case TYPETEXT:
            return (struct object_detail*)unserialize_text(origin, size);
        case TYPEITEM:
            return (struct object_detail*)unserialize_item(origin, size);
        default:
            print_err("type unknown");
    }
}

/*
 * global game object list op
 */

void insert_game_object(struct game_object* obj) {
    struct game_object* tmp = global_objects;
    struct game_object* pre = tmp;
    if(tmp == 0) {
        global_objects = obj;
        return;
    }
    while(tmp != 0) {
        if(tmp->z_index >= obj->z_index) {
            if(tmp == global_objects) {
                global_objects = obj;
                obj->next = tmp;
            }
            else {
                obj->next = tmp;
                pre->next = obj;
            }
            return;
        }
        if(tmp->next == 0) {
            tmp->next = obj;
            return;
        }
        pre = tmp;
        tmp = tmp->next;
    }
}

struct game_object* find_game_object(unsigned int id) {
    struct game_object* tmp = global_objects;
    while(tmp != 0) {
        if(tmp->id == id) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

struct game_object* find_game_object_by_resource_id(unsigned int id) {
    struct game_object* tmp = global_objects;
    while(tmp != 0) {
        if(tmp->resource_id == id) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

void delete_game_object(unsigned int id) {
    struct game_object* obj = find_game_object(id);
    if(!obj) {
        obj->needed_removed = 1;
    }
}
/*
 * object create and destory
 */

struct game_object* create_game_object(unsigned int x, unsigned int y, unsigned int z_index, struct object_detail* detail) {
    struct game_object* obj = malloc(sizeof(struct game_object));
    if(detail->type == TYPEBITMAP || detail->type == TYPEMAP) {
        obj->resource_id = global_resource_id++;
    }
    obj->x = x;
    obj->y = y;
    obj->z_index = z_index;
    obj->detail = detail;
    obj->id = global_id++;
    obj->is_show = 1;
    obj->is_updated = 1;
    obj->is_added = 0;
    obj->needed_removed = 0;
}

void destory_detail(struct object_detail* detail) {
    switch(detail->type) {
        case TYPETEXT:
            free(((struct object_text*)detail)->text);
            break;
        case TYPEITEM:
            destory_detail(((struct object_item*)detail)->entity);
            break;
        case TYPEBITMAP:
            free(((struct object_bitmap*)detail)->pixels);
            free(((struct object_bitmap*)detail)->is_free);
            break;
        case TYPEMAP:
            free(((struct object_map*)detail)->pixels);
            free(((struct object_map*)detail)->if_free);
            break;
        default:
            break;
    }
    free(detail);
}

void destory_game_object(struct game_object* obj) {
    destory_detail(obj->detail);
    free(obj);
}



struct object_map* wrap_map(unsigned int width, unsigned int height, unsigned int has_color, unsigned char* pixels) {
    struct object_map* obj =  malloc(sizeof(struct object_map));
    obj->type = TYPEMAP;
    obj->width = width;
    obj->height = height;
    obj->has_color = has_color;
    obj->pixels = pixels;
    return obj;
}

struct object_bitmap* wrap_bitmap(unsigned int width, unsigned int height, unsigned int has_color, unsigned char* pixels) {
    struct object_bitmap* obj =  malloc(sizeof(struct object_bitmap));
    obj->type = TYPEBITMAP;
    obj->width = width;
    obj->height = height;
    obj->has_color = has_color;
    obj->pixels = pixels;
    obj->is_free = calloc(1,width * height/8);
    return obj;
}

struct object_text* wrap_text(unsigned char* text) {
    struct object_text* obj = malloc(sizeof(struct object_text));
    obj->type = TYPETEXT;
    obj->text = text;
    return obj;
}

struct object_item* wrap_item(struct object_detail* entity, unsigned int script_handle) {
    struct object_item* obj = malloc(sizeof(struct object_item));
    obj->type = TYPEITEM;
    obj->entity = entity;
    obj->script_handle = script_handle;
    return obj;
}