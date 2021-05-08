/*
 *     libclz - compact single header file utility library
 *     Copyright (C) 2020  Lorenzo Calza
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file dynarray.h
 * @author Lorenzo Calza
 * @date 13 Nov 2020
 * @brief Header file containing the declarations for a dynamic array and its related functionalities
 *
 * This header file contains the definition of the @ref dynarray type and the functions which make up
 * the API to use it.
 *
 * **Implementation**
 *
 * The implementation can be included if the macro `CLZ_DYNARRAY_IMPL` is defined beforehand. Some functionalities
 * are implemented as macros for performance reasons.
 */

#ifndef _CLZ_DYNARRAY_H
/**
 * @brief Include guard for this file.
 */
#define _CLZ_DYNARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clz.h"
#include "def.h"

/**
 * @brief Definition of structure representing a dynamic array.
 *
 * The recommended way to interact with this struct is by using the API functions and macros.
 * See the examples section for a tutorial on how to use this.
 *
 * **Notes**
 *
 * **Do not, under any circumstances, modify and values or pointers belonging to member fiends of this struct**.
 * Use the provided API instead!
 *
 */
typedef struct dynarray {
    /**
     * @brief The size of the allocation buffer
     *
     * This value is initialized to the value @ref CLZ_DYNARRAY_ALLOC, and increased dynamically
     * as soon as the data size surpasses the allocation. Every time this happens, the previous value
     * for the buffer size is doubled.
     *
     * **Notes**
    *
    * This value contains the buffer size in units of `sizeof(void *)`. This value can be checked safely
    * with the macro @ref dynarray_alloc_size.
     *
     * @see dynarray_alloc_size, CLZ_DYNARRAY_ALLOC
     */
    size_t alloc_size;
    /**
     * @brief The size of the array
     *
     * This value is initialized to `0`, and increased dynamically
     * as the data grows, or decreased as the data shrinks.
     *
     * **Notes**
     *
     * This value contains the number of elements of the array. This value can be checked safely with the macro
     * @ref dynarray_length.
     *
     * @see dynarray_length
     */
    size_t data_size;
    /**
     * @brief The array
     *
     * This variable holds a dynamically allocated (read: `malloc` family) array of `void *`-pointers. You can use
     * the function @ref dynarray_get or the macro @ref dynarray_at.
     *
     * @see dynarray_get, dynarray_at
     */
    void **ptr;
    /**
     * @brief Internal variable holding the last index of the @ref dynarray_find_next search function.
     *
     * **Notes**
     *
     * This is an internal variable that is not meant to be accessed in any way. Its value is initialised to @ref CLZ_FIND_INDEX_START.
     * It can and should be reset to that value with the macro @ref dynarray_find_reset after the @ref dynarray_find_next search is concluded.
     *
     * @see dynarray_find_next, dynarray_find_reset
     */
    int find_index;
} dynarray;

/**
 * @brief Initializes a new @ref dynarray struct.
 *
 * This function makes use of dynamic allocation (see `malloc` family). As such, it could _fail_
 * if heap allocation fails.
 *
 * **Notes**
 *
 * When finished using it, the function @ref dynarray_free must be invoked to free the internal allocations
 * made by this function.
 *
 * This function (if successful) returns the `ptr` buffer (see @ref dynarray). It does **not** return a pointer
 * to the @ref dynarray, which is a big difference from the @ref dynarray_new function.
 *
 * @param d The pointer to the array struct
 * @return The pointer to the buffer array (`dynarray.ptr`) if successful, `NULL` otherwise
 *
 * @see dynarray_new, @see dynarray_free
 */
void *dynarray_init(dynarray *d);
/**
 * @brief Allocates a new `dynarray` struct object on the heap and returns its pointer.
 *
 * Since the `malloc` family is used internally, heap allocation may fail. In this case, `NULL` is returned.
 *
 * **Notes**
 *
 * When finished using it, the function @ref dynarray_free must be invoked to free the internal allocations
 * made by this function.
 *
 * @return The pointer to the @ref dynarray if successful, `NULL` otherwise
 *
 * @see dynarray_init, @see dynarray_free
 */
