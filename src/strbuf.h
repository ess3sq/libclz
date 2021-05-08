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
 * @file strbuf.h
 * @author Lorenzo Calza
 * @date 8 May 2021
 * @brief Header file containing the declarations for a dynamic strings and character or byte buffers
 * and its related functionalities
 *
 * This header file contains the definition of the @ref strbuf type and the functions which make up
 * the API to use it.
 *
 * **Implementation**
 *
 * The implementation can be included if the macro `CLZ_STRBUF_IMPL` is defined beforehand. Some functionalities
 * are implemented as macros for performance reasons.
 */

#ifndef _CLZ_STRBUF_H
/**
 * @brief Include guard for this file.
 */
#define _CLZ_STRBUF_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clz.h"
#include "def.h"

/**
 * @brief Definition of structure representing a character or byte buffer.
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
typedef struct strbuf {
    /**
     * @brief The size of the allocation buffer
     *
     * This value is initialized to the value @ref CLZ_STRBUF_ALLOC, and increased dynamically
     * as soon as the data size surpasses the allocation. Every time this happens, the previous value
     * for the buffer size is doubled.
     *
     * **Notes**
    *
    * This value contains the buffer size in units of `sizeof(char)`. This value can be checked safely
    * with the macro @ref strbuf_alloc_size.
     *
     * @see strbuf_alloc_size, CLZ_STRBUF_ALLOC
     */
    size_t alloc_size;
    /**
     * @brief The size of the character array
     *
     * This value is initialized to `0`, and increased dynamically
     * as the data grows, or decreased as the data shrinks.
     *
     * **Notes**
     *
     * This value contains the number of elements of the array. This value can be checked safely with the macro
     * @ref strbuf_length.
     *
     * @see strbuf_length
     */
    size_t data_size;
    /**
     * @brief The array
     *
     * This variable holds a dynamically allocated (read: `malloc` family) array of `void *`-pointers. You can use
     * the function @ref strbuf_get or the macro @ref strbuf_at.
     *
     * @see strbuf_get, strbuf_at
     */
    char *buf;
    /**
     * @brief Internal variable holding the last index of the @ref strbuf_find_next search function.
     *
     * **Notes**
     *
     * This is an internal variable that is not meant to be accessed in any way. Its value is initialised to @ref CLZ_FIND_INDEX_START.
     * It can and should be reset to that value with the macro @ref strbuf_find_reset after the @ref strbuf_find_next search is concluded.
     *
     * @see strbuf_find_next, strbuf_find_reset
     */
    int find_index;
} strbuf;

/**
 * @brief Initializes a new @ref strbuf struct.
 *
 * This function makes use of dynamic allocation (see `malloc` family). As such, it could _fail_
 * if heap allocation fails.
 *
 * **Notes**
 *
 * When finished using it, the function @ref strbuf_free must be invoked to free the internal allocations
 * made by this function.
 *
 * This function (if successful) returns the `buf` buffer (see @ref strbuf). It does **not** return a pointer
 * to the @ref strbuf, which is a big difference from the @ref strbuf_new function.
 *
 * @param d The pointer to the array struct
 * @return The pointer to the buffer array (`strbuf.buf`) if successful, `NULL` otherwise
 *
 * @see strbuf_new, @see strbuf_free
 */
void *strbuf_init(strbuf *d);
/**
 * @brief Allocates a new `strbuf` struct object on the heap and returns its pointer.
 *
 * Since the `malloc` family is used internally, heap allocation may fail. In this case, `NULL` is returned.
 *
 * **Notes**
 *
 * When finished using it, the function @ref strbuf_free must be invoked to free the internal allocations
 * made by this function.
 *
 * @return The pointer to the @ref strbuf if successful, `NULL` otherwise
 *
 * @see strbuf_init, @see strbuf_free
 */
