#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

extern char **environ;

char *find_command(char *command)
{
    char *path_env, *path_copy, *dir, *full_path;
    int len;

    if (command[0] == '/')
    {
        if (access(command, X_OK) == 0)
            return strdup(command);
        return NULL;
    }

    path_env = getenv("PATH");
    if (!path_env)
        return NULL;

    path_copy = strdup(path_env);
    dir = strtok(path_copy, ":");

    while (dir)
    {
        len = strlen(dir) + strlen(command) + 2;
        full_path = malloc(len);
        if (!full_path)
            return NULL;

        snprintf(full_path, len, "%s/%s", dir, command);

        if (access(full_path, X_OK) == 0)
        {
            free(path_copy);
            return full_path;
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

void builtin_env(void)
{
    int i = 0;
    while (environ[i])
    {
        printf("%s\n", environ[i]);
        i++;
    }
}

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    char *args[MAX_ARGS];
    char *token, *cmd_path;
    pid_t pid;
    int status, i;

    while (1)
    {
        printf(":) ");
        fflush(stdout);

        if (getline(&line, &len, stdin) == -1)
            break;

        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0)
            continue;

        i = 0;
        token = strtok(line, " ");
        while (token && i < MAX_ARGS - 1)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0)
        {
            free(line);
            return 0;
        }

        if (strcmp(args[0], "env") == 0)
        {
            builtin_env();
            continue;
        }

        cmd_path = find_command(args[0]);

        if (!cmd_path)
        {
            printf("./shell: %s: command not found\n", args[0]);
            continue;
        }

        pid = fork();
        if (pid == 0)
        {
            execve(cmd_path, args, environ);
            perror("execve");
            exit(1);
        }
        else
        {
            wait(&status);
        }

        free(cmd_path);
    }

    free(line);
    return 0;
}
