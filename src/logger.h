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
 * This implementation of a logger is meant for console or file logging with a single format, namely
 *
 *     |2021-05-08 20:55:07| [INFO] (My program) Attempting to verify update...
 *     |2021-05-08 20:55:07| [WARN] (My program) Could not connect to the internet to verify update.
 *     |2021-05-08 20:55:07| [DEBUG] (My program) Program setup worker started!
 *     |2021-05-08 20:55:07| [FATAL] (My program) Insufficient permissions to write to /etc/myprogram/etz.properties.
 *
 * It is also possible to remove the date, time or both, as well as the name:
 *
 *     |2021-05-08 20:55:07| [ERROR] (Datetime Logger) This contains both date and time, as well as logger name
 *     |2021-05-08| [INFO] (Date Logger) This contains only the date and logger name
 *     |20:55:07| [WARN] (Time Logger) This contains only the time and logger name
 *     [DEBUG] (Simple Logger) This only contains the logger name
 *     [ERROR] This is just a very ugly message
 *     |2021-05-08 20:55:07| [FATAL] This is a naked logger, aks without name
 *     |2021-05-08 20:55:07| [INFO] (Full Logger) This is a full logger
 *
 *
 * @file logger.h
 * @author Lorenzo Calza
 * @date 8 May 2021
 * @brief Header file containing the implementation of a very basic and simple logging facility
 *
 */

#ifndef _CLZ_LOGGER_H
#define _CLZ_LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "clz.h"

/**
 * @brief Definition of enum representing the severity level of the log entry
 *
 * The convention is to use:
 *  - `INFO` for general log messages
 *  - `DEBUG` for debugging information that does not reach production
 *  - `WARN` to warn for things that could cause potential problems
 *  - `ERROR` for outright errors that the user should fix but that the program could still recover from or at least
 *      close gracefully
 *  - `FATAL` for problems so severe the program could not possibly recover from and has to shutdown forcefully or
 *      even crash
 *
 */
enum logger_severity {
    LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
};

/**
 * @brief Definition of structure representing a logger configuration
 *
 * Refer to the functions listed below and to the description of this header file (@ref logger.h) in order to
 * find out more.
 *
 * If you wish to retain all the standard settings (`date`, `time` as `true` and write to `stdout`, see @ref logger_default.
 *
 * **Notes**
 *
 * **Do not, under any circumstances, modify and values or pointers belonging to member fiends of this struct**.
 * Use the provided API instead!
 *
 * @see logger.h, logger_severity, logger_default, logger_new, logger_log
 */
typedef struct logger {
    /**
     * @brief Flag that indicates whether or not to log the date stamp
     *
     * @see logger.h, logger_log
     */
    bool date;
    /**
     * @brief Flag that indicates whether or not to log the time stamp
     *
     * @see logger.h, logger_log
     */
    bool time;
    /**
     * @brief String that holds the logger name
     *
     * If null, no logger name is shown and the logger appears "naked".
     *
     * @see logger.h, logger_log
     */
    char *prefix;
    /**
     * @brief File or stream to write to
     *
     * @see logger_default
     */
    FILE *out;
} logger;

/**
 * @brief Returns the default logger
 *
 * This function makes use of dynamic allocation (see `malloc` family). As such, it could _fail_
 * if heap allocation fails. In this case, `NULL` is returned.
 *
 * If you need to retrieve the default logger more than once, you can re-use this function because it will not create
 * the logger again, since it is constructed the first time you invoke it, and stored within a `static` variable
 * for future calls.
 *
 * **Notes**
 *
 * **Never** use @ref logger_free on the default logger because a future call will return the freed struct, it will
 * not re-construct it.
 *
 * @return The pointer to @ref logger if dynamic allocation was successful, `NULL` otherwise
 *
 * @see logger_new
 */
logger *logger_default();

/**
 * @brief Constructs a new logger on heap
 *
 * See @ref logger.h for more information on `date`, `time` and `prefix`. If you intend to use `stdout` with datetime
 * logging, consider using @ref logger_default.
 *
 * This function makes use of dynamic allocation (see `malloc` family). As such, it could _fail_
 * if heap allocation fails. In this case, `NULL` is returned.
 *
 * **Notes**
 *
 * Use @ref logger_free on logger instances returned by this function after done using. Also make sure that `out`
 * is a valid file pointer opened with one of the writing modes.
 *
 * @param out The output file pointer
 * @param date Whether or not to log datestamp
 * @param time Whether or not to log timestamp
 * @param prefix The logger name, prefixed to the messages
 * @return The pointer to the logger if successful, `NULL` otherwise
 *
 * @see logger.h, logger, logger_default
 */
logger *logger_new(FILE *out, bool date, bool time, char *prefix);

/**
 * @brief Frees the logger on heap
 *
 * This function is intended to free the dynamically allocated memory returned by @ref logger_new.
 * If `close` is true, the file is closed as well using the standard function `fclose`.
 *
 * **Notes**
 *
 * **Never** use this function on the default logger (see @ref logger_default).
 *
 * @param log The logger
 * @param close Whether or not to close the output file
 *
 * @see logger_new, logger_default
 */
void logger_free(logger *log, bool close);

/**
 * @brief Logs message using the given logger configuration
 *
 * See @ref logger.h and @ref logger_severity for more information on the output format.
 *
 * **Notes**
 *
 * If the file pointer has been closed, the program will crash.
 *
 * @param log The logger
 * @param level The severity level
 * @param line The message to write out
 *
 * @see logger.h, severity, logger_logf
 */
