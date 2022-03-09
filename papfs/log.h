#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>

FILE *log_open(void);

int log_error(char *str);

void log_print(const char *format, ...);
#endif
