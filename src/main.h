#ifndef MAIN_H
#define MAIN_H

#include "_zlib.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int command_init(int, char **);
int command_cat_file(int, char **);
int command_hash_object(int, char **);
int is_flag(const char *);
char *get_git_root(void);
char *get_git_object_path(const char *);
char *mk_git_object_path(const char *);
int def(FILE *, FILE *, int);
int inf(FILE *, FILE *);
void zerr(int, FILE *, FILE *);
long copy_bytes(FILE *, FILE *);
char *hash_file(FILE *stream, char *buffer, size_t buffer_size);

#endif // !MAIN_H
