#define CLZ_LOGGER_IMPL
#include "src/logger.h"

int main(int argc, char **argv) {
    logger *log = logger_new(stdout, false, false, "server/worker-1");
    logger_log(log, LOG_ERROR, "This is a test run. This is a veeeeeeeeeeeeeeeeeeeeeeeeery loooooooooooooong line.");
    logger_log(log, LOG_ERROR, "This is a test run. This is a veeeeeeeeeeeeeeeeeeeeeeeeery loooooooooooooong line.");
    logger_free(log, true);
    return 0;
}