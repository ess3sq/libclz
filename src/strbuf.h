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
 *
 * A `strbuf` is nothing but a heap-allocated buffer intended to hold null-terminated `char` (byte) arrays.
 * Typically this buffer will be larger than the string itself in order to account for the space required
 * by potential appending operations. If an operation requires more space than the buffer currently has
 * to offer, the entire buffer will be `realloc`'d. To this end, most functions declared in this header
 * file will take a double pointer `char **destbuf`, which represents a pointer to the destination buffer.
 * What this implies is that when the buffer is `realloc`'d, the original variable holding the pointer is
 * overwritten and now points to the `realloc`'d memory instead of the original chunk.
 *
 * For programs that use large enough amounts of memory, (re-) allocations may fail. If this happens,
 * typically a certain value is returned. Nevertheless, this value is not always unique for failure.
 * Example: a string replacing routine that returns the amount of replacement instances performed.
 * If nothing was replaced, `(size_t) 0` is returned, which could also happen if the necessary (re-) allocations
 * failed. Hence in critical applications and programs are very much incouraged to check `errno` for
 * `ENOMEM`.
 *
 * Another important point is that the buffer stores and manages its size automatically without the
 * programmer's attention. As such, calls to `free(...)` will fail and crash the program! See @ref strbuf_free
 * instead. Additionally, since the buffers are often larger than the string itself, chances are they
 * are more forgiving towards buffer overflow, provided the overflow is not dire enough that it goes beyond
 * the buffer size. While the full buffer size can be checked with @ref strbuf_alloc_size, it is still
 * something that has to be avoided.
 *
 * Because of the allcoation and re-allocation being done behind the scene, **any parameter called** `strbuf`,
 * `destbuf` or anything containing the three letters ` buf` **must** be a proper buffer allocated by @ref strbuf_new
 * and/or @ref strbuf_new_size. This rule should be enforced with an iron fist, because strbufs are prefixed
 * with information about the allocation size that regular C-strings do not have. What this implies is that
 * passing such C-strings off as buffers will result in undefined behavior the moment the buffer size is
 * supposed to be retrieved or the buffer is meant to be `realloc`'d.
 *
 * **Implementation**
 *
 * The implementation can be included if the macro `CLZ_STRBUF_IMPL` is defined beforehand. Some functionalities
 * are implemented as macros for performance reasons.
 *
 * @file strbuf.h
 * @author Lorenzo Calza
 * @date 8-13 May 2021
 * @brief Header file containing the declarations for string operations and character or byte buffers
 * and its related functionalities
 *
 */

#ifndef _CLZ_STRBUF_H
/**
 * @brief Include guard for this file.
 */
#define _CLZ_STRBUF_H

#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#include "clz.h"

/**
 * @brief Allocates a new `strbuf` with default size
 *
 * This function constructs a new buffer with the default size (see @ref CLZ_STRBUF_ALLOC).
 * Since dynamic allocation is used, it will need to be freed after use through @ref strbuf_free.
 * It is also possible to use another initial size, see the general case @ref strbuf_new_size. As such,
 * this function simply calls
 *
 * @code
 * strbuf_new_size(CLZ_STRBUF_ALLOC);
 * @endcode
 *
 *
 * Should the dynamic allocation fail, then `NULL` is returned.
 *
 * **Notes**
 *
 * Do **not** attempt to call `free(...)` on a pointer returned by this function or @ref strbuf_new_size!
 *
 * @return The allocated buffer
 * @see strbuf_new_size, strbuf_free
 */
char *strbuf_new();

/**
 * @brief Allocates a new `strbuf` with the specified minimum size
 *
 * This function constructs a new buffer with the minimum size (see @ref CLZ_STRBUF_ALLOC).
 * The actual size will be the next power of 2.
 * Since dynamic allocation is used, it will need to be freed after use through @ref strbuf_free.
 * It is possible to use the default size by simply calling,
 *
 * @code
 * strbuf_new_size(CLZ_STRBUF_ALLOC);
 * @endcode
 *
 * or even easier @ref strbuf_new.
 *
 * Should the dynamic allocation fail, then `NULL` is returned.
 *
 * **Notes**
 *
 * Do **not** attempt to call `free(...)` on a pointer returned by this function or @ref strbuf_new!
 *
 * @param sz The minimum size
 * @return The allocated buffer
 * @see strbuf_new, strbuf_free
 */
char *strbuf_new_size(size_t sz);
/**
 * @brief Creates a string buffer with the contents of the given C-string.
 *
 * This function allocates a proper buffer size (power of 2) and copies over the bytes from `s`.
 * The new string buffer is a fully fledged buffer that can be operated on using all buffer functions
 * from @ref strbuf.h, and has to be freed using `strbuf_free` after usage.
 *
 * Since dynamic allocation is used, failure is possible. In this case, `NULL` is returned and `errno`
 * is meant to be set to `ENOMEM`.
 *
 * If you wish to create a string buffer from another proper buffer, see @ref strbuf_clone.
 *
 * @param s The string to copy over
 * @return the new buffer
 *
 * @see strbuf_clone, strbuf_free
 */
char *strbuf_new_str(char *s);
/**
 * @brief Frees previously allocated `strbuf`.
 *
 * This function frees a buffer previously allocated with @ref strbuf_new or @ref strbuf_new_size.
 * Attempts to free regular pointers with this function will fail and crash the program (standard behavior
 * for attempts call `free(...)` on invalid pointers.
 *
 * Should the dynamic allocation fail, then `NULL` is returned.
 *
 * @return the allocated buffer
 * @see strbuf_new_size, strbuf_free
 */
