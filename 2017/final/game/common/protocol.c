/*
 * Lat's create magic!
 */
#include "protocol.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*
 * struct section {
    int type;
    unsigned int size;
    unsigned char* data;
    int crc32;
};
 */

struct section* serialize_section(struct message* msg) {
    struct section* result = malloc(sizeof(struct section));
    result->type = msg->type;
    switch (result->type) {
        case PROTOSTOCADD: {
            /*
            struct s2c_add_object {
                unsigned int number; //number of the object to add
                struct game_object** objs;
            };
            */
            unsigned int total_size = sizeof(int);
            unsigned int size;
            result->data = malloc(sizeof(int));
            struct s2c_add_object *s2c_add = (struct s2c_add_object *) msg->detail;
            *(unsigned int *) result->data = s2c_add->number;
            for (int i = 0; i < s2c_add->number; i++) {
                unsigned char *data = serialize_object(s2c_add->objs[i], &size);
                result->data = realloc(result->data, total_size + size + sizeof(int));
                *((unsigned int *) (result->data + total_size)) = size;
                memcpy(result->data + total_size + sizeof(int), data, size);
                total_size += size + sizeof(int);
            }
            result->size = total_size;
            break;
        }
        case PROTOSTOCDEL: {
            /*
            struct s2c_delete_object {
                unsigned int number; //number of the object to delete
                int* ids;
            };
            */
            struct s2c_delete_object* s2c_del = (struct s2c_delete_object*)msg->detail;
            result->data = malloc((s2c_del->number+1) * sizeof(int));
            *(unsigned int*)result->data = s2c_del->number;
            for(int i = 0;i < s2c_del->number;i++) {
                *(((unsigned int*)result->data)+i+1) = s2c_del->ids[i];
            }
            result->size = s2c_del->number * sizeof(int);
            break;
        }
        case PROTOSTOCUPDATE: {
            /*
            struct s2c_update_object {
                unsigned int number; // number of the object to update
                int* ids;
                struct game_object** objs;
            };
            */
            struct s2c_update_object *s2c_update = (struct s2c_update_object *) msg->detail;
            unsigned int total_size = 0;
            unsigned int size;
            result->data = malloc((s2c_update->number + 1) * sizeof(int));
            *(unsigned int *) result->data = s2c_update->number;
            for (int i = 0; i < s2c_update->number; i++) {
                *(((unsigned int *) result->data) + i + 1) = s2c_update->ids[i];
            }
            total_size = (s2c_update->number+1) * sizeof(int);
            for (int i = 0; i < s2c_update->number; i++) {
                unsigned char *data = serialize_object(s2c_update->objs[i], &size);
                result->data = realloc(result->data, total_size + size + sizeof(int));
                *(unsigned int *) (result->data + total_size) = size;
                memcpy(result->data + total_size + sizeof(int), data, size);
                total_size += (size+sizeof(int));
            }
            result->size = total_size;
            break;
        }
        case PROTOCTOSMOD: {
            /*
            struct c2s_mod {
                unsigned int resource_id;
                struct object_detail* detail;
                int length;
            };
            */
            unsigned int size;
            struct c2s_mod *_c2s_mod = (struct c2s_mod *) msg->detail;
            result->data = malloc(2 * sizeof(int));
            *(unsigned int *) (result->data) = _c2s_mod->resource_id;
            unsigned char *obj = serialize_object_internal(_c2s_mod->detail, &size);
            *(((unsigned int *) result->data) + 1) = _c2s_mod->resource_id;
            result->data = realloc(result->data, size + 2 * sizeof(int));
            memcpy(result->data + 2 * sizeof(int), obj, size);
            result->size = size + 2 * sizeof(int);
            break;
        }
        case PROTOCTOSKEY: {
            struct c2s_key* _c2s_key = (struct c2s_key*) msg->detail;
            result->data = malloc(1);
            *result->data = _c2s_key->keycode;
            result->size = 1;
            break;
        }
        case PROTOCTOSTEXT: {
            struct c2s_text* _c2s_text = (struct c2s_text*) msg->detail;
            result->data = malloc(_c2s_text->length + sizeof(int));
            *(unsigned int*)_c2s_text = _c2s_text->length;
            memcpy(result->data+sizeof(int), _c2s_text->text, _c2s_text->length);
            break;
        }
        default:
            print_err("error message type");
    }
    return result;
}

