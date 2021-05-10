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
 * @file clz.h
 * @author Lorenzo Calza
 * @date 8 May 2021
 * @brief Header file containing the declarations for general constants concerning the library
 *
 * This header file contains the definitions of general functions such as library version and implementation constants
 * such as @ref CLZ_NOT_FOUND which are used across the implementations of several data structures such as
 * @ref dynarray.
 *
 */

#ifndef _CLZ_H
#define _CLZ_H

#define CLZ_VERSION 2

#define CLZ_FIND_INDEX_START -1
#define CLZ_NOT_FOUND -1

#endif
