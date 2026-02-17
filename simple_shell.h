#ifndef SIMPLE_SHELL
#define SIMPLE_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int execute(char *const command[], char **envp);
int print_env(void);
char *_getenv(const char *name, char **envp);
void parse(char command[], char **envp);

#endif /* SIMPLE_SHELL */
