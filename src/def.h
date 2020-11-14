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
 * @file def.h
 * @author Lorenzo Calza
 * @date 13 Nov 2020
 * @brief Header file containing the declarations for several types used throughout the library
 *
 * This header file contains the definitions of many types and type-like constructions (read: `typedef`),
 * including the function types @ref clz_predicate and @ref clz_consumer.
 *
 * **Implementation**
 *
 * There is no implementation section, as this header file contains only type definitions.
 */

#include <stdbool.h>

#ifndef _CLZ_DEF_H
/**
 * @brief Include guard for this file.
 */
#define _CLZ_DEF_H

/**
 * @brief Definition of a predicate function, aka a function which takes any kind of data and
 * returns a @ref bool truth value.
 *
 * This function is supposed to evaluate the argument based on some user-defined criteria, and return
 * a @ref bool value (as in logic predicates).
 */
typedef bool (*clz_predicate) (void *);

/**
 * @brief Definition of a consumer function, aka a function which takes any kind of data and
 * performs a series of operations on it.
 *
 * This function is supposed to process the argument without returning any kind of feedback to the
 * calling routine (hence _consume_).
 */
typedef void (*clz_consumer) (void *);

#endif