void strbuf_free(char *strbuf);
/**
 * @brief Clones a buffer
 *
 * This function is essentially the version of `strdup` but for for string buffers managed by the functions
 * defined in this header. While it is possible to use `strdup` on a strbuf, the result will just be the
 * string contents, not an actual buffer. As such, any buffer operations are forbidden from acting on such
 * a pointer, and passing it regardless of this will result in undefined behavior.
 *
 * It should be noted that this function takes a `char *strbuf` instead of a `char **strbuf`. This is not
 * a mistake, but still one of the (few) exceptions and the programmer should be mindful not to pass a
 * double pointer by mistake. Nevertheless, `strbuf` **must** be a proper buffer and not just a regular C-string.
 *
 * If the parameter `bufsz` is `true`, the new buffer will have the same allocation size as the old one.
 * If not, the new buffer will have a size just large enough to hold the contents up to the null-terminator,
 * while still being a power of 2.
 *
 * If you wish to create a string buffer from a regular C-string, see @ref strbuf_new_str.
 *
 * Since heap allocation takes place, failure is possible. If this is the case, `NULL` is returned
 * and `errno` is set to `ENOMEM` by `malloc`.
 *
 * **Notes**
 *
 * Similar to the case of buffers returned by @ref strbuf_new and @ref strbuf_new_size, you should always
 * free the memory using @ref strbuf_free.
 * Do **not** attempt to call `free(...)` on a pointer returned by this function!
 *
 * @param strbuf The buffer that has to be cloned.
 * @param bufsz Whether or not to use the same buffer size as the original buffer
 * @return The new strbuf
 *
 * @see strbuf_new, strbuf_free, strbuf_alloc_size, strbuf_new_str
 */
char *strbuf_clone(char *strbuf, bool bufsz);

/**
 * @brief Returns buffer size
 *
 * This function retrieves and returns the allocation size, not to be confused with the string length.
 * Do **not** pass a regular C-string to this function!
 *
 * It should be noted that this function takes a `char *strbuf` instead of a `char **strbuf`. This is not
 * a mistake, but still one of the (few) exceptions and the programmer should be mindful not to pass a
 * double pointer by mistake. Nevertheless, `strbuf` **must** be a proper buffer and not just a regular C-string.
 *
 * @param strbuf The buffer
 * @return The buffer size
 */
size_t strbuf_alloc_size(char *strbuf);
/**
 * @brief Resizes buffer using `realloc`
 *
 * This function determines the next power of 2 greater than or equal to `minsize + 1`, then calls `realloc`
 * on the input buffer and lastly overwrites the original pointer to point to the new location.
 * To the programmer, the buffer has been magically stretched because they do not have to reassign anything.
 *
 * If the specified parameter `minsize` is too small to hold the buffer contents including the null-terminator,
 * the buffer is **not** resized and `false` is returned. If what you are attempting to do is trim the string
 * within the buffer, use `strbuf_trim_length` or another function from the `strbuf_trim_...` family.
 * Alternatively, if you are attempting to reduce the buffer size as much as possible, see
 * @ref strbuf_compress.
 *
 * **Notes**
 *
 * Since use of `realloc` is made, it is possible that the dynamic allocation fails. In this case, the buffer
 * is not resized and `false` is returned.
 *
 * @param dest The buffer that needs to be resized
 * @param minsize The minimum size for the new buffer
 * @return true if successful
 *
 * @see strbuf_compress, strbuf_alloc_size, strbuf_trim_length
 */
bool strbuf_resize(char **dest, size_t minsize);
/**
 * @brief Resizes buffer to its minimum size
 *
 * This function determines the next power of 2 greater than or equal to `strlen(*dest) + 1`, then calls `
 * @ref strbuf_resize.
 * To the programmer, the buffer has been magically stretched because they do not have to reassign anything.
 *
 * **Notes**
 *
 * Since use of `realloc` is made within @ref strbuf_resize, it is possible that the dynamic allocation fails.
 * In this case, the buffer is not compressed (resized) and `false` is returned.
 *
 * @param dest The buffer that needs to be resized
 * @return true if successful
 *
 * @see strbuf_resize, strbuf_alloc_size, strbuf_trim_length
 */
bool strbuf_compress(char **dest);

/**
 * @brief Appends a `char` to a strbuf
 *
 * This function appends the specified `char` after the end of the given buffer.
 * If the buffer is full, it will be `realloc`'d and the `char` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param c The `char` to append
 * @return `true` if successful
 *
 * @see strbuf_append_str
 */
bool strbuf_append_char(char **destbuf, char c);
/**
 * @brief Appends a string to a strbuf
 *
 * This function appends the specified `char *` C-string after the end of the given buffer.
 * If the buffer is too small to store the full string, it will be `realloc`'d and the `char`
 * will be appended to the new buffer. In case the heap allocation fails, `false` is returned,
 * otherwise `true`.
 *
 * To be clear, `src` need not be a buffer, it can be a regular string or an actual buffer.
 *
 * @param destbuf The destination buffer
 * @param src The string to append
 * @return `true` if successful
 *
 * @see strbuf_append_char, strbuf_append_strn
 */
bool strbuf_append_str(char **destbuf, char *src);
/**
 * @brief Appends at most the first `n` characters of a string to a strbuf
 *
 * This function appends the first `n` characters of the specified `char *` C-string after the
 * end of the given buffer. If `n` is larger than or equal to the length of `src`, `src` is appended.
 *
 * If the buffer is too small to store the full string, it will be `realloc`'d and the `char`
 * will be appended to the new buffer. In case the heap allocation fails, `false` is returned,
 * otherwise `true`.
 *
 * To be clear, `src` need not be a buffer, it can be a regular string or an actual buffer.
 *
 * @param destbuf The destination buffer
 * @param src The string to append
 * @param n The max amount of chars to append
 * @return `true` if successful
 *
 * @see strbuf_append_char, strbuf_append_str
 */
bool strbuf_append_strn(char **destbuf, char *src, size_t n);
/**
 * @brief Appends an `int` to a strbuf in decimal notation
 *
 * This function appends the specified `int` after the end of the given buffer by "stringifying" it and
 * appending it using @ref strbuf_append_str.
 * If the buffer is full, it will be `realloc`'d and the `int` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param i The integer to append
 * @return `true` if successful
 *
 * @see strbuf_append_char, strbuf_append_uint, strbuf_append_long
 */
bool strbuf_append_int(char **destbuf, int i);
/**
 * @brief Appends an `unsigned int` to a strbuf in decimal notation
 *
 * This function appends the specified `unsigned int` after the end of the given buffer by "stringifying" it and
 * appending it using @ref strbuf_append_str.
 * If the buffer is full, it will be `realloc`'d and the `unsigned int` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param i The integer to append
 * @return `true` if successful
 *
 * @see strbuf_append_char, strbuf_append_int, strbuf_append_long
 */