dynarray *dynarray_new();
/**
 * @brief Frees the internal allocations of the dynarray
 *
 * When the dynarray is no longer used, this function should be invoked for cleanup.
 *
 * It is also possible to deallocate (`free`) the pointer-elements of the array by passing the value `true` to the (`stdbool.h`) `bool`
 * type variable `deep`.
 *
 * **Notes**
 *
 * If the @ref dynarray itself was allocated on the heap, it must be freed separately **after** @ref dynarray_free has been called,
 * as this function does not free it.
 * Example:
 *
 *     dynarray *d = dynarray_new();
 *     // ...
 *     dynarray_free(d);
 *     free(d);
 *
 * The reason why this behaviour is intended is that it is also possible to create a statically allocated @ref dynarray like this:
 *
 *     dynarray d;
 *     dynarray_init(&d);
 *     // ...
 *     dynarray_free(&d);
 *     free(&d) // WRONG! //
 *
 * See the examples for more information.
 *
 * @param d The @ref dynarray to free.
 * @param deep Whether or not to invoke `free()` on the elements of the array
 */
void dynarray_free(dynarray *d, bool deep);

/**
 * @brief Append element after the end of the array
 *
 * This function appends the given pointer element after the end of the array. If the allocated buffer is full, the
 * buffer is extended to account for the required memory. The initial buffer size is defined in @ref CLZ_DYNARRAY_ALLOC,
 * and the size is doubled every time the buffer needs to be extended.
 *
 * **Notes**
 *
 * If the buffer needs to be extended, this is done by dynamic heap allocation. The functions that are part of the `malloc`
 * family may fail and return `NULL`. If this is the case, `NULL` is returned to indicate failure. In case of success, `obj`
 * itself is returned.
 *
 * @param d The @ref dynarray
 * @param obj The pointer that needs to be appended after the end
 * @return `NULL` in case of failure, `obj` otherwise
 *
 * @see dynarray_alloc_size
 */
void *dynarray_append(dynarray *d, void *obj);
/**
 * @brief Set element by index
 *
 * This function overwrites an element of the array. `index` is required to be less than the length of the array, otherwise
 * the operation is attempted out of bounds. In this case, the function will fail to accomplish the goal. Even if `index` is equal
 * to the array length, the object is **not** appended.
 *
 * **Notes**
 *
 * `index` must be the index of an element already in the array, otherwise the function will not perform the
 * operation.
 *
 * @param d The @ref dynarray
 * @param index The element index in the array
 * @param obj The pointer that needs to be appended after the end
 * @return `NULL` in case of failure, `obj` otherwise
 *
 * @see dynarray_append, dynarray_get, dynarray_length
 */
void *dynarray_set(dynarray *d, size_t index, void *obj);
/**
 * @brief Get element by index
 *
 * This function retrieves an element from the array. `index` is required to be less than the length of the array, otherwise
 * the operation is attempted out of bounds. In this case, the function will fail to accomplish the goal and `NULL` is
 * returned.
 *
 * **Notes**
 *
 * `index` must be the index of an element already in the array, otherwise the function will not perform the
 * operation. Since `NULL` is technically a valid entry, checking whether an index is valid by comparing this function's
 * return value and `NULL` does not work. Even if you are certain that no element of the array may be `NULL`, this is
 * not worth it. See the macro @ref dynarray_valid_index for that.
 *
 * We have also provided the macro @ref dynarray_at that performs the same operation and is slightly more performant, however this function is type-safe
 * and performs index bounds checking, whereas the macro's usage results in undefined behaviour if the index is invalid.
 *
 * @param d The @ref dynarray
 * @param index The element index in the array
 * @return `NULL` in case of failure, the entry pointer otherwise
 *
 * @see dynarray_append, dynarray_at, dynarray_length
 */
void *dynarray_get(dynarray *d, size_t index);

/**
 * @brief Removes the first occurrence of the pointer within the array
 *
 * If the element is not found within the array, `false` is returned.
 *
 * **Notes**
 *
 * The object is not deallocated. This function also reallocates the element array using `realloc`. If `realloc` fails, the operation
 * is aborted and `false` is returned. The buffer is not shrunk.
 *
 * @param d The @ref dynarray
 * @param obj The entry to remove
 * @return `true` if the entry could be deleted, `false` otherwise
 *
 * @see dynarray_remove_all, @ref dynarray_remove_index
 */
