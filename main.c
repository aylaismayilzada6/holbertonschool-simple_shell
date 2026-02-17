#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

/**
 * main - Simple UNIX command line interpreter
 *
 * Description: Displays a prompt, reads a command,
 * forks a child process and executes it using execve.
 * Handles EOF (Ctrl+D) and errors.
 *
 * Return: Always 0
 */
int main(void)
{
	char *line;
	size_t len;
	ssize_t nread;
	pid_t pid;
	int status;

	line = NULL;
	len = 0;

	while (1)
	{
		printf("#cisfun$ ");
		fflush(stdout);

		nread = getline(&line, &len, stdin);

		if (nread == -1)
		{
			free(line);
			exit(0);
		}

		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		pid = fork();

		if (pid == 0)
		{
			char *argv[2];

			argv[0] = line;
			argv[1] = NULL;

			if (execve(line, argv, environ) == -1)
			{
				perror("./hsh");
				exit(1);
			}
		}
		else if (pid > 0)
		{
			wait(&status);
		}
		else
		{
			perror("fork");
		}
	}

	return (0);
}