strbuf *strbuf_new();
/**
 * @brief Frees the internal allocations of the strbuf
 *
 * When the strbuf is no longer used, this function should be invoked for cleanup.
 *
 * **Notes**
 *
 * If the @ref strbuf itself was allocated on the heap, it must be freed separately **after** @ref strbuf_free has been called,
 * as this function does not free it.
 * Example:
 *
 *     strbuf *d = strbuf_new();
 *     // ...
 *     strbuf_free(d);
 *     free(d);
 *
 * The reason why this behaviour is intended is that it is also possible to create a statically allocated @ref strbuf like this:
 *
 *     strbuf d;
 *     strbuf_init(&d);
 *     // ...
 *     strbuf_free(&d);
 *     free(&d) // WRONG! //
 *
 * See the examples for more information.
 *
 * @param d The @ref strbuf to free.
 */
void strbuf_free(strbuf *d);

/**
 * @brief Append a single character after the end of the buffer
 *
 * This function appends the given `char` value after the end of the array. If the allocated buffer is full, the
 * buffer is extended to account for the required memory. The initial buffer size is defined in @ref CLZ_STRBUF_ALLOC,
 * and the size is doubled every time the buffer needs to be extended.
 *
 * **Notes**
 *
 * If the buffer needs to be extended, this is done by dynamic heap allocation. The functions that are part of the `malloc`
 * family may fail and return `NULL`. If this is the case, `false` (from `stdbool.h`) is returned to indicate failure.
 * In case of success, the opposite value `true` is returned.
 *
 * @param sb The @ref strbuf
 * @param c The pointer that needs to be appended after the end
 * @return `false` in case of failure, `true` otherwise
 *
 * @see strbuf_alloc_size
 */
bool strbuf_append_char(strbuf *sb, char c);

/**
 * @brief Append a C string (aka. a null-terminated `char`array) after the end of the buffer
 *
 * This function appends the given `char *` value after the end of the array. If the allocated buffer is full or not
 * long enough, the buffer is extended to account for the required memory. The initial buffer size is defined in
 * @ref CLZ_STRBUF_ALLOC, and the size is doubled every time the buffer needs to be extended.
 *
 * **Notes**
 *
 * If the buffer needs to be extended, this is done by dynamic heap allocation. The functions that are part of the `malloc`
 * family may fail and return `NULL`. If this is the case, `false` (from `stdbool.h`) is returned to indicate failure.
 * In case of success, the opposite value `true` is returned.
 *
 * @param sb The @ref strbuf
 * @param s The pointer to the string that needs to be appended after the end
 * @return `false` in case of failure, `true` otherwise
 *
 * @see strbuf_alloc_size
 */
bool strbuf_append_str(strbuf *sb, char *s);

/**
 * @brief Append a `strbuf` (`sb2`) after the end of another `strbuf` (`sb1`)
 *
 * This function appends the second `strbuf` after the end of the first. If the allocated buffer of the first is full or not
 * long enough, the buffer is extended to account for the required memory. The initial buffer size is defined in
 * @ref CLZ_STRBUF_ALLOC, and the size is doubled every time the buffer needs to be extended.
 *
 * **Notes**
 *
 * If the buffer needs to be extended, this is done by dynamic heap allocation. The functions that are part of the `malloc`
 * family may fail and return `NULL`. If this is the case, `false` (from `stdbool.h`) is returned to indicate failure.
 * In case of success, the opposite value `true` is returned.
 *
 * @param sb1 The destination @ref strbuf
 * @param sb2 The source @ref strbuf
 * @return `false` in case of failure, `true` otherwise
 *
 * @see strbuf_alloc_size
 */
bool strbuf_append_strbuf(strbuf *sb1, strbuf *sb2);

/**
 * @brief Append the first `n` characters of a C string (aka. a null-terminated `char`array) after the end of the buffer
 *
 * This function appends the first `n` characters of the given `char *` value after the end of the array.
 * If the allocated buffer is full or not long enough, the buffer is extended to account for the required memory.
 * The initial buffer size is defined in @ref CLZ_STRBUF_ALLOC, and the size is doubled every time the buffer needs to
 * be extended.
 *
 * If `n` is smaller than `strlen(s)`, the entire string `s` will be appended.
 *
 * **Notes**
 *
 * If the buffer needs to be extended, this is done by dynamic heap allocation. The functions that are part of the `malloc`
 * family may fail and return `NULL`. If this is the case, `false` (from `stdbool.h`) is returned to indicate failure.
 * In case of success, the opposite value `true` is returned.
 *
 * @param sb The @ref strbuf
 * @param s The pointer to the string that needs to be appended after the end
 * @param n The max amount of characters to append
 * @return `false` in case of failure, `true` otherwise
 *
 * @see strbuf_alloc_size
 */