struct message* unserialize_section(struct section* sec) {
    struct buffer_ref* ref = prepare_buffer(sec->data, sec->size);
    struct message* result = malloc(sizeof(struct message));
    result->type = sec->type;
    switch (result->type) {
        case PROTOSTART:
        case PROTOEND:
            return result;
            break;
        case PROTOSTOCADD: {
            /*
            struct s2c_add_object {
                unsigned int number; //number of the object to add
                struct game_object** objs;
            };
            */
            struct s2c_add_object *s2c_add = malloc(sizeof(struct s2c_add_object));
            s2c_add->number = read_int(ref);
            s2c_add->objs = malloc(sizeof(struct game_object *) * s2c_add->number);
            for (int i = 0; i < s2c_add->number; i++) {
                unsigned size = read_int(ref);
                unsigned char *buffer = malloc(size);
                read_n(ref, buffer, size);
                s2c_add->objs[i] = unserialize_object(buffer, size);
                free(buffer);
            }
            result->detail = (void *) s2c_add;
            break;
        }
        case PROTOSTOCDEL: {
            /*
            struct s2c_delete_object {
                unsigned int number; //number of the object to delete
                int* ids;
            };
            */
            struct s2c_delete_object *s2c_del = malloc(sizeof(struct s2c_delete_object));
            s2c_del->number = read_int(ref);
            s2c_del->ids = malloc(sizeof(int) * s2c_del->number);
            for (int i = 0; i < s2c_del->number; i++) {
                s2c_del->ids[i] = read_int(ref);
            }
            result->detail = (void *) s2c_del;
            break;
        }
        case PROTOSTOCUPDATE: {
            /*
            struct s2c_update_object {
                unsigned int number; // number of the object to update
                int* ids;
                struct game_object** objs;
            };
            */
            struct s2c_update_object *s2c_update = malloc(sizeof(struct s2c_update_object));
            s2c_update->number = read_int(ref);
            s2c_update->ids = malloc(sizeof(int) * s2c_update->number);
            for (int i = 0; i < s2c_update->number; i++) {
                s2c_update->ids[i] = read_int(ref);
            }
            s2c_update->objs = malloc(sizeof(struct game_object *) * s2c_update->number);
            for (int i = 0; i < s2c_update->number; i++) {
                unsigned int size = read_int(ref);
                unsigned char *buffer = malloc(size);
                read_n(ref, buffer, size);
                s2c_update->objs[i] = unserialize_object(buffer, size);
                free(buffer);
            }
            result->detail = (void *) s2c_update;
            break;
        }
        case PROTOCTOSMOD: {
            /*
            struct c2s_mod {
                unsigned int resource_id;
                struct object_detail* detail;
                int length;
            };
            */
            struct c2s_mod *_c2s_mod = malloc(sizeof(struct c2s_mod));
            _c2s_mod->resource_id = read_int(ref);
            _c2s_mod->length = read_int(ref);
            unsigned char* buffer = malloc(_c2s_mod->length);
            read_n(ref, buffer, _c2s_mod->length);
            struct object_detail *obj = unserialize_object_internal(buffer, _c2s_mod->length);
            free(buffer);
            _c2s_mod->detail = obj;
            result->detail = (void *) _c2s_mod;
            break;
        }
        case PROTOCTOSKEY: {
            struct c2s_key* _c2s_key = malloc(sizeof(struct c2s_key));
            read_n(ref, &_c2s_key->keycode, 1);
            result->detail = (void*) _c2s_key;
            break;
        }
        case PROTOCTOSTEXT: {
            struct c2s_text* _c2s_text = malloc(sizeof(struct c2s_text));
            _c2s_text->length = read_int(ref);
            _c2s_text->text = malloc(_c2s_text->length);
            read_n(ref, _c2s_text->text, _c2s_text->length);
            result->detail = (void*) _c2s_text;
            break;
        }
        default:
            print_err("error when unserialize message");
    }
    free(ref);
    return result;
}

