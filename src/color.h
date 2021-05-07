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
 * @file color.h
 * @author Lorenzo Calza
 * @date 13 Nov 2020
 * @brief Header file containing the declarations for color console output utilities
 *
 * This header file contains the definitions of the color sequence macros and the standard output
 * setting functions
 *
 * **Implementation**
 *
 * The implementation can be included if the macro `CLZ_COLOR_IMPL` is defined beforehand.
 * If forward-declarations of the function prototypes can be skipped if the macro `CLZ_NO_COLOR_FUNCTIONS`
 * is found to be defined. In this case, the implementation part is skipped too, regardless of whether the
 * implementation flag is active or not.
 */

#ifndef _CLZ_COLOR_H
/**
 * @brief Include guard for this file.
 */
#define _CLZ_COLOR_H

/**
 * @brief Escape sequence for ASCII the escape sequence
 */
#define COL_ESC		 "\033"
/**
 * @brief Escape sequence for the ASCII red color
 */
#define COL_RED		 "\033[0;31m"
/**
 * @brief Escape sequence for the ASCII bold red color
 */
#define COL_BOLD_RED	 "\033[1;31m"
/**
 * @brief Escape sequence for the ASCII green color
 */
#define COL_GREEN	 "\033[0;32m"
/**
 * @brief Escape sequence for the ASCII bold green color
 */
#define COL_BOLD_GREEN	 "\033[1;32m"
/**
 * @brief Escape sequence for the ASCII yellow color
 */
#define COL_YELLOW	 "\033[0;33m"
/**
 * @brief Escape sequence for the ASCII bold yellow color
 */
#define COL_BOLD_YELLOW	 "\033[1;33m"
/**
 * @brief Escape sequence for the ASCII blue color
 */
#define COL_BLUE	 "\033[0;34m"
/**
 * @brief Escape sequence for the ASCII bold blue color
 */
#define COL_BOLD_BLUE	 "\033[1;34m"
/**
 * @brief Escape sequence for the ASCII magenta color
 */
#define COL_MAGENTA	 "\033[0;35m"
/**
 * @brief Escape sequence for the ASCII bold magenta color
 */
#define COL_BOLD_MAGENTA "\033[1;35m"
/**
 * @brief Escape sequence for the ASCII cyan color
 */
#define COL_CYAN	 "\033[0;36m"
/**
 * @brief Escape sequence for the ASCII bold cyan color
 */
#define COL_BOLD_CYAN	 "\033[1;36m"
/**
 * @brief Escape sequence for the ASCII reset sequence
 */
#define COL_RESET	 "\033[0m"

/**
 * @brief Sends the sequence for the color red through standard output
 */
void col_red();
/**
 * @brief Sends the sequence for the bold color red through standard output
 */
void col_bold_red();

/**
 * @brief Sends the sequence for the color green through standard output
 */
void col_green();
/**
 * @brief Sends the sequence for the bold color green through standard output
 */
void col_bold_green();

/**
 * @brief Sends the sequence for the color yellow through standard output
 */
void col_yellow();
/**
 * @brief Sends the sequence for the bold color yellow through standard output
 */
void col_bold_yellow();

/**
 * @brief Sends the sequence for the color blue through standard output
 */
void col_blue();
/**
 * @brief Sends the sequence for the blue color bold through standard output
 */
void col_bold_blue();

/**
 * @brief Sends the sequence for the color magenta through standard output
 */
void col_magenta();
/**
 * @brief Sends the sequence for the bold color magenta through standard output
 */
void col_bold_magenta();

/**
 * @brief Sends the sequence for the color cyan through standard output
 */
void col_cyan();
/**
 * @brief Sends the sequence for the bold color cyan through standard output
 */
void col_bold_cyan();

/**
 * @brief Sends the sequence for the ASCII reset sequence through standard output
 */
void col_reset();
#endif

#ifdef CLZ_COLOR_IMPL

#ifndef CLZ_NO_COLOR_FUNCTIONS

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

#endif

