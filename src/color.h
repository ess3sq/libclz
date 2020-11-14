#ifndef _CLZ_COLOR_H
#define _CLZ_COLOR_H

#define COL_ESC		 "\033"
#define COL_RED		 "\033[0;31m"
#define COL_BOLD_RED	 "\033[1;31m"
#define COL_GREEN	 "\033[0;32m"
#define COL_BOLD_GREEN	 "\033[1;32m"
#define COL_YELLOW	 "\033[0;33m"
#define COL_BOLD_YELLOW	 "\033[1;33m"
#define COL_BLUE	 "\033[0;34m"
#define COL_BOLD_BLUE	 "\033[1;34m"
#define COL_MAGENTA	 "\033[0;35m"
#define COL_BOLD_MAGENTA "\033[1;35m"
#define COL_CYAN	 "\033[0;36m"
#define COL_BOLD_CYAN	 "\033[1;36m"
#define COL_RESET	 "\033[0m"

void col_red();
void col_bold_red();

void col_green();
void col_bold_green();

void col_yellow();
void col_bold_yellow();

void col_blue();
void col_bold_blue();

void col_magenta();
void col_bold_magenta();

void col_cyan();
void col_bold_cyan();

void col_reset();
#endif

// IMPLEMENTATION
#ifdef CLZ_COLOR_IMPL

#include <stdio.h>

void col_red() {
    printf(COL_RED);
}

void col_bold_red() {
    printf(COL_BOLD_RED);
}

void col_green() {
    printf(COL_GREEN);
}

void col_bold_green() {
    printf(COL_BOLD_GREEN);
}

void col_yellow() {
    printf(COL_YELLOW);
}

void col_bold_yellow() {
    printf(COL_BOLD_YELLOW);
}

void col_blue() {
    printf(COL_BLUE);
}

void col_bold_blue() {
    printf(COL_BOLD_BLUE);
}

void col_magenta() {
    printf(COL_MAGENTA);
}

void col_bold_magenta() {
    printf(COL_BOLD_MAGENTA);
}

void col_cyan() {
    printf(COL_CYAN);
}

void col_bold_cyan() {
    printf(COL_BOLD_CYAN);
}

void col_reset() {
    printf(COL_RESET);
}

#endif

