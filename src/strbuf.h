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
bool strbuf_append_int(char **destbuf, int i);
bool strbuf_append_uint(char **destbuf, unsigned int i);
bool strbuf_append_long(char **destbuf, long l);
bool strbuf_append_ulong(char **destbuf, unsigned long l);
bool strbuf_append_llong(char **destbuf, long  long l);
bool strbuf_append_ullong(char **destbuf, unsigned long long l);

bool strbuf_insert_char(char **destbuf, char c, size_t index);
bool strbuf_insert_str(char **destbuf, char *s, size_t index);
bool strbuf_insert_strn(char **destbuf, char *s, size_t index, size_t maxlen);
bool strbuf_insert_int(char **destbuf, int i, size_t index);
bool strbuf_insert_uint(char **destbuf, unsigned int i, size_t index);
bool strbuf_insert_long(char **destbuf, long l, size_t index);
bool strbuf_insert_ulong(char **destbuf, unsigned long l, size_t index);
bool strbuf_insert_llong(char **destbuf, long long l, size_t index);
bool strbuf_insert_ullong(char **destbuf, unsigned long long l, size_t index);

void strbuf_trim_index(char **destbuf, size_t start, size_t end);
void strbuf_trim_length(char **destbuf, size_t length);
void strbuf_trim_head(char **destbuf);
void strbuf_trim_head_char(char **destbuf, char c);
void strbuf_trim_tail(char **destbuf);
void strbuf_trim_tail_char(char **destbuf, char c);

bool strbuf_padding_head(char **destbuf, char c, size_t sz);
bool strbuf_padding_tail(char **destbuf, char c, size_t sz);

int strbuf_find_first_char(char **destbuf, char c);
int strbuf_find_last_char(char **destbuf, char c);
int strbuf_replace_first_char(char **destbuf, char c, char v);
size_t strbuf_replace_all_char(char **destbuf, char c, char v);

int strbuf_find_first_str(char **destbuf, char *s);
int strbuf_find_last_str(char **destbuf, char *s);
int strbuf_replace_first_str(char **destbuf, char *s, char *t);
size_t strbuf_replace_all_str(char **destbuf, char *s, char *t);

bool strbuf_remove_char(char **destbuf, size_t index);
bool strbuf_remove_str(char **destbuf, size_t start, size_t end);

void strbuf_to_lowercase(char **destbuf);
void strbuf_to_uppercase(char **destbuf);
void strbuf_reverse(char **destbuf);

#define CLZ_STRBUF_ALLOC 32

#endif

#ifdef CLZ_STRBUF_IMPL

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

bool strbuf_append_int(char **destbuf, int i) {
    char val[12]; // max int has 10 digits plus one potential sign and null-terminator
    sprintf(val, "%d", i);
    bool ret = strbuf_append_str(destbuf, val);
    return ret;
}

bool strbuf_append_uint(char **destbuf, unsigned int i) {
    char val[11]; // max uint has 10 digits plus null-terminator
    sprintf(val, "%u", i);
    bool ret = strbuf_append_str(destbuf, val);
    return ret;
}

bool strbuf_append_long(char **destbuf, long l) {
    char val[21]; // max long has 19 digits plus one potential sign and null-terminator
    sprintf(val, "%ld", l);
    bool ret = strbuf_append_str(destbuf, val);
    return ret;
}

bool strbuf_append_ulong(char **destbuf, unsigned long l) {
    char val[21]; // max long has 20 digits plus null-terminator
    sprintf(val, "%lu", l);
    bool ret = strbuf_append_str(destbuf, val);
    return ret;
}

bool strbuf_append_llong(char **destbuf, long long l) {
    char val[21]; // max long has 19 digits plus one potential sign and null-terminator
    sprintf(val, "%lld", l);
    bool ret = strbuf_append_str(destbuf, val);
    return ret;
}

bool strbuf_append_ullong(char **destbuf, unsigned long long l) {
    char val[21]; // max long has 20 digits plus null-terminator
    sprintf(val, "%llu", l);
    bool ret = strbuf_append_str(destbuf, val);
    return ret;
}

bool strbuf_insert_char(char **destbuf, char c, size_t index) {
    if (index > strlen(*destbuf)) return false;
    char *bufnew = strbuf_new_size(strbuf_alloc_size(*destbuf) + 1);
    strbuf_append_strn(&bufnew, *destbuf, index);
    strbuf_append_char(&bufnew, c);
    strbuf_append_str(&bufnew, *destbuf + index);
    strbuf_free(*destbuf);
    *destbuf = bufnew;
    return true;
}

bool strbuf_insert_str(char **destbuf, char *s, size_t index) {
    strbuf_insert_strn(destbuf, s, index, strlen(s));
}