bool strbuf_append_uint(char **destbuf, unsigned int i);
/**
 * @brief Appends a `long` to a strbuf in decimal notation
 *
 * This function appends the specified `long` after the end of the given buffer by "stringifying" it and
 * appending it using @ref strbuf_append_str.
 * If the buffer is full, it will be `realloc`'d and the `long` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param l The long to append
 * @return `true` if successful
 *
 * @see strbuf_append_int, strbuf_append_ulong, strbuf_append_llong, strbuf_append_ullong
 */
bool strbuf_append_long(char **destbuf, long l);
/**
 * @brief Appends an `unsigned long` to a strbuf in decimal notation
 *
 * This function appends the specified `unsigned long` after the end of the given buffer by
 * "stringifying" it and appending it using @ref strbuf_append_str.
 * If the buffer is full, it will be `realloc`'d and the `unsigned long` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param l The long to append
 * @return `true` if successful
 *
 * @see strbuf_append_int, strbuf_append_long, strbuf_append_llong, strbuf_append_ullong
 */
bool strbuf_append_ulong(char **destbuf, unsigned long l);
/**
 * @brief Appends a `long long` to a strbuf in decimal notation
 *
 * This function appends the specified `long long` after the end of the given buffer by "stringifying" it and
 * appending it using @ref strbuf_append_str.
 * If the buffer is full, it will be `realloc`'d and the `long long` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param l The long to append
 * @return `true` if successful
 *
 * @see strbuf_append_int, strbuf_append_long, strbuf_append_ullong
 */
bool strbuf_append_llong(char **destbuf, long  long l);
/**
 * @brief Appends a `unsigned long long` to a strbuf in decimal notation
 *
 * This function appends the specified `unsigned long long` after the end of the given buffer by "stringifying" it and
 * appending it using @ref strbuf_append_str.
 * If the buffer is full, it will be `realloc`'d and the `unsigned long long` will be appended to
 * the new buffer. In case the heap allocation fails, `false` is returned, otherwise `true`.
 *
 * @param destbuf The destination buffer
 * @param l The long to append
 * @return `true` if successful
 *
 * @see strbuf_append_int, strbuf_append_ulong, strbuf_append_llong
 */
bool strbuf_append_ullong(char **destbuf, unsigned long long l);

/**
 * @brief Inserts a `char` at the given position.
 *
 * This function inserts the specified `char` at the given `index`, shifting the trailing
 * substring by 1 byte. If the buffer needs to be extended, this is done automatically. However,
 * should the internal heap allocation fail, `false` will be returned and the `char` will not be appended,
 * effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param c The `char` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_str
 */
bool strbuf_insert_char(char **destbuf, char c, size_t index);
/**
 * @brief Inserts a string at the given position.
 *
 * This function inserts the specified `char *` at the given `index`, shifting the trailing
 * substring by `strlen(s)` many bytes. If the buffer needs to be extended, this is done automatically. However,
 * should the internal heap allocation fail, `false` will be returned and the `char *` will not be appended,
 * effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * To be clear, `s` need not be a buffer. It can be, but it can also be a regular null-terminated C-string.
 *
 * Furthermore, calling this function is equivalent to
 * @code
 *     strbuf_insert_strn(destbuf, s, index, strlen(*destbuf));
 * @endcode
 *
 * @param destbuf The destination buffer
 * @param s The `char *` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_char, strbuf_insert_strn
 */
bool strbuf_insert_str(char **destbuf, char *s, size_t index);
/**
 * @brief Inserts at most the first `maxlen` characters of a string at the given position.
 *
 * This function inserts the at most the first `maxlen` characters of the specified `char *` at the given
 * `index`, shifting the trailing substring by `maxlen` many bytes if n is smaller than `strlen(s)`,
 * `strlen(s)` otherwise. If the buffer needs to be extended, this is done automatically. However,
 * should the internal heap allocation fail, `false` will be returned and the `char *` will not be appended,
 * effectively leaving the buffer and its contents unchanged.
 *
 * If `maxlen` is greater than or equal to `strlen(s)`, the full string is inserted like in @ref strbuf_insert_str.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * To be clear, `s` need not be a buffer. It can be, but it can also be a regular null-terminated C-string.
 *
 * Furthermore, calling this function is equivalent to
 * @code
 *     strbuf_insert_strn(destbuf, s, index, strlen(*destbuf));
 * @endcode
 *
 * @param destbuf The destination buffer
 * @param s The `char *` to append
 * @param index  The position of the insertion
 * @param maxlen The max amount of characters to insert
 * @return `true` if successful
 *
 * @see strbuf_insert_char, strbuf_insert_str
 */
bool strbuf_insert_strn(char **destbuf, char *s, size_t index, size_t maxlen);
/**
 * @brief Inserts an `int` at the given position.
 *
 * This function inserts the specified `int` at the given `index` after "stringifying" it (decimal notation!),
 * shifting the trailing substring by the necessary amount of bytes. If the buffer needs to be extended,
 * this is done automatically. However, should the internal heap allocation fail, `false` will be returned
 * and the `int` will not be appended, effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param i The `int` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_uint, strbuf_insert_long
 */
bool strbuf_insert_int(char **destbuf, int i, size_t index);
/**
 * @brief Inserts an `unsigned int` at the given position.
 *
 * This function inserts the specified `unsigned int` at the given `index` after "stringifying" it (decimal notation!),
 * shifting the trailing substring by the necessary amount of bytes. If the buffer needs to be extended,
 * this is done automatically. However, should the internal heap allocation fail, `false` will be returned
 * and the `int` will not be appended, effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param i The `unsigned int` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_int, strbuf_insert_ulong
 */
bool strbuf_insert_uint(char **destbuf, unsigned int i, size_t index);
/**
 * @brief Inserts an `long` at the given position.
 *
 * This function inserts the specified `long` at the given `index` after "stringifying" it (decimal notation!),
 * shifting the trailing substring by the necessary amount of bytes. If the buffer needs to be extended,
 * this is done automatically. However, should the internal heap allocation fail, `false` will be returned
 * and the `int` will not be appended, effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param l The `long` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_int, strbuf_insert_ulong
 */
