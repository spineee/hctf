struct chunk {
    unsigned int size;
    unsigned int pre_size;
    struct chunk* fd;
};
#define next_chunk(current) ((current->size&0xfffffffe)+(unsigned char*)current+12) 
#define next_size(current) (*(int*)((current->size&0xfffffffe)+(unsigned char*)current+12))
#define find_index(size) ((size/0x10) > 0x20 ? 0x1f : (size/0x10))
#define inuse 1
#define to_mem(s) ((unsigned char*)s)
#define to_chunk(s) ((struct chunk*)(s-12))
struct chunk* bins[0x20] = { 0 }; 
struct chunk* top = 0;
void* malloc(unsigned int size) {
    //align
    size = ((size+3)/4)*4;
    unsigned int index = find_index(size);
    struct chunk* current = bins[index];
    struct chunk* next, * prev = current;
    // try to find memory from bins
    while(current != 0) {
        if(current->size >= size) {
            prev->fd = current->fd;
            // add remainder to bins
            if(current->size - size > 12) {
                current->size = size | inuse;
                next = next_chunk(current);
                next->size = (current->size - size - 12) | inuse;
                index = find_index(next->size);
                next->fd = bins[index];
                bins[index] = next;
            } 
            return (void*)(((char*)current)+12);
        }
        prev = current;
        current = current->fd;
    }
    
// try to get memory from top chunk
alloc_from_top:
    current = top;
    if(current->size >= size) {
        top = to_mem(current) + size + 12;
        top->size = (current->size - size - 12) | inuse;
        current->size = size | inuse;
        current->fd = 0;
        return (void*)(((char*)current)+12);
    }
    //alloc memory from memory.grow
    unsigned int needed;
    if(top) {
        needed = (size + 12 - top->size) > 0x10000 ? (size + 12 - top->size)/0x10000 : 1;
    }
    else {
        needed = size > 0x10000 ? size/0x10000 : 1;
    }
    struct chunk* new_mem = grow(needed);
    if (top) {
        top -> size += needed * 0x10000;
    }
    else {
        new_mem->size = (needed*0x10000-12) | inuse;
        new_mem->fd = 0;
        top = new_mem;
    }
    goto alloc_from_top;
}

void unlink(struct chunk* current) {
    int index = find_index(current->size);
    struct chunk* ite = bins[index];
    if(bins[index] != 0) {
        while(ite->fd != 0) {
            if(ite->fd == current) {
                ite->fd = current->fd;
                break;
            }
            ite = ite -> fd;
        }
    }
}

void free(unsigned char* ptr) {
    struct chunk* current = to_chunk(ptr);
    struct chunk* next = next_chunk(current);
    if(!(current->size & 1)) {
        struct chunk* pre = to_mem(current) - current->pre_size - 12;
        pre->size += ((current->size&0xfffffffe) + 12);
        // unlink pre
        unlink(pre);
        current = pre;
    }
    // check if next is top chunk
    if(next == top) {
        current->size += ((next->size&0xfffffffe) + 12);
        top = current;
        return;    
    }

    if(!(next_size(next) & 1)) {
        // merge current and next
        current->size += ((next->size&0xfffffffe) + 12);
    }
    // link current to bins
    int index = find_index(current->size);
    current->fd = bins[index];
    bins[index] = current;
    // clear next chunk's inuse bit and set the pre_size
    next = next_chunk(current);
    next->size &= 0xfffffffe;
    next->pre_size = current->size&0xfffffffe;
}

