#ifndef SIMPLE_SHELL_H
#define SIMPLE_SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

char *find_command(char *command);
int handle_builtin(char **args);

#endif /* SIMPLE_SHELL_H */