bool strbuf_insert_long(char **destbuf, long l, size_t index);
/**
 * @brief Inserts an `unsigned long` at the given position.
 *
 * This function inserts the specified `unsigned long` at the given `index` after "stringifying" it (decimal notation!),
 * shifting the trailing substring by the necessary amount of bytes. If the buffer needs to be extended,
 * this is done automatically. However, should the internal heap allocation fail, `false` will be returned
 * and the `int` will not be appended, effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param l The `unsigned long` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_long, strbuf_insert_ullong
 */
bool strbuf_insert_ulong(char **destbuf, unsigned long l, size_t index);
/**
 * @brief Inserts an `long long` at the given position.
 *
 * This function inserts the specified `long long` at the given `index` after "stringifying" it (decimal notation!),
 * shifting the trailing substring by the necessary amount of bytes. If the buffer needs to be extended,
 * this is done automatically. However, should the internal heap allocation fail, `false` will be returned
 * and the `int` will not be appended, effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param l The `long long` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_long, strbuf_insert_ullong
 */
bool strbuf_insert_llong(char **destbuf, long long l, size_t index);
/**
 * @brief Inserts an `unsigned long long` at the given position.
 *
 * This function inserts the specified `unsigned long long` at the given `index` after "stringifying" it (decimal notation!),
 * shifting the trailing substring by the necessary amount of bytes. If the buffer needs to be extended,
 * this is done automatically. However, should the internal heap allocation fail, `false` will be returned
 * and the `int` will not be appended, effectively leaving the buffer and its contents unchanged.
 *
 * If `index` is greater than `strlen(*destbuf)`, nothing will happen and `false` will be returned.
 * If `index` is exactly `strlen(*destbuf)`, the insertion will happen in the place of the null-terminator
 * and the null-terminator itself will be shifted.
 * Since this means that `false` can be returned due to reasons other than memory allocation failures,
 * the programmer might want to check `errno` for `ENOMEM` in case of `false` return value.
 *
 * @param destbuf The destination buffer
 * @param l The `unsigned long long` to append
 * @param index  The position of the insertion
 * @return `true` if successful
 *
 * @see strbuf_insert_ulong, strbuf_insert_llong
 */
bool strbuf_insert_ullong(char **destbuf, unsigned long long l, size_t index);

/**
 * @brief Trims head and tail of the string within the buffer
 *
 * This function cuts off the head and tail, keeping only the characters starting and including with
 * `(*destbuf)[start]`, ending and excluding `(*destbuf)[end]`. If `end` is past the end of the string,
 * the function behaves as if it were the end of the string (ie. no back trimming). Similarly, if `start = 0`,
 * no head trimming takes place. If `start` is greater than or equal to `end`, everything is trimmed as in
 * `**destbuf = 0`.
 *
 * In case the buffer size is reduced drastically, the buffer is still not shortened. Call @ref strbuf_compress
 * after trimming for that.
 *
 * **Notes**
 *
 * This function is essentially the opposite of @ref strbuf_remove_str, which deletes the bytes between the
 * two given indices and keeps the rest, pasting its ends together.
 *
 * @param destbuf The destination buffer
 * @param start The start of the substring
 * @param end The end of the substring
 *
 * @see strbuf_trim_length, strbuf_compress, strbuf_remove_str
 */
void strbuf_trim_index(char **destbuf, size_t start, size_t end);
/**
 * @brief Trims the contents past a certain length.
 *
 * This function cuts off everything past and including the position given by the index `length`. If `length`
 * is greater than or equal to `strlen(*destbuf)`, nothing happens.
 *
 * In case the buffer size is reduced drastically, the buffer is still not shortened. Call @ref strbuf_compress
 * after trimming for that.
 *
 * Calling this function is equivalent to using
 * @code
 *     strbuf_trim_index(destbuf, 0, length);
 * @endcode
 *
 * @param destbuf The destination buffer
 * @param length The length to trim to
 *
 * @see strbuf_trim_index, strbuf_compress
 */
void strbuf_trim_length(char **destbuf, size_t length);
/**
 * @brief Trim leading spaces.
 *
 * This function trims all the leading spaces (' ').
 *
 * In case the buffer size is reduced drastically, the buffer is still not shortened. Call @ref strbuf_compress
 * after trimming for that.
 *
 * Calling this function is equivalent to using
 * @code
 *     strbuf_trim_head_char(destbuf, ' ');
 * @endcode
 *
 * @param destbuf The destination buffer
 *
 * @see strbuf_trim_head_char, strbuf_trim_tail, strbuf_compress
 */
void strbuf_trim_head(char **destbuf);
/**
 * @brief Trim leading instances of a given `char`.
 *
 * This function trims all the leading instances of a given `char`.
 *
 * In case the buffer size is reduced drastically, the buffer is still not shortened. Call @ref strbuf_compress
 * after trimming for that.
 *
 * @param destbuf The destination buffer
 * @param c The leading `char` to clear out
 *
 * @see strbuf_trim_head, strbuf_trim_tail_char, strbuf_compress
 */
void strbuf_trim_head_char(char **destbuf, char c);
/**
 * @brief Trim trailing spaces.
 *
 * This function trims all the trailing spaces (' ').
 *
 * In case the buffer size is reduced drastically, the buffer is still not shortened. Call @ref strbuf_compress
 * after trimming for that.
 *
 * Calling this function is equivalent to using
 * @code
 *     strbuf_trim_tail_char(destbuf, ' ');
 * @endcode
 *
 * @param destbuf The destination buffer
 *
 * @see strbuf_trim_tail_char, strbuf_trim_head, strbuf_compress
 */
void strbuf_trim_tail(char **destbuf);
/**
 * @brief Trim trailing instances of a given `char`.
 *
 * This function trims all the trailing instances of a given `char`.
 *
 * In case the buffer size is reduced drastically, the buffer is still not shortened. Call @ref strbuf_compress
 * after trimming for that.
 *
 * @param destbuf The destination buffer
 * @param c The trailing `char` to clear out
 *
 * @see strbuf_trim_tail, strbuf_trim_head_char, strbuf_compress
 */
