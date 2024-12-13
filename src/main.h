#ifndef MAIN_H
#define MAIN_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int command_init(void);
int command_cat_file(int, char **);
#endif // !MAIN_H