void logger_log(logger *log, enum logger_severity level, char *line);

/**
 * @brief Logs message using the given logger configuration and format string with variable args
 *
 * See @ref logger.h and @ref logger_severity for more information on the output format.
 *
 * Makes internal use of `vfprintf`.
 *
 * **Notes**
 *
 * If the file pointer has been closed, the program will crash.
 *
 * @param log The logger
 * @param level The severity level
 * @param fmt The format string
 * @param ... The format args
 *
 * @see logger.h, severity, logger_log
 */
void logger_logf(logger *log, enum logger_severity level, char *fmt, ...);

/**
 * @brief Shortcut for @ref logger_log with the @ref logger_severity level `LOG_INFO`
 *
 * This is equivalent to calling `logger_log(log, LOG_INFO, line)`
 */
#define logger_info(log, line) logger_log(log, LOG_INFO, line)
/**
 * @brief Shortcut for @ref logger_log with the @ref logger_severity level `LOG_DEBUG`
 *
 * This is equivalent to calling `logger_log(log, LOG_DEBUG, line)`
 */
#define logger_debug(log, line) logger_log(log, LOG_DEBUG, line)
/**
 * @brief Shortcut for @ref logger_log with the @ref logger_severity level `LOG_WARN`
 *
 * This is equivalent to calling `logger_log(log, LOG_WARN, line)`
 */
#define logger_warn(log, line) logger_log(log, LOG_WARN, line)
/**
 * @brief Shortcut for @ref logger_log with the @ref logger_severity level `LOG_ERROR`
 *
 * This is equivalent to calling `logger_log(log, LOG_ERROR, line)`
 */
#define logger_error(log, line) logger_log(log, LOG_ERROR, line)
/**
 * @brief Shortcut for @ref logger_log with the @ref logger_severity level `LOG_FATAL`
 *
 * This is equivalent to calling `logger_log(log, LOG_FATAL, line)`
 */
#define logger_fatal(log, line) logger_log(log, LOG_FATAL, line)

#endif

#ifdef CLZ_LOGGER_IMPL
#undef CLZ_LOGGER_IMPL

logger *logger_default() {
    static logger *def = NULL;
    if (def == NULL) def = logger_new(stdout, true, true, "STDOUT");
    return def;
}

logger *logger_new(FILE *out, bool date, bool time, char *prefix) {
    logger *log = malloc(sizeof(logger));
    if (log == NULL) return NULL;
    log->date = date;
    log->time = time;
    log->out = out;
    if (prefix != NULL) log->prefix = strdup(prefix);
    return log;
}

void logger_free(logger *log, bool close) {
    if (close) fclose(log->out);
    free(log->prefix);
    free(log);
}

char *_logger_sev_level(enum logger_severity level) {
    fflush(stdout);
    switch (level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO: return "INFO";
        case LOG_WARN: return "WARN";
        case LOG_ERROR: return "ERROR";
        case LOG_FATAL: return "FATAL";
        default: return "invalid";
    }
}

void logger_log(logger *log, enum logger_severity level, char *line) {
    // |2020-04-18 14:58:22| [DEBUG] (PREFIX) line-contents\n
    char *datetime = "";
    if (log-> date || log->time) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        datetime = malloc(24);

        char *date = malloc(12);
        if (log->date) {
            sprintf(date, "%04d-%02d-%02d%s", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, log->time ? " " : "");
        }
        else {
            *date = '\0';
        }

        char *time = malloc(9);
        if (log->time) {
            sprintf(time, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        else {
            *time = '\0';
        }

        sprintf(datetime, "|%s%s|", date, time);
        free(date);
        free(time);
    }

    char *prefix = "";
    if (log->prefix != NULL) {
        prefix = malloc(strlen(log->prefix) + 4);
        sprintf(prefix, "(%s) ", log->prefix);
    }

    fprintf(log->out, "%s%s[%s] %s%s\n",
            datetime, strlen(datetime) > 0 ? " " : "", _logger_sev_level(level), prefix, line);
    free(datetime);
    free(prefix);
}

void logger_logf(logger *log, enum logger_severity level, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // |2020-04-18 14:58:22| [DEBUG] (PREFIX) line-contents\n
    char *datetime = "";
    if (log-> date || log->time) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        datetime = malloc(24);

        char *date = malloc(12);
        if (log->date) {
            sprintf(date, "%04d-%02d-%02d%s", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, log->time ? " " : "");
        }
        else {
            *date = '\0';
        }

        char *time = malloc(9);
        if (log->time) {
            sprintf(time, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        else {
            *time = '\0';
        }

        sprintf(datetime, "|%s%s|", date, time);
        free(date);
        free(time);
    }

    char *prefix = "";
    if (log->prefix != NULL) {
        prefix = malloc(strlen(log->prefix) + 4);
        sprintf(prefix, "(%s) ", log->prefix);
    }

    fprintf(log->out, "%s%s[%s] %s",
            datetime, strlen(datetime) > 0 ? " " : "", _logger_sev_level(level), prefix);
    char *line = malloc(strlen(fmt) + 2);
    sprintf(line, "%s\n", fmt);
    vfprintf(log->out, line, args);
    free(line);
    free(datetime);
}

#endif