void strbuf_trim_tail_char(char **destbuf, char c);

/**
 * @brief Pads the head of the string with a given `char`.
 *
 * This function prepends `c` as many times as necessary until the string within the buffer is
 * the size `sz`. If `strlen(*destbuf)` is already greater than `sz`, nothing will happen
 * and `false` is returned. If it is exactly `sz`, nothing happens but `true` is returned.
 *
 * Since heap allocation is used, this `false` can also indicate failure in that regard. Check
 * `errno` for `ENOMEM` to account for that possibility.
 *
 * @param destbuf The destination buffer
 * @param c The character to pad `destbuf` with
 * @param sz The final size
 * @return `true` if successful
 *
 * @see strbuf_padding_tail
 */
bool strbuf_padding_head(char **destbuf, char c, size_t sz);
/**
 * @brief Pads the tail of the string with a given `char`.
 *
 * This function appends `c` as many times as necessary until the string within the buffer is
 * the size `sz`. If `strlen(*destbuf)` is already greater than `sz`, nothing will happen
 * and `false` is returned. If it is exactly `sz`, nothing happens but `true` is returned.
 * This is more efficient than calling `strbuf_append_char(destbuf, c)` in `sz - strlen(*destbuf)`
 * iterations provided that `sz` is not *too* small.
 *
 * Since heap allocation is used, this `false` can also indicate failure in that regard. Check
 * `errno` for `ENOMEM` to account for that possibility.
 *
 * @param destbuf The destination buffer
 * @param c The character to pad `destbuf` with
 * @param sz The final size
 * @return `true` if successful
 *
 * @see strbuf_padding_head
 */
bool strbuf_padding_tail(char **destbuf, char c, size_t sz);

/**
 * @brief Finds the first instance of the given `char`.
 *
 * This function returns the index of the first position at which `c` is found starting from the
 * beginning of the string buffer.
 * If `c` is not found anywhere within the string, the macro constant @ref CLZ_NOT_FOUND defined
 * within @ref clz.h is returned.
 *
 * @param destbuf The destination buffer (haystack)
 * @param c The needle
 * @return the position if found
 *
 * @see strbuf_find_last_char
 */
int strbuf_find_first_char(char **destbuf, char c);

/**
 * @brief Finds the last instance of the given `char`.
 *
 * This function returns the index of the first position at which `c` is found starting from the
 * end of the string buffer.
 * If `c` is not found anywhere within the string, the macro constant @ref CLZ_NOT_FOUND defined
 * within @ref clz.h is returned.
 *
 * @param destbuf The destination buffer (haystack)
 * @param c The needle
 * @return the position if found
 *
 * @see strbuf_find_first_char
 */
int strbuf_find_last_char(char **destbuf, char c);
/**
 * @brief Finds the first instance of the given substring.
 *
 * This function returns the index of the first position at which the substring `s` is found starting from the
 * beginning of the string buffer.
 * If `s` is not found anywhere within the string, the macro constant @ref CLZ_NOT_FOUND defined
 * within @ref clz.h is returned.
 *
 * @param destbuf The destination buffer (haystack)
 * @param s The needle
 * @return the position if found
 *
 * @see strbuf_find_last_str
 */
int strbuf_find_first_str(char **destbuf, char *s);
/**
 * @brief Finds the last instance of the given substring.
 *
 * This function returns the index of the first position at which the substring `s` is found starting from the
 * end of the string buffer.
 * If `s` is not found anywhere within the string, the macro constant @ref CLZ_NOT_FOUND defined
 * within @ref clz.h is returned.
 *
 * @param destbuf The destination buffer (haystack)
 * @param s The needle
 * @return the position if found
 *
 * @see strbuf_find_first_str
 */
int strbuf_find_last_str(char **destbuf, char *s);

/**
 * @brief Replaces the first instance of a `char`.
 *
 * This function finds and replaces the first instance of `c` within the string with `v`.
 * If `c` is not found anywhere, @ref CLZ_NOT_FOUND is returned, otherwise the position will be returned.
 *
 * This function makes use of heap allocation. If it fails, nothing is replaced and
 * @ref CLZ_GENERAL_FAIL is returned.
 *
 * @param destbuf The destination buffer
 * @param c The `char` to replace
 * @param v The replacement
 * @return the position if found
 *
 * @see strbuf_replace_all_char, strbuf_replace_first_str
 */
int strbuf_replace_first_char(char **destbuf, char c, char v);
/**
 * @brief Replaces all instances of a `char`.
 *
 * This function finds and replaces all instances of `c` within the string with `v`.
 * If `c` is not found anywhere, `0` is returned, otherwise the amount of replacements is returned.
 *
 * @param destbuf The destination buffer
 * @param c The `char` to replace
 * @param v The replacement
 * @return the number of replacements
 *
 * @see strbuf_replace_first_char, strbuf_replace_all_str
 */
size_t strbuf_replace_all_char(char **destbuf, char c, char v);
/**
 * @brief Replaces the given substring once.
 *
 * This function finds and replaces the first instance of `s` as a substring within the string with `t`.
 * If `s` is not found anywhere, @ref CLZ_NOT_FOUND is returned, otherwise the position of the first
 * `char` of `s` within `*destbuf` will be returned.
 *
 * @param destbuf The destination buffer
 * @param s The substring to replace
 * @param t The replacement
 * @return the position if found
 *
 * @see strbuf_replace_all_str, strbuf_replace_first_char
 */
int strbuf_replace_first_str(char **destbuf, char *s, char *t);
/**
 * @brief Replaces all instances of a substring.
 *
 * This function finds and replaces all instances of `s` as a substring within the string with `t`.
 * If `s` is not found anywhere, `0` is returned, otherwise the amount of replacements is returned.
 *
 * @param destbuf The destination buffer
 * @param s The string to replace
 * @param t The replacement
 * @return the number of replacements
 *
 * @see strbuf_replace_first_str, strbuf_replace_all_char
 */
size_t strbuf_replace_all_str(char **destbuf, char *s, char *t);

//size_t strbuf_split_char(char **destbuf, char **ptr, char c);

