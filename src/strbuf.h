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
 * @brief Header file containing the declarations for string operations and character or byte buffers
 * and its related functionalities
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
#include <stdbool.h>

#include "clz.h"
#include "def.h"

size_t strbuf_alloc_size(char *strbuf);
bool strbuf_extend(char **dest, size_t minsize);

        char *strbuf_new();
char *strbuf_new_size(size_t sz);
void strbuf_free(char *strbuf);
char *strbuf_clone(char *strbuf, bool bufsz);

bool strbuf_append_char(char **destbuf, char c);
bool strbuf_append_str(char **destbuf, char *src);
bool strbuf_append_strn(char **destbuf, char *src, size_t n);

void strbuf_trim_index(char **destbuf, size_t start, size_t end);
void strbuf_trim_length(char **destbuf, size_t length);
void strbuf_trim_head(char **destbuf);
void strbuf_trim_head_char(char **destbuf, char c);
void strbuf_trim_tail(char **destbuf);
void strbuf_trim_tail_char(char **destbuf, char c);

int strbuf_find_char(char **destbuf, char c);
int strbuf_replace_first_char(char **destbuf, char c, char v);
size_t strbuf_replace_all_char(char **destbuf, char c, char v);

int strbuf_find_str(char **destbuf, char *s);
int strbuf_replace_first_str(char **destbuf, char *s, char *t);
//size_t strbuf_replace_all_str(char **destbuf, char *s, char *t);

void strbuf_to_lowercase(char **destbuf);
void strbuf_to_uppercase(char **destbuf);

#define CLZ_STRBUF_ALLOC 32

#endif

#ifdef CLZ_STRBUF_IMPL

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

size_t strbuf_alloc_size(char *strbuf) {
    return *(((size_t *) strbuf) - 1);
}
char *strbuf_new() {
    return strbuf_new_size(CLZ_STRBUF_ALLOC);
}

char *strbuf_new_size(size_t sz) {
    size_t *sb = malloc(CLZ_STRBUF_ALLOC * sizeof(char) + sizeof(size_t));
    *sb = sz;
    return (char *) (sb + 1);
}

void strbuf_free(char *strbuf) {
    free(((size_t *) strbuf) - 1);
}

bool strbuf_append_char(char **destbuf, char c) {
    char s[2] = {c, 0};
    strbuf_append_strn(destbuf, s, 1);
}

bool strbuf_append_str(char **dest, char *src) {
    return strbuf_append_strn(dest, src, strlen(src));
}

bool strbuf_append_strn(char **dest, char *src, size_t n) {
    size_t minsize = strlen(*dest) + strlen(src) + 1;
    if (strbuf_alloc_size(*dest) < minsize) {
        if (!strbuf_extend(dest, minsize)) {
            return false;
        }
    }
    strncpy(*dest + strlen(*dest), src, n);
    return true;
}

bool strbuf_extend(char **dest, size_t minsize) {
    size_t sz;
    for (sz = strbuf_alloc_size(*dest); sz < minsize; sz *= 2);

    char *newbuf = malloc(sz + sizeof(size_t));
    if (newbuf == NULL) return false;

    *((size_t *) newbuf) = sz;
    strcpy(newbuf + sizeof(size_t), *dest);
    free(((size_t *)*dest) - 1);
    *dest = newbuf + sizeof(size_t);
    return true;
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
    strcpy(*dest, head);
    free(cpy);
}

void strbuf_trim_tail(char **destbuf) {
        strbuf_trim_tail_char(destbuf, ' ');
}

void strbuf_trim_tail_char(char **dest, char c) {
    char *tail = *dest + strlen(*dest) - 1;
    for (; tail != *dest - 1 && *tail == c; --tail) {
        *tail = 0;
    }
}

char *strbuf_clone(char *strbuf, bool bufsz) {
    char *newbuf = strbuf_new();
    if (bufsz) {
        strbuf_extend(&newbuf, strbuf_alloc_size(strbuf));
    }
    strbuf_append_str(&newbuf, strbuf);
    return newbuf;
}

int strbuf_find_char(char **destbuf, char c) {
    for (int i = 0; i < strlen(*destbuf); ++i) {
        if ((*destbuf)[i] == c) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_replace_first_char(char **destbuf, char c, char v) {
    int i = strbuf_find_char(destbuf, c);
    if (i == CLZ_NOT_FOUND) return CLZ_NOT_FOUND;
    (*destbuf)[i] = v;
}

size_t strbuf_replace_all_char(char **destbuf, char c, char v) {
    size_t count = 0;
    for (int i = 0; i < strlen(*destbuf); ++i) {
        if ((*destbuf)[i] == c) {
            (*destbuf)[i] = v;
            ++count;
        }
    }
    return count;
}

int strbuf_find_str(char **destbuf, char *s) {
    size_t len = strlen(s);
    for (int i = 0; i < strlen(*destbuf); ++i) {
        int j;
        for (j = 0; j < len; ++j) {
            if ((*destbuf)[i + j] != s[j]) break;
        }
        if (j == len) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_replace_first_str(char **destbuf, char *s, char *t) {
    int ind = strbuf_find_str(destbuf, s);
    if (ind == CLZ_NOT_FOUND) return CLZ_NOT_FOUND;

    char *start = strbuf_clone(*destbuf, false);
    strbuf_trim_index(&start, 0, ind);
    char *end = strbuf_clone(*destbuf, false);
    strbuf_trim_index(&end, ind + strlen(s), strlen(*destbuf));

    char *buf = malloc(strlen(*destbuf) + strlen(t) - strlen(s) + 1);
    sprintf(buf, "%s%s%s", start, t, end);
    size_t len = strlen(buf);
    if (len >= strbuf_alloc_size(*destbuf)) {
        strbuf_extend(destbuf, len + 1);
    }

    strcpy(*destbuf, buf);
    free(buf);
    return ind;
}
/*
size_t strbuf_replace_all_str(char **destbuf, char *s, char *t) {
    size_t count = 0;
    if (strbuf_find_str(destbuf, s) == CLZ_NOT_FOUND) return 0;

    int i = 0, j;
    while (i != strlen(*destbuf)) {
        j = strbuf_find_str(destbuf, s);
        //if (j )
    }
}*/

void strbuf_to_lowercase(char **destbuf) {
    for (int i = 0; i < strlen(*destbuf); ++i) {
        if (isupper((*destbuf)[i])) {
            (*destbuf)[i] = tolower((*destbuf)[i]);
        }
    }
}

void strbuf_to_uppercase(char **destbuf) {
    for (int i = 0; i < strlen(*destbuf); ++i) {
        if (islower((*destbuf)[i])) {
            (*destbuf)[i] = toupper((*destbuf)[i]);
        }
    }
}

#endif
