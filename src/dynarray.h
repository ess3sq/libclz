#define DEBUG(s) printf("(DEBUG) %s\n", s)

#ifndef _CLZ_DYNARRAY_H
#define _CLZ_DYNARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "def.h"

typedef struct _dynarray {
    size_t alloc_size;
    size_t data_size;
    void **ptr;
    int find_index;
} dynarray;

void *dynarray_init(dynarray *d);
dynarray *dynarray_new();
void dynarray_free(dynarray *d, bool deep);

void *dynarray_append(dynarray *d, void *obj);
void *dynarray_set(dynarray *d, size_t index, void *obj);
void *dynarray_get(dynarray *d, size_t index);

bool dynarray_remove_first(dynarray *d, void *obj);
bool dynarray_remove_all(dynarray *d, void *obj);
bool dynarray_remove_index(dynarray *d, size_t index);

void dynarray_clear(dynarray *d, bool tofree);

int dynarray_find_first(dynarray *d, void *obj);
int dynarray_find_next(dynarray *d, void *obj);

void dynarray_foreach(dynarray *d, clz_consumer c);
void dynarray_foreach_if(dynarray *d, clz_predicate p, clz_consumer c);
void dynarray_foreach_if_else(dynarray *d, clz_predicate p, clz_consumer ifc, clz_consumer elsec);

#endif

#ifndef CLZ_DYNARRAY_MACRO
#define CLZ_DYNARRAY_MACRO

#define CLZ_DYNARRAY_ALLOC 8
#define CLZ_FIND_INDEX_START -1
#define CLZ_NOT_FOUND -1

#define dynarray_length(d) ((d)->data_size)
#define dynarray_alloc_size(d) ((d)->alloc_size)
#define dynarray_find_reset(d) ((d)->find_index = CLZ_FIND_INDEX_START)

#endif

#ifdef CLZ_DYNARRAY_IMPL

void *dynarray_init(dynarray *d) {
    d->alloc_size = CLZ_DYNARRAY_ALLOC;
    d->data_size = 0;
    d->ptr = (void **) calloc(CLZ_DYNARRAY_ALLOC, sizeof(void *));
    printf("d->ptr = %p\n", (void*) d->ptr);
    d->find_index = CLZ_FIND_INDEX_START;
    return (void *) d->ptr;
}

dynarray *dynarray_new() {
    dynarray *d = malloc(sizeof(dynarray));
    if( dynarray_init(d)) {
        return d;
    }
    free(d);
    return NULL;
}

void dynarray_free(dynarray *d, bool deep) {
    if (deep) {
        for (int i = 0; i < d->data_size; ++i) {
            free(dynarray_get(d, i));
        }
    }

    free(d->ptr);
}

void *dynarray_append(dynarray *d, void *obj) {
    if (d->alloc_size == d->data_size) {
        void ** new_ptr = (void**) reallocarray(d->ptr, d->alloc_size * 2, sizeof(void *));
        if (new_ptr == NULL) return NULL;
        d->alloc_size *= 2;
        d->ptr = new_ptr;
    }

    *(d->ptr + d->data_size) = obj;
    d->data_size++;
    return obj;
}

void *dynarray_set(dynarray *d, size_t index, void *obj) {
    if (index >= d->data_size) return NULL;
    *(d->ptr + index) = obj;
    return obj;
}

bool dynarray_remove_first(dynarray *d, void *obj) {
    void **ptr_new = (void **) calloc(d->alloc_size, sizeof(void *));
    if (ptr_new == NULL) {
        return false;
    }

    bool done = false;
    size_t index = 0;

    for (int i = 0; i < d->data_size; ++i) {
        if (*(d->ptr + i) == obj && !done) {
            done = true;
            DEBUG("ARRIVED\n");
            continue;
        }

        DEBUG("RUNNING");
        *(ptr_new + index) = *(d->ptr + i);
        ++index;
    }

    free(d->ptr);
    d->ptr = ptr_new;
    d->data_size += done ? -1 : 0;

    return done;
}

bool dynarray_remove_all(dynarray *d, void *obj) {
    void **ptr_new = (void **) calloc(d->alloc_size, sizeof(void *));
    if (ptr_new == NULL) {
        return false;
    }

    size_t index = 0;
    int counter = 0;

    for (int i = 0; i < d->data_size; ++i) {
        if (*(d->ptr + i) == obj) {
            ++counter;
            continue;
        }

        *(ptr_new + index) = *(d->ptr + i);
        ++index;
    }

    free(d->ptr);
    d->ptr = ptr_new;
    d->data_size -= counter;

    return counter > 0;
}

bool dynarray_remove_index(dynarray *d, size_t index) {
    void **ptr_new = (void **) calloc(d->alloc_size, sizeof(void *));
    if (ptr_new == NULL) {
        return false;
    }
    else if (index >= d->alloc_size) return false;

    int j = 0;

    for (int i = 0; i < d->data_size; ++i) {
        if (i == index) {
            continue;
        }

        *(ptr_new + j) = *(d->ptr + i);
        ++j;
    }

    free(d->ptr);
    d->ptr = ptr_new;
    d->data_size += -1;

    return true;
}

void dynarray_clear(dynarray *d, bool tofree) {
    if (tofree) {
        for (int i = 0; i < d->data_size; ++i) {
            free(dynarray_get(d, i));
        }
    }
    d->data_size = 0;
}

void *dynarray_get(dynarray *d, size_t index) {
    if (index >= d->data_size) {
        return NULL;
    }

    return *(d->ptr + index);
}

int dynarray_find_first(dynarray *d, void *obj) {
    for (int i = 0; i < d->data_size; ++i) {
        if (*(d->ptr + i) == obj) {
            return i;
        }
    }

    return CLZ_NOT_FOUND;
}

int dynarray_find_next(dynarray *d, void *obj) {
    for (int i = d->find_index + 1; i < d->data_size; ++i) {
        if (*(d->ptr + i) == obj) {
            d->find_index = i;
            return i;
        }
    }

    return CLZ_NOT_FOUND;
}

void dynarray_foreach(dynarray *d, clz_consumer c) {
    for (int i = 0; i < d->data_size; ++i) {
        c(*(d->ptr + i));
    }
}

void dynarray_foreach_if(dynarray *d, clz_predicate p, clz_consumer c) {
    for (int i = 0; i < d->data_size; ++i) {
        if (p(*(d->ptr + i))) {
            c(*(d->ptr + i));
        }
    }
}

void dynarray_foreach_if_else(dynarray *d, clz_predicate p, clz_consumer ifc, clz_consumer elsec) {
    for (int i = 0; i < d->data_size; ++i) {
        if (p(*(d->ptr + i))) {
            ifc(*(d->ptr + i));
        }
        elsec(*(d->ptr + i));
    }
}

#endif