/**
 * @brief Removes the `char` at the given position.
 *
 * This function removes the `char` at `index` and shifts all subsequent characters including the
 * null-terminator by 1 byte towards the beginning, essentially filling the gap left by the removed
 * `char`.
 *
 * This function makes use of heap allocation, which may fail. If this happens, `false` is returned.
 * `false` is also returned if `start` is greater than or equal to `end`.
 *
 * @param destbuf The destination buffer
 * @param index The position to remove
 * @return `true` if successful
 *
 * @see strbuf_remove_str
 */
bool strbuf_remove_char(char **destbuf, size_t index);
/**
 * @brief Removes the given substring.
 *
 * This function removes the substring that starts from `start` and ends at `end` and shifts all subsequent characters including the
 * null-terminator by `end - start` bytes towards the beginning, essentially filling the gap left by the removed
 * substring.
 *
 * This function makes use of heap allocation, which may fail. If this happens, `false` is returned.
 * `false` is also returned if `start` is greater than or equal to `end` and/or `strlen(*destbuf)`.
 *
 * **Notes**
 *
 * This function is essentially the opposite of @ref strbuf_trim_index, which keeps the bytes between the
 * two given indices and discards the rest.
 *
 * @param destbuf The destination buffer
 * @param start The start of the substring to remove
 * @param end The end of the substring to remove
 * @return `true` if successful
 *
 * @see strbuf_remove_char, strbuf_trim_index
 */
bool strbuf_remove_str(char **destbuf, size_t start, size_t end);

/**
 * @brief Reduces all alpha characters to lowercase.
 *
 * This function replaces all uppercase characters that are part of the alphabet with their
 * lowercase counterpart. In other words, it calls `tolower` if `isupper` is true.
 *
 * @param destbuf The destination buffer
 *
 * @see strbuf_to_lowercase_l, strbuf_to_uppercase
 */
void strbuf_to_lowercase(char **destbuf);
/**
 * @brief Reduces all alpha characters to lowercase according to the given locale.
 *
 * This function replaces all uppercase characters that are part of the alphabet with their
 * lowercase counterpart. In other words, it calls `tolower_l` if `isupper_l` is true.
 *
 * @param destbuf The destination buffer
 * @param locale The chosen locale
 *
 * @see strbuf_to_lowercase_l, strbuf_to_uppercase
 */
void strbuf_to_lowercase_l(char **destbuf, locale_t locale);
/**
 * @brief Reduces all alpha characters to uppercase.
 *
 * This function replaces all lowercase characters that are part of the alphabet with their
 * uppercase counterpart. In other words, it calls `toupper` if `islower` is true.
 *
 * @param destbuf The destination buffer
 *
 * @see strbuf_to_uppercase_l, strbuf_to_lowercase
 */
void strbuf_to_uppercase(char **destbuf);
/**
 * @brief Reduces all alpha characters to uppercase according to the given locale.
 *
 * This function replaces all lowercase characters that are part of the alphabet with their
 * uppercase counterpart. In other words, it calls `toupper_l` if `islower_l` is true.
 *
 * @param destbuf The destination buffer
 * @param locale The chosen locale
 *
 * @see strbuf_to_uppercase_l, strbuf_to_lowercase
 */
void strbuf_to_uppercase_l(char **destbuf, locale_t locale);
/**
 * @brief Inverts the string.
 *
 * This function inverts the string within the buffer. It makes use of heap allocation and may thus fail.
 * If this is the case, `false` is returned and `errno` is set to `ENOMEM`. In all other cases, the
 * operation is meant to succeed and `true` is returned.
 *
 * @param destbuf The destination buffer
 * @return `true` on success
 */
bool strbuf_reverse(char **destbuf);

/**
 * Macro defining the default (starting) size for a string buffer.
 * All other sizes are meant to be powers of 2 greater than this value.
 */
#define CLZ_STRBUF_ALLOC 32

#endif

#ifdef CLZ_STRBUF_IMPL
#undef CLZ_STRBUF_IMPL

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

size_t strbuf_alloc_size(char *strbuf) {
    return *(((size_t *) strbuf) - 1);
}
char *strbuf_new() {
    return strbuf_new_size(CLZ_STRBUF_ALLOC);
}

char *strbuf_new_size(size_t sz) {
    size_t *sb = malloc(CLZ_STRBUF_ALLOC * sizeof(char) + sizeof(size_t));
    if (sb == NULL) return NULL;
    *sb = sz;
    return (char *) (sb + 1);
}

char *strbuf_new_str(char *s) {
    return strbuf_clone(s, false);
}

void strbuf_free(char *strbuf) {
    free(((size_t *) strbuf) - 1);
}

bool strbuf_append_char(char **destbuf, char c) {
    char s[2] = {c, 0};
    return strbuf_append_strn(destbuf, s, 1);
}

bool strbuf_append_str(char **dest, char *src) {
    return strbuf_append_strn(dest, src, strlen(src));
}

bool strbuf_append_strn(char **dest, char *src, size_t n) {
    size_t minsize = strlen(*dest) + strlen(src) + 1;
    if (strbuf_alloc_size(*dest) < minsize) {
        if (!strbuf_resize(dest, minsize)) {
            return false;
        }
    }
    strncpy(*dest + strlen(*dest), src, n);
    return true;
}

bool strbuf_append_int(char **destbuf, int i) {
    char val[12]; // max int has 10 digits plus one potential sign and null-terminator
    sprintf(val, "%d", i);
    return strbuf_append_str(destbuf, val);
}

bool strbuf_append_uint(char **destbuf, unsigned int i) {
    char val[11]; // max uint has 10 digits plus null-terminator
    sprintf(val, "%u", i);
    return strbuf_append_str(destbuf, val);
}

bool strbuf_append_long(char **destbuf, long l) {
    char val[21]; // max long has 19 digits plus one potential sign and null-terminator
    sprintf(val, "%ld", l);
    return strbuf_append_str(destbuf, val);
}

bool strbuf_append_ulong(char **destbuf, unsigned long l) {
    char val[21]; // max long has 20 digits plus null-terminator
    sprintf(val, "%lu", l);
    return strbuf_append_str(destbuf, val);
}