bool dynarray_remove_first(dynarray *d, void *obj);
/**
 * @brief Removes all occurrences of the pointer within the array
 *
 * If the element is not found anywhere within the array, `false` is returned.
 *
 * **Notes**
 *
 * The object is not deallocated. This function also reallocates the element array using `realloc`. If `realloc` fails, the operation
 * is aborted and `false` is returned. The buffer is not shrunk.
 *
 * @param d The @ref dynarray
 * @param obj The entry to remove
 * @return `true` if the entry could be deleted, `false` otherwise
 *
 * @see dynarray_remove_first, @ref dynarray_remove_index
 */
bool dynarray_remove_all(dynarray *d, void *obj);
/**
 * @brief Removes an element pointer within the array by index
 *
 * If the index is out of bounds, `false` is returned.
 *
 * **Notes**
 *
 * The object is not deallocated. This function also reallocates the element array using `realloc`. If `realloc` fails, the operation
 * is aborted and `false` is returned. The buffer is not shrunk.
 *
 * @param d The @ref dynarray
 * @param index The index of the entry to remove
 * @return `true` if the entry could be deleted, `false` otherwise
 *
 * @see dynarray_remove_first, @ref dynarray_remove_all, @ref dynarray_pop
 */
bool dynarray_remove_index(dynarray *d, size_t index);

/**
 * @brief Clears the array contents
 *
 * This function clears all array contents. The `bool` (C99, `stdbool.h`) parameter indicates
 * whether or not to free the entries.
 *
 * **Notes**
 *
 * If the same pointer is contained by the array multiple times, passing `true` to `tofree` will fail
 * and the program will crash because this function will attempt to free it twice. Use `true` for `tofree`
 * at your own risk.
 *
 * This function does **not** reset buffer size to its default value. If you wish to wipe the data and create a
 * new array with initial buffer size @ref CLZ_DYNARRAY_ALLOC, you probably need to deallocate the entire array
 * with @ref dynarray_free and create a new one.
 *
 * @param d The dynarray
 * @param tofree Whether or not to free the pointer-elements of the array
 *
 * @ref dynarray_free, dynarray_remove_all
 */
void dynarray_clear(dynarray *d, bool tofree);

/**
 * @brief Return position of first occurrence of the pointer in the array.
 *
 * This function returns the index of the first occurrence of `obj` in `d`. If `obj` is not found anywhere,
 * @ref CLZ_NOT_FOUND is returned.
 *
 * @param d The @ref dynarray
 * @param obj The pointer-entry to look for
 * @return The position of the element
 *
 * @see dynarray_find_next, dynarray_find_reset
 */
int dynarray_find_first(dynarray *d, void *obj);
/**
 * @brief Return position of next occurrence of the pointer in the array.
 *
 * This function returns the index of the next occurrence of `obj` in `d`. If `obj` is not found anywhere,
 * @ref CLZ_NOT_FOUND is returned.
 *
 * The idea is that you can continue your search from the position where the last item was found.
 *
 * Example:
 *
 *     dynarray *d = dynarray_new();
 *     // ...
 *     int pos;
 *     while ((pos = dynarray_find_next(d, obj)) != CLZ_NOT_FOUND) {
 *         printf("Found obj at %d\n", pos);
 *     }
 *     dynarray_find_reset(d);
 *     // ...
 *     dynarray_free(d);
 *
 * The macro @ref dynarray_find_reset resets the search so that the dynarray is ready by the next time.
 * If this macro is not invoked, the search will resume from the last index where a match was found. If the last search
 * yielded @ref CLZ_NOT_FOUND, the search is not reset automatically and any further searches will yield @ref CLZ_NOT_FOUND
 * until the search is reset.
 *
 * See the tutorial for more information.
 *
 * @param d The @ref dynarray
 * @param obj The pointer-entry to look for
 * @return The position of the element
 *
 * @see dynarray_find_first, dynarray_find_reset
 */
int dynarray_find_next(dynarray *d, void *obj);