int recv_section(int fd, struct section* sec) {
    read_check(fd, (unsigned char*)&sec->type,sizeof(int));
    read_check(fd, (unsigned char*)&sec->size,sizeof(int));
    if(sec->size > 0x1000000) {
        _exit(1);
    }
    sec->data = malloc(sec->size);
    int len = sec->size;
    int recved = 0;
    while(recved != len) {
        recved += read_check(fd, sec->data+recved, len-recved);
    }
    read_check(fd, (unsigned char*)&sec->crc32,sizeof(int));
    return sec->type;
}

void send_section(int fd, struct section* sec) {
    unsigned char* buffer = malloc(3*sizeof(int)+sec->size);
    struct buffer_ref* ref = prepare_buffer(buffer,3*sizeof(int)+sec->size);
    write_int(ref, sec->type);
    write_int(ref, sec->size);
    write_n(ref, sec->data, sec->size);
    write_int(ref, 0); // crc surpport later
    write(fd, buffer, 3*sizeof(int)+sec->size);
    free(buffer);
    free(ref);
}

struct message* main_loop(int fd, void (*handle)(struct message* msg)) {
    struct section* tmp = malloc(sizeof(struct section));
    client_fd = fd;
    while(1) {
        recv_section(fd, tmp);
        struct message *msg = unserialize_section(tmp);
        handle(msg);
        usleep(300000);
    }
}


void send_message(struct message* message) {
    struct section* sec = serialize_section(message);
    send_section(client_fd, sec);
    free(sec);
}

void init_all() {
    global_id = 0;
    global_resource_id = 0;
    global_objects = 0;
}

int render_lock = 0;
void render_all() {
    if(render_lock) {
        return;
    }
    render_lock = 1;
    // find all needed to add
    struct game_object* obj = global_objects;
    struct message* msg = malloc(sizeof(struct message));
    msg->type = PROTOSTOCADD;
    struct s2c_add_object* proto_add = calloc(1,sizeof(struct s2c_add_object));
    unsigned int size = 0;
    while(obj != 0) {
        if(!obj->is_added) {
            size += 1;
            proto_add->objs = realloc(proto_add->objs, sizeof(struct game_object*) * size);
            proto_add->objs[size-1] = obj;
            obj->is_added = 1;
        }
        obj = obj->next;
    }
    proto_add->number = size;
    msg->detail = proto_add;
    if(proto_add->number != 0) {
        send_message(msg);
    }
    free(proto_add->objs);
    free(proto_add);
    free(msg);

    // find all needed to update
    obj = global_objects;
    msg = malloc(sizeof(struct message));
    msg->type = PROTOSTOCUPDATE;
    struct s2c_update_object* proto_update = calloc(1,sizeof(struct s2c_update_object));
    size = 0;
    while(obj != 0) {
        if(!obj->is_updated) {
            size += 1;
            proto_update->ids = realloc(proto_update->ids, sizeof(int)*size);
            proto_update->objs = realloc(proto_update->objs, sizeof(struct game_object*) * size);
            proto_update->objs[size-1] = obj;
            proto_update->ids[size-1] = obj->id;
            obj->is_updated = 1;
        }
        obj = obj->next;
    }
    proto_update->number = size;
    msg->detail = proto_update;
    if(proto_update->number != 0) {
        send_message(msg);
    }
    free(proto_update->objs);
    free(proto_update);
    free(msg);

    //remove all object
    obj = global_objects;
    struct game_object* pre = obj;
    msg = malloc(sizeof(struct message));
    msg->type = PROTOSTOCDEL;
    struct s2c_delete_object* proto_del = (struct s2c_delete_object*)calloc(1,sizeof(struct s2c_delete_object));
    size = 0;
    while(obj != 0) {
        if(obj->needed_removed) {
            size += 1;
            proto_del->ids = realloc(proto_del->ids, sizeof(int) * proto_del->number);
            proto_del->ids[size - 1] = obj->id;
            // destory gameobj
            if (obj == global_objects) {
                global_objects = global_objects->next;
                destory_game_object(obj);
            } else {
                pre->next = obj->next;
                destory_game_object(obj);
            }
            obj = pre->next;
        } else {
            pre = obj;
            obj = obj->next;
        }
    }
    proto_del->number = size;
    msg->detail = proto_del;
    if(proto_del->number != 0) {
        send_message(msg);
    }
    free(proto_del->ids);
    free(proto_del);
    free(msg);
    render_lock = 0;
}