bool strbuf_insert_strn(char **destbuf, char *s, size_t index, size_t maxlen) {
    if (index > strlen(*destbuf)) return false;
    size_t len_s = strlen(s);
    if (maxlen > len_s) maxlen = len_s;
    char *bufnew = strbuf_new_size(strbuf_alloc_size(*destbuf) + maxlen);
    strbuf_append_strn(&bufnew, *destbuf, index);
    strbuf_append_strn(&bufnew, s, maxlen);
    strbuf_append_str(&bufnew, *destbuf + index);
    strbuf_free(*destbuf);
    *destbuf = bufnew;
    return true;
}

bool strbuf_insert_int(char **destbuf, int i, size_t index) {
    char *intbuf = strbuf_new();
    strbuf_append_int(&intbuf, i);
    bool ret = strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_uint(char **destbuf, unsigned int i, size_t index) {
    char *intbuf = strbuf_new();
    strbuf_append_uint(&intbuf, i);
    bool ret = strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_long(char **destbuf, long l, size_t index) {
    char *intbuf = strbuf_new();
    strbuf_append_long(&intbuf, l);
    bool ret = strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_ulong(char **destbuf, unsigned long l, size_t index) {
    char *intbuf = strbuf_new();
    strbuf_append_ulong(&intbuf, l);
    bool ret = strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_llong(char **destbuf, long long l, size_t index) {
    char *intbuf = strbuf_new();
    strbuf_append_llong(&intbuf, l);
    bool ret = strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
}

bool strbuf_insert_ullong(char **destbuf, unsigned long long l, size_t index) {
    char *intbuf = strbuf_new();
    strbuf_append_ullong(&intbuf, l);
    bool ret = strbuf_insert_str(destbuf, intbuf, index);
    strbuf_free(intbuf);
    return ret;
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

bool strbuf_padding_head(char **destbuf, char c, size_t sz) {
    size_t len = strlen(*destbuf);
    if (len > sz) return false;
    else if (len == sz) return true;
    size_t padlen = sz - len;
    char *padbuf = malloc(padlen + 1);
    memset(padbuf, c, padlen);
    strbuf_insert_str(destbuf, padbuf, 0);
    free(padbuf);
}

bool strbuf_padding_tail(char **destbuf, char c, size_t sz) {
    size_t len = strlen(*destbuf);
    if (len > sz) return false;
    else if (len == sz) return true;
    size_t padlen = sz - len;
    char *padbuf = malloc(padlen + 1);
    memset(padbuf, c, padlen);
    strbuf_append_str(destbuf, padbuf);
    free(padbuf);
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
    strbuf_append_str(&newbuf, strbuf);
    return newbuf;
}

int strbuf_find_first_char(char **destbuf, char c) {
    for (int i = 0; i < strlen(*destbuf); ++i) {
        if ((*destbuf)[i] == c) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_replace_first_char(char **destbuf, char c, char v) {
    int i = strbuf_find_first_char(destbuf, c);
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

int strbuf_find_last_char(char **destbuf, char c) {
    for (int i = strlen(*destbuf) - 1; i >= 0; --i) {
        if ((*destbuf)[i] == c) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_find_first_str(char **destbuf, char *s) {
    size_t len = strlen(s);
    for (int i = 0; i < strlen(*destbuf); ++i) {
        if (!strncmp(*destbuf + i, s, len)) return i;
    }
    return CLZ_NOT_FOUND;
}

int strbuf_find_last_str(char **destbuf, char *s) {
    size_t len = strlen(s);
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
    char *cpy = strbuf_clone(*destbuf);
    **destbuf = 0;

    int i, j;
    for (i = 0, j = 0; i < strlen(cpy) && (j = strbuf_find_first_str(&(cpy + i), s) != CLZ_NOT_FOUND; ++count) {
        strbuf_replace_first_string(destbuf)
    }
    free(cpy);
    return count;
}*/

bool strbuf_remove_char(char **destbuf, size_t index) {
    if (index >= strlen(*destbuf)) return false;
    char *newbuf = strbuf_new_size(strbuf_alloc_size(*destbuf));
    if (!newbuf) return false;
    strbuf_append_strn(&newbuf, *destbuf, index);
    strbuf_append_str(&newbuf, *destbuf + index + 1);
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
    if (!newbuf) return NULL;
    strbuf_append_strn(&newbuf, *destbuf, start);
    strbuf_append_str(&newbuf, *destbuf + end);
    strbuf_free(*destbuf);
    *destbuf = newbuf;
    return true;
}

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

void strbuf_reverse(char **destbuf) {
    char *cpy = strdup(*destbuf);
    size_t len = strlen(*destbuf);
    for (int i = 0; i < len; ++i) {
        (*destbuf)[i] = cpy[len - 1 - i];
    }
    free(cpy);
}

#endif