bool strbuf_append_llong(char **destbuf, long long l) {
    char val[21]; // max long has 19 digits plus one potential sign and null-terminator
    sprintf(val, "%lld", l);
    return strbuf_append_str(destbuf, val);
}

bool strbuf_append_ullong(char **destbuf, unsigned long long l) {
    char val[21]; // max long has 20 digits plus null-terminator
    sprintf(val, "%llu", l);
    return strbuf_append_str(destbuf, val);
}

bool strbuf_insert_char(char **destbuf, char c, size_t index) {
    if (index > strlen(*destbuf)) return false;
    char *bufnew = strbuf_new_size(strbuf_alloc_size(*destbuf) + 1);
    if (!bufnew) return false;
    bool ret = strbuf_append_strn(&bufnew, *destbuf, index)
            && strbuf_append_char(&bufnew, c)
            && strbuf_append_str(&bufnew, *destbuf + index);
    if (!ret) {
        strbuf_free(bufnew);
        return false;
    }
    strbuf_free(*destbuf);
    *destbuf = bufnew;
    return true;
}

bool strbuf_insert_str(char **destbuf, char *s, size_t index) {
    return strbuf_insert_strn(destbuf, s, index, strlen(s));
}

bool strbuf_insert_strn(char **destbuf, char *s, size_t index, size_t maxlen) {
    if (index > strlen(*destbuf)) return false;
    size_t len_s = strlen(s);
    if (maxlen > len_s) maxlen = len_s;
    char *bufnew = strbuf_new_size(strbuf_alloc_size(*destbuf) + maxlen);
    if (!bufnew) return false;
    bool ret = strbuf_append_strn(&bufnew, *destbuf, index)
            && strbuf_append_strn(&bufnew, s, maxlen)
            && strbuf_append_str(&bufnew, *destbuf + index);
    if (!ret) {
        strbuf_free(bufnew);
        return false;
    }
    strbuf_free(*destbuf);
    *destbuf = bufnew;
    return true;
}