/**
 * @brief Iterate through the array and performs a predefined operation on each pointer-element
 *
 * This function performs a trivial iteration across all array elements and invokes the @ref clz_consumer function `c` which was
 * passed to the function on every entry pointer.
 *
 * @param d The @ref dynarray
 * @param c The consumer which needs to be invoked on each element
 *
 * @see clz_consumer, dynarray_foreach_if, dynarray_foreach_if_else
 */
void dynarray_foreach(dynarray *d, clz_consumer c);
/**
 * @brief Iteratex through the array and performs a predefined operation on each pointer-element if a certain condition
 * applies
 *
 * This function performs a trivial iteration across all array elements and invokes the @ref clz_consumer function `c` which was
 * passed to the function on every entry pointer if and only if the @ref clz_predicate function `p` returns the `bool` (C99,
 * `stdbool.h`) value `true`.
 *
 * @param d The @ref dynarray
 * @param p The predicate which represents the `if`-conditional
 * @param c The consumer which needs to be invoked on each element if the predicate evaluates to `true`
 *
 * @see clz_consumer, clz_predicate, dynarray_foreach, dynarray_foreach_if_else
 */
void dynarray_foreach_if(dynarray *d, clz_predicate p, clz_consumer c);
/**
 * @brief Iteratex through the array and performs an "if"-operation on each pointer-element if a certain condition
 * applies, and an "else"-operation otherwise
 *
 * This function performs a trivial iteration across all array elements and invokes the @ref clz_consumer function `ifc` which was
 * passed to the function on every entry pointer if and only if the @ref clz_predicate function `p` returns the `bool` (C99,
 * `stdbool.h`) value `true`. If the predicate fails, the `elsec` consumer function is invoked on the entry-pointer.
 *
 * @param d The @ref dynarray
 * @param p The predicate which represents the `if`-conditional
 * @param ifc The consumer which needs to be invoked on each element if the predicate evaluates to `true`
 * @param elsec The consumer which needs to be invoked on each element if the predicate evaluates to `false`
 *
 * @see clz_consumer, clz_predicate, dynarray_foreach, dynarray_foreach_if
 */
void dynarray_foreach_if_else(dynarray *d, clz_predicate p, clz_consumer ifc, clz_consumer elsec);

/**
 * @brief Removes the last element pointer within the array and returns it
 *
 * If the array is empty, `NULL` is returned. Keep in mind that `NULL` is a valid entry, as such comparing the return
 * value to `NULL` to check whether the list is empty may not be a good idea, depending on whether `NULL` pointers could
 * have been added to the array.
 *
 * **Notes**
 *
 * The object is not deallocated and the buffer is not shrunk.
 *
 * @param d The @ref dynarray
 * @return The entry that was removed or `NULL` in case of failure
 *
 * @see dynarray_get, dynarray_at, dynarray_remove_first, @ref dynarray_remove_index
 */
void *dynarray_pop(dynarray *d);

#define CLZ_DYNARRAY_ALLOC 8

#define dynarray_length(d) ((d)->data_size)
#define dynarray_alloc_size(d) ((d)->alloc_size)
#define dynarray_find_reset(d) ((d)->find_index = CLZ_FIND_INDEX_START)
#define dynarray_at(d, index) (*((d)->ptr + (index)))
#define dynarray_valid_index(d, index) (dynarray_length(d) > (index))

#endif

#ifdef CLZ_DYNARRAY_IMPL

void *dynarray_init(dynarray *d) {
    d->alloc_size = CLZ_DYNARRAY_ALLOC;
    d->data_size = 0;
    d->ptr = (void **) calloc(CLZ_DYNARRAY_ALLOC, sizeof(void *));
    if (d->ptr == NULL) return NULL;
    d->find_index = CLZ_FIND_INDEX_START;
    return (void *) d->ptr;
}

dynarray *dynarray_new() {
    dynarray *d = malloc(sizeof(dynarray));
    if (d == NULL) return NULL;
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
            continue;
        }

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

void *dynarray_pop(dynarray *d) {
    if (d->data_size == 0) return NULL;
    void *obj = dynarray_get(d, dynarray_length(d) - 1);
    dynarray_remove_index(d, dynarray_length(d) - 1);
    return obj;
}

#endif