bool strbuf_append_str_n(strbuf *sb, char *s, size_t n);

/**
 * @brief Append the first `n` characters of a `strbuf` (`sb2`) after the end of another `strbuf` (`sb1`)
 *
 * This function appends the first `n` characters of the second `strbuf` after the end of the first. If the allocated buffer of the first is full or not
 * long enough, the buffer is extended to account for the required memory. The initial buffer size is defined in
 * @ref CLZ_STRBUF_ALLOC, and the size is doubled every time the buffer needs to be extended.
 *
 * If `n` is smaller than `strlen(s)`, the entire string `s` will be appended.
 *
 * **Notes**
 *
 * If the buffer needs to be extended, this is done by dynamic heap allocation. The functions that are part of the `malloc`
 * family may fail and return `NULL`. If this is the case, `false` (from `stdbool.h`) is returned to indicate failure.
 * In case of success, the opposite value `true` is returned.
 *
 * @param sb1 The destination @ref strbuf
 * @param sb2 The source @ref strbuf
 * @param n The max amount of characters to append
 * @return `false` in case of failure, `true` otherwise
 *
 * @see strbuf_alloc_size
 */
bool strbuf_append_strbuf_n(strbuf *sb1, strbuf *sb2, size_t n);

/**
 * @brief Set `char` by index
 *
 * This function overwrites an element of the array. `index` is required to be less than the length of the char, otherwise
 * the operation is attempted out of bounds. In this case, the behaviour is undefined. If `index` is equal
 * to the buffer length, the null-terminator is overwritten and removed, which will most likely result in undefined
 * behaviour later.
 *
 * **Notes**
 *
 * `index` must be the index of a `char` already in the array, otherwise the function will not perform the
 * operation.
 *
 * @param sb The @ref strbuf
 * @param index The `char` index in the @ref strbuf
 * @param c The pointer that needs to be appended after the end
 *
 * @see strbuf_append_char, strbuf_get, strbuf_length
 */
void strbuf_set(strbuf *sb, size_t index, char c);

/**
 * @brief Get `char` by index
 *
 * This function retrieves an element from the array. `index` is required to be less than the length of the buffer, otherwise
 * the operation is attempted out of bounds. In this case, `0` is returned.
 * returned.
 *
 * **Notes**
 *
 * `index` must be the index of an element already in the array, otherwise the function will not perform the
 * operation. Since `0` is technically a valid char array entry (the last one), checking whether an index is valid by comparing this function's
 * return value and `0` does not always work. See the macro @ref strbuf_valid_index for that.
 *
 * We have also provided the macro @ref strbuf_at that performs the same operation and is slightly more performant, however this function is type-safe
 * and performs index bounds checking, whereas the macro's usage results in undefined behaviour if the index is invalid.
 *
 * @param sb The @ref strbuf
 * @param index The `char` index in the array
 * @return `0` in case of failure, the requested `char` otherwise
 *
 * @see strbuf_append_char, strbuf_at, strbuf_length
 */
void *strbuf_get(strbuf *sb, size_t index);

/**
 * @brief Removes the first occurrence of the `char` within the @ref strbuf
 *
 * If the element is not found within the array, `false` is returned.
 *
 * **Notes**
 *
 * This function also reallocates the element array using `realloc`. If `realloc` fails, the operation
 * is aborted and `false` is returned. The buffer is not shrunk.
 *
 * @param sb The @ref strbuf
 * @param c The `char` to remove
 * @return `true` if the `char` could be deleted, `false` otherwise
 *
 * @see strbuf_remove_all, @ref strbuf_remove_index
 */