bool strbuf_insert_int(char **destbuf, int i, size_t index) {
    char *intbuf = strbuf_new();
    if (!intbuf) return false;
    bool ret = strbuf_append_int(&intbuf, i)
            && strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_uint(char **destbuf, unsigned int i, size_t index) {
    char *intbuf = strbuf_new();
    if (!intbuf) return false;
    bool ret = strbuf_append_uint(&intbuf, i)
            && strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_long(char **destbuf, long l, size_t index) {
    char *intbuf = strbuf_new();
    if (!intbuf) return false;
    bool ret = strbuf_append_long(&intbuf, l)
            && strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_ulong(char **destbuf, unsigned long l, size_t index) {
    char *intbuf = strbuf_new();
    if (!intbuf) return false;
    bool ret = strbuf_append_ulong(&intbuf, l)
            && strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_llong(char **destbuf, long long l, size_t index) {
    char *intbuf = strbuf_new();
    if (!intbuf) return false;
    bool ret = strbuf_append_llong(&intbuf, l)
            && strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_ullong(char **destbuf, unsigned long long l, size_t index) {
    char *intbuf = strbuf_new();
    if (!intbuf) return false;
    bool ret = strbuf_append_ullong(&intbuf, l) &&
            strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_resize(char **dest, size_t minsize) {
    size_t sz, len = strlen(*dest);
    if (len >= minsize + 1) return false;
    for (sz = len + 1; sz < minsize; sz *= 2);

    char *newbuf = malloc(sz + sizeof(size_t));
    if (!newbuf) return false;

    *((size_t *) newbuf) = sz;
    strcpy(newbuf + sizeof(size_t), *dest);
    free(((size_t *)*dest) - 1);
    *dest = newbuf + sizeof(size_t);
    return true;
}

bool strbuf_compress(char **dest) {
    return strbuf_resize(dest, strlen(*dest));
}

void strbuf_trim_index(char **destbuf, size_t start, size_t end) {
    if (end > strlen(*destbuf)) {
        end = strlen(*destbuf);
    }
    // not else
    if (start >= strlen(*destbuf) || start >= end) {
        **destbuf = 0;
        return;
    }
    else if (end == 0) return;

    char *cpy = malloc(end - start + 1);
    if (!cpy) return;
    strncpy(cpy, *destbuf + start, end - start);
    strcpy(*destbuf, cpy);
    free(cpy);
}

void strbuf_trim_length(char **destbuf, size_t length) {
    strbuf_trim_index(destbuf, 0, length);
}

void strbuf_trim_head(char **destbuf) {
    strbuf_trim_head_char(destbuf, ' ');
}

void strbuf_trim_head_char(char **dest, char c) {
    char *head = *dest;
    for (; *head != 0 && *head == c; ++head);
    char *cpy = strdup(head);
    if (!cpy) return;
    strcpy(*dest, head);
    free(cpy);
}

void strbuf_trim_tail(char **destbuf) {
    strbuf_trim_tail_char(destbuf, ' ');
}

void strbuf_trim_tail_char(char **dest, char c) {
    char *tail = *dest + strlen(*dest) - 1;
    for (; tail != *dest - 1 && *tail == c; --tail) {
    }
    *++tail = 0;
}

bool strbuf_padding_head(char **destbuf, char c, size_t sz) {
    size_t len = strlen(*destbuf);
    if (len > sz) return false;
    else if (len == sz) return true;
    size_t padlen = sz - len;
    char *padbuf = malloc(padlen + 1);
    if (!padbuf) return false;
    memset(padbuf, c, padlen);
    bool ret = strbuf_insert_str(destbuf, padbuf, 0);
    free(padbuf);
    return ret;
}

bool strbuf_padding_tail(char **destbuf, char c, size_t sz) {
    size_t len = strlen(*destbuf);
    if (len > sz) return false;
    else if (len == sz) return true;
    size_t padlen = sz - len;
    char *padbuf = malloc(padlen + 1);
    if (!padbuf) return false;
    memset(padbuf, c, padlen);
    bool ret = strbuf_append_str(destbuf, padbuf);
    free(padbuf);
    return ret;
}

char *strbuf_clone(char *strbuf, bool bufsz) {
    char *newbuf;
    if (bufsz) {
        newbuf = strbuf_new_size(strbuf_alloc_size(strbuf));
    } else {
        size_t s = 2;
        while (s <= strlen(strbuf) || s < CLZ_STRBUF_ALLOC) {
            s *= 2;
        }
        newbuf = strbuf_new_size(s);
    }

    if (!newbuf) return NULL;
    else if (!strbuf_append_str(&newbuf, strbuf)) {
        strbuf_free(newbuf);
        return NULL;
    }
    return newbuf;
}

int strbuf_find_first_char(char **destbuf, char c) {
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if ((*destbuf)[i] == c) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_replace_first_char(char **destbuf, char c, char v) {
    int i = strbuf_find_first_char(destbuf, c);
    if (i == CLZ_NOT_FOUND) return CLZ_NOT_FOUND;
    (*destbuf)[i] = v;
    return i;
}

size_t strbuf_replace_all_char(char **destbuf, char c, char v) {
    size_t count = 0;
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if ((*destbuf)[i] == c) {
            (*destbuf)[i] = v;
            ++count;
        }
    }
    return count;
}

/*
size_t strbuf_split_char(char **destbuf, char **ptr, char c) {
    size_t arrlen = 0;
    for (size_t i = 1; i < strlen(*destbuf); ++i) {
        if (((*destbuf)[i]) == c) {
            if (((*destbuf)[i - 1]) == c) continue;
            ++arrlen;
        }
    }

    if (!arrlen) {
        *ptr = NULL;
        return 0;
    }

    *ptr = calloc(arrlen, sizeof(char *));
    if (!ptr) return 0;

    size_t j = 0;
    while (j < arrlen) {
        ptr[j] = strndup(c);
    }
}*/

int strbuf_find_last_char(char **destbuf, char c) {
    for (int i = strlen(*destbuf) - 1; i >= 0; --i) {
        if ((*destbuf)[i] == c) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_find_first_str(char **destbuf, char *s) {
    size_t len = strlen(s);
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if (!strncmp(*destbuf + i, s, len)) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_find_last_str(char **destbuf, char *s) {
    for (int i = strlen(*destbuf) - 1; i >= 0; --i) {
        if (!strncmp(*destbuf + i, s, strlen(s))) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_replace_first_str(char **destbuf, char *s, char *t) {
    int ind = strbuf_find_first_str(destbuf, s);
    if (ind == CLZ_NOT_FOUND) return CLZ_NOT_FOUND;

    char *start = strbuf_clone(*destbuf, false);
    strbuf_trim_index(&start, 0, ind);
    char *end = strbuf_clone(*destbuf, false);
    strbuf_trim_index(&end, ind + strlen(s), strlen(*destbuf));

    char *buf = malloc(strlen(*destbuf) + strlen(t) - strlen(s) + 1);
    if (!buf) return CLZ_GENERAL_FAIL;
    sprintf(buf, "%s%s%s", start, t, end);
    size_t len = strlen(buf);
    if (len >= strbuf_alloc_size(*destbuf)) {
        strbuf_resize(destbuf, len + 1);
    }

    strcpy(*destbuf, buf);
    free(buf);
    return ind;
}

size_t strbuf_replace_all_str(char **destbuf, char *s, char *t) {
    size_t count = 0, lenneedle = strlen(s);
    int head;
    char *newbuf = strbuf_new_size(strbuf_alloc_size(*destbuf));
    char *haystack = *destbuf;

    while ((head = strbuf_find_first_str(&haystack, s)) != CLZ_NOT_FOUND) {
        strbuf_append_strn(&newbuf, haystack, head);
        strbuf_append_str(&newbuf, t);
        haystack += head + lenneedle;
        ++count;
    }
    strbuf_append_str(&newbuf, haystack);
    strbuf_free(*destbuf);
    *destbuf = newbuf;
    return count;
}

bool strbuf_remove_char(char **destbuf, size_t index) {
    if (index >= strlen(*destbuf)) return false;
    char *newbuf = strbuf_new_size(strbuf_alloc_size(*destbuf));
    if (!newbuf) return false;
    bool ret  = strbuf_append_strn(&newbuf, *destbuf, index)
            && strbuf_append_str(&newbuf, *destbuf + index + 1);
    if (!ret) {
        strbuf_free(newbuf);
        return false;
    }
    strbuf_free(*destbuf);
    *destbuf = newbuf;
    return true;
}

bool strbuf_remove_str(char **destbuf, size_t start, size_t end) {
    if (start >= end) return false;
    size_t len = strlen(*destbuf);
    if (start >= len) return false;
    else if (end >= len) end = len;

    char *newbuf = strbuf_new_size(strbuf_alloc_size(*destbuf));
    if (!newbuf) return false;
    bool ret = strbuf_append_strn(&newbuf, *destbuf, start)
            && strbuf_append_str(&newbuf, *destbuf + end);
    if (!ret) {
        strbuf_free(newbuf);
        return false;
    }
    strbuf_free(*destbuf);
    *destbuf = newbuf;
    return true;
}

void strbuf_to_lowercase(char **destbuf) {
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if (isupper((*destbuf)[i])) {
            (*destbuf)[i] = tolower((*destbuf)[i]);
        }
    }
}

void strbuf_to_lowercase_l(char **destbuf, locale_t locale) {
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if (isupper_l((*destbuf)[i], locale)) {
            (*destbuf)[i] = tolower_l((*destbuf)[i], locale);
        }
    }
}

void strbuf_to_uppercase(char **destbuf) {
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if (islower((*destbuf)[i])) {
            (*destbuf)[i] = toupper((*destbuf)[i]);
        }
    }
}

void strbuf_to_uppercase_l(char **destbuf, locale_t locale) {
    for (size_t i = 0; i < strlen(*destbuf); ++i) {
        if (islower_l((*destbuf)[i], locale)) {
            (*destbuf)[i] = toupper_l((*destbuf)[i], locale);
        }
    }
}

bool strbuf_reverse(char **destbuf) {
    char *cpy = strdup(*destbuf);
    if (!cpy) return false;
    size_t len = strlen(*destbuf);
    for (size_t i = 0; i < len; ++i) {
        (*destbuf)[i] = cpy[len - 1 - i];
    }
    free(cpy);
    return true;
}

#endif
