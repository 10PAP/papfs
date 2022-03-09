#include "params.h"
#include "log.h"

#include <fuse.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int log_error(char *str) {
    int ret = -errno;
    log_print("    ERROR %s: %s\n", str, strerror(errno));
    return ret;
}

FILE * log_open() {
    FILE * logfile;
    
    logfile = fopen("papfs.log", "w");
    if (logfile == NULL) {
	    perror("logfile");
	    exit(EXIT_FAILURE);
    }
    // set logfile to line buffering
    setvbuf(logfile, NULL, _IOLBF, 0);
    return logfile;
}

void log_print(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(PAPFS_DATA->logfile, format, ap);
}