bool strbuf_remove_first(strbuf *sb, char c);

/**
 * @brief Removes all occurrences of the `char` within the @ref strbuf
 *
 * If the element is not found anywhere within the array, `false` is returned.
 *
 * **Notes**
 *
 * This function also reallocates the element array using `realloc`. If `realloc` fails, the operation
 * is aborted and `false` is returned. The buffer is not shrunk.
 *
 * @param sb The @ref strbuf
 * @param c The entry to remove
 * @return `true` if the entry could be deleted, `false` otherwise
 *
 * @see strbuf_remove_first, @ref strbuf_remove_index
 */
bool strbuf_remove_all(strbuf *sb, char c);
/**
 * @brief Removes the `char` at the index
 *
 * If the index is out of bounds, `false` is returned.
 *
 * **Notes**
 *
 * This function also reallocates the element array using `realloc`. If `realloc` fails, the operation
 * is aborted and `false` is returned. The buffer is not shrunk.
 *
 * @param sb The @ref strbuf
 * @param index The index of the `char` to remove
 * @return `true` if the entry could be deleted, `false` otherwise
 *
 * @see strbuf_remove_first, @ref strbuf_remove_all, @ref strbuf_pop
 */
bool strbuf_remove_index(strbuf *sb, size_t index);

/**
 * @brief Clears the buffer contents
 *
 * **Notes**
 *
 * This function does **not** reset buffer size to its default value. If you wish to wipe the data and create a
 * new array with initial buffer size @ref CLZ_STRBUF_ALLOC, you probably need to deallocate the entire array
 * with @ref strbuf_free and create a new one.
 *
 * @param sb The strbuf
 *
 * @ref strbuf_free, strbuf_remove_all
 */
void strbuf_clear(strbuf *sb);

/**
 * @brief Return position of first occurrence of the `char` in the array.
 *
 * This function returns the index of the first occurrence of `c` in `sb`. If `c` is not found anywhere,
 * @ref CLZ_NOT_FOUND is returned.
 *
 * @param sb The @ref strbuf
 * @param c The `char` to look for
 * @return The position of the `char`
 *
 * @see strbuf_find_next, strbuf_find_reset
 */
int strbuf_find_first(strbuf *sb, char c);

/**
 * @brief Return position of next occurrence of the `char` in the array.
 *
 * This function returns the index of the next occurrence of `c` in `sb`. If `c` is not found anywhere,
 * @ref CLZ_NOT_FOUND is returned.
 *
 * The idea is that you can continue your search from the position where the last item was found.
 *
 * Example:
 *
 *     strbuf *sb = strbuf_new();
 *     // ...
 *     int pos;
 *     while ((pos = strbuf_find_next(sb, c)) != CLZ_NOT_FOUND) {
 *         printf("Found c at %d\n", pos);
 *     }
 *     strbuf_find_reset(c);
 *     // ...
 *     strbuf_free(sb);
 *
 * The macro @ref strbuf_find_reset resets the search so that the strbuf is ready by the next time.
 * If this macro is not invoked, the search will resume from the last index where a match was found. If the last search
 * yielded @ref CLZ_NOT_FOUND, the search is not reset automatically and any further searches will yield @ref CLZ_NOT_FOUND
 * until the search is reset.
 *
 * See the tutorial for more information.
 *
 * @param sb The @ref strbuf
 * @param c The `char`-entry to look for
 * @return The position of the `char`
 *
 * @see strbuf_find_first, strbuf_find_reset
 */
int strbuf_find_next(strbuf *sb, char c);

/**
 * @brief Iterate through the array and performs a predefined operation on each `char`-element
 *
 * This function performs a trivial iteration across all array elements and invokes the @ref clz_consumer function `c` which was
 * passed to the function on every entry pointer.
 *
 * @param sb The @ref strbuf
 * @param c The consumer which needs to be invoked on each element
 *
 * @see clz_consumer, strbuf_foreach_if, strbuf_foreach_if_else
 */
