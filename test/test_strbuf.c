#define CLZ_STRBUF_IMPL
#define B_PASS_SILENTLY

#include <test-beam/beam.h>
#include "../src/strbuf.h"
#include <stdbool.h>
#include <string.h>

#define PASS_IF(cond)   \
    if (succ) {         \
        B_PASS();       \
    }                   \
    else {              \
        B_FAIL();       \
    }                   \


void test_create_free() {
    char *buf = strbuf_new();
    bool succ = true;
    if (strcmp(buf, "")) succ = false;
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
    strbuf_free(buf);

    buf = strbuf_new_str("hello");
    if (strcmp(buf, "hello")) succ = false;
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
    strbuf_free(buf);

    PASS_IF(succ);
}

void test_new_size() {
    bool succ = true;
    char *buf;

    buf = strbuf_new_size(CLZ_STRBUF_ALLOC + 2);
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC * 2) succ = false;
    strbuf_free(buf);

    buf = strbuf_new_size(0);
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
    strbuf_free(buf);

    size_t small_sizes[] = {0, 1, 2, 3, 4, 7, 8, 15, 16, 20, 30, 32};
    for (size_t i = 0; i < sizeof(small_sizes) / sizeof(*small_sizes); ++i) {
        buf = strbuf_new_size(small_sizes[i]);
        if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
        strbuf_free(buf);
    }

    PASS_IF(succ);
}

void test_new_str() {
    char *s   = "Hello, World!";
    char *buf = strbuf_new_str(s);
    bool succ = true;

    if (strcmp(buf, s)) succ = false;
    if (strlen(s) != strlen(buf)) succ = false;
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;

    strbuf_free(buf);
    PASS_IF(succ);
}

#define REP7(s) (s#s#s#s#s#s#s)

void test_clone() {
    bool succ = true;

    char *s = "Hello, World!";
    char *buf = strbuf_new_str(s);
    char *buf2 = strbuf_clone(buf, true);
    char *buf3;

    if (strcmp(s, buf)) succ = false;
    if (strcmp(buf, buf2)) succ = false;
    if (strlen(buf) != strlen(buf2)) succ = false;
    if (strbuf_alloc_size(buf) != strbuf_alloc_size(buf2)) succ = false;

    strbuf_free(buf);
    strbuf_free(buf2);

    buf = strbuf_new_size(CLZ_STRBUF_ALLOC + 12);
    buf2 = strbuf_clone(buf, true);
    buf3 = strbuf_clone(buf, false);
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC * 2) succ = false;
    if (strbuf_alloc_size(buf2) != strbuf_alloc_size(buf)) succ = false;
    if (strbuf_alloc_size(buf3) == strbuf_alloc_size(buf)) succ = false;
    if (strbuf_alloc_size(buf3) != CLZ_STRBUF_ALLOC) succ = false;

    strbuf_free(buf);
    strbuf_free(buf2);
    strbuf_free(buf3);

    buf = strbuf_new_size(CLZ_STRBUF_ALLOC + 12);
    buf2 = strbuf_clone(buf, true);
    buf3 = strbuf_clone(buf, false);
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC * 2) succ = false;
    if (strbuf_alloc_size(buf2) != strbuf_alloc_size(buf)) succ = false;
    if (strbuf_alloc_size(buf3) == strbuf_alloc_size(buf)) succ = false;
    if (strbuf_alloc_size(buf3) != CLZ_STRBUF_ALLOC) succ = false;
    if (strlen(buf) != strlen(buf2)) succ = false;
    if (strlen(buf3) != strlen(buf2)) succ = false;
    if (strcmp(buf, buf2) || strcmp(buf, buf3)) succ = false;

    strbuf_free(buf);
    strbuf_free(buf2);
    strbuf_free(buf3);

    s = REP7("This is a long string, at least 50 chars long. I should know because I have counted them!");
    buf = strbuf_new_str(s);
    buf2 = strbuf_clone(buf, true);
    buf3 = strbuf_clone(buf, false);

    if (strbuf_alloc_size(buf) != 1024) succ = false;
    if (strbuf_alloc_size(buf2) != 1024) succ = false;
    if (strbuf_alloc_size(buf3) != 1024) succ = false;
    if (strlen(buf) != strlen(buf2)) succ = false;
    if (strlen(buf3) != strlen(buf2)) succ = false;
    if (strcmp(buf, buf2) || strcmp(buf, buf3)) succ = false;

    strbuf_free(buf);
    strbuf_free(buf2);
    strbuf_free(buf3);

    s = "This is a short string.";
    buf = strbuf_new_size(CLZ_STRBUF_ALLOC * 2);
    strcpy(buf, s);
    buf2 = strbuf_clone(buf, true);
    buf3 = strbuf_clone(buf, false);

    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC * 2) succ = false;
    if (strbuf_alloc_size(buf2) != strbuf_alloc_size(buf)) succ = false;
    if (strbuf_alloc_size(buf3) != CLZ_STRBUF_ALLOC) succ = false;
    if (strlen(buf) != strlen(buf2)) succ = false;
    if (strlen(buf3) != strlen(buf2)) succ = false;
    if (strcmp(buf, buf2) || strcmp(buf, buf3)) succ = false;

    strbuf_free(buf);
    strbuf_free(buf2);
    strbuf_free(buf3);

    PASS_IF(succ);
}

