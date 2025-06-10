#ifndef BIRPLOT_FILE_H
#define BIRPLOT_FILE_H

#include <stddef.h>
#include <stdio.h>

void watch_file(const char *filename, void (*callback)(const char *));

// size is optional, contents will be null terminated.
void read_file(const char *filename, char **contents, size_t *size);
void read_until_char(char *store, char till, FILE *stream);

#endif
