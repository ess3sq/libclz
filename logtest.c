//
// Created by lorenzo on 09/05/2021.
//

#define CLZ_LOGGER_IMPL
#include "src/logger.h"
int main() {
    logger *log = logger_default();
    logger_log(log, LOG_INFO, "Hello World!");
    logger_logf(log, LOG_INFO, "%s, %s!", "Greetings", "Friend");
}