void dynarray_foreach(strbuf *sb, clz_consumer c);

/**
 * @brief Iterate through the array and performs a predefined operation on each pointer-element if a certain condition
 * applies
 *
 * This function performs a trivial iteration across all array elements and invokes the @ref clz_consumer function `c` which was
 * passed to the function on every entry pointer if and only if the @ref clz_predicate function `p` returns the `bool` (C99,
 * `stdbool.h`) value `true`.
 *
 * @param sb The @ref strbuf
 * @param p The predicate which represents the `if`-conditional
 * @param c The consumer which needs to be invoked on each element if the predicate evaluates to `true`
 *
 * @see clz_consumer, clz_predicate, strbuf_foreach, strbuf_foreach_if_else
 */
void dynarray_foreach_if(strbuf *sb, clz_predicate p, clz_consumer c);

/**
 * @brief Iterate through the array and performs an "if"-operation on each `char`-element if a certain condition
 * applies, and an "else"-operation otherwise
 *
 * This function performs a trivial iteration across all array elements and invokes the @ref clz_consumer function `ifc` which was
 * passed to the function on every entry pointer if and only if the @ref clz_predicate function `p` returns the `bool` (C99,
 * `stdbool.h`) value `true`. If the predicate fails, the `elsec` consumer function is invoked on the entry-pointer.
 *
 * @param sb The @ref dynarray
 * @param p The predicate which represents the `if`-conditional
 * @param ifc The consumer which needs to be invoked on each element if the predicate evaluates to `true`
 * @param elsec The consumer which needs to be invoked on each element if the predicate evaluates to `false`
 *
 * @see clz_consumer, clz_predicate, strbuf_foreach, strbuf_foreach_if
 */
void dynarray_foreach_if_else(strbuf *sb, clz_predicate p, clz_consumer ifc, clz_consumer elsec);

/**
 * @brief Removes the last element `char` within the array and returns it
 *
 * If the array is empty, `0` is returned.
 *
 * **Notes**
 *
 * The object is not deallocated and the buffer is not shrunk.
 *
 * @param sb The @ref strbuf
 * @return The entry that was removed or `0` in case of failure
 *
 * @see strbuf_get, strbuf_at, strbuf_remove_first, @ref strbuf_remove_index
 */
void *strbuf_pop(strbuf *sb);

#define CLZ_STRBUF_ALLOC 32

#define strbuf_length(sb) ((sb)->data_size)
#define strbuf_alloc_size(sb) ((sb)->alloc_size)
#define strbuf_find_reset(sb) ((sb)->find_index = CLZ_FIND_INDEX_START)
#define strbuf_at(sb, index) (*((sb)->buf + (index)))
#define strbuf_valid_index(sb, index) (strbuf_length(sb) > (index))

#endif

#ifdef CLZ_STRBUF_IMPL

void *strbuf_init(strbuf *sb) {
    sb->alloc_size = CLZ_STRBUF_ALLOC_ALLOC;
    sb->data_size = 0;
    sb->buf = (char *) calloc(CLZ_STRBUF_ALLOC, sizeof(char));
    if (sb->ptr == NULL) return NULL;
    d->find_index = CLZ_FIND_INDEX_START;
    return (void *) sb->buf;
}

strbuf *strbuf_new() {
    strbuf *sb = malloc(sizeof(strbuf));
    if (sb == NULL) return NULL;
    if(strbuf_init(sb)) {
        return sb;
    }
    free(sb);
    return NULL;
}

void strbuf_free(strbuf *sb) {
    free(sb->buf);
}

bool strbuf_append_char(strbuf *sb, char c) {
    if (sb->alloc_size == sb->data_size + 1) {
        char *new_buf = (char *) reallocarray(sb->buf, sb->alloc_size * 2, sizeof(char));
        if (new_buf == NULL) return false;
        sb->alloc_size *= 2;
        sb->buf = new_ptr;
    }

    *(sb->buf + sb->data_size) = c;
    *(sb->buf + sb->data_size + 1) = '\0';
    sb->data_size++;
    return c;
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
