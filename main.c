#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * main - Simple UNIX shell
 *
 * Description: Displays a prompt, reads commands,
 * executes them using execve.
 *
 * Return: Always 0
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int status;

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
			char *argv[] = {line, NULL};

			if (execve(line, argv, NULL) == -1)
			{
				perror("./shell");
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