void test_resize() {
    char *buf = strbuf_new();
    bool succ = true;

    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
    strbuf_resize(&buf, 1);
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
    strbuf_resize(&buf, 0);
    if (strbuf_alloc_size(buf) != CLZ_STRBUF_ALLOC) succ = false;
    strbuf_resize(&buf, 33);
    if (strbuf_alloc_size(buf) != 64) succ = false;
    strbuf_resize(&buf, 1023);
    if (strbuf_alloc_size(buf) != 1024) succ = false;
    strbuf_resize(&buf, 63);
    if (strbuf_alloc_size(buf) != 64) succ = false;
    strbuf_resize(&buf, 5);
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_free(buf);

    buf = strbuf_new_str("Hello, World! This is a string 68 chars long plus a null-terminator!");
    if (strbuf_alloc_size(buf) != 128) succ = false;
    if (strlen(buf) != 68) succ = false;
    strbuf_resize(&buf, 256);
    if (strbuf_alloc_size(buf) != 256) succ = false;
    strbuf_resize(&buf, 128);
    if (strbuf_alloc_size(buf) != 128) succ = false;
    strbuf_resize(&buf, 55);
    if (strbuf_alloc_size(buf) != 128) succ = false;
    strbuf_resize(&buf, 64);
    if (strbuf_alloc_size(buf) != 128) succ = false;
    strbuf_resize(&buf, 70);
    if (strbuf_alloc_size(buf) != 128) succ = false;

    strbuf_resize(&buf, strlen(buf) + 1);
    if (strbuf_alloc_size(buf) != 128) succ = false;
    strbuf_resize(&buf, strlen(buf));
    if (strbuf_alloc_size(buf) != 128) succ = false;
    strbuf_resize(&buf, strlen(buf) - 1);
    if (strbuf_alloc_size(buf) != 128) succ = false;
    strbuf_resize(&buf, strlen(buf) + 2);
    if (strbuf_alloc_size(buf) != 128) succ = false;

    strbuf_free(buf);
    buf = strbuf_new_str("A string that is 31 chars long.");
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_resize(&buf, 31);
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_resize(&buf, 30);
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_resize(&buf, 1);
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_resize(&buf, 0);
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_resize(&buf, 32);
    if (strbuf_alloc_size(buf) != 32) succ = false;
    strbuf_resize(&buf, 33);
    if (strbuf_alloc_size(buf) != 64) succ = false;

    strbuf_free(buf);
    PASS_IF(succ);
}

int main(void) {
    test_create_free();
    test_new_size();
    test_new_str();
    test_clone();
    test_resize();

    B_SUMMARY();
    return 0;
}