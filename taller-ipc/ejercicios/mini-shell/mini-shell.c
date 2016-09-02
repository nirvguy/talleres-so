#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* waitpid */
#include <unistd.h>     /* exit, fork */

int run(char *program_name[], char **program_argv[], unsigned int count) {
	int fd[2], oldout;

	for(int i = 0; i < count; i++) {
		if(pipe(fd) < 0) {
			perror("ERROR pipe");
			return 1;
		}

		pid_t child = fork();

		if(child == -1) {
			perror("ERROR fork");
			return 1;
		}

		if(child == 0) {
			close(fd[0]);

			/* Cambia la entrada estandar por
			 * el fd del pipe (extremo de lectura) del pipe
			 * anterior salvo que sea el primer proceso
			 */
			if(i > 0) {
				if(dup2(oldout, 0) < 0) {
					perror("ERROR dup2 in!");
					return 1;
				}
			}

			/* Cambia la salida estandar por
			 * el fd del pipe (extremo de escritura) del pipe
			 * salvo que este sea el ultimo proceso
			 */
			if(i < count - 1) {
				if(dup2(fd[1], 1) < 0) {
					perror("ERROR dup2 out!");
					return 1;
				}
			}

			execvp(program_name[i], program_argv[i]);

			perror("ERROR child execvp(...)!");
			exit(1);
		}

		close(fd[1]);

		oldout = fd[0];
	}

	int status;

	// Espera a que todos los hijos terminen
	for(;;) {
		if (wait(&status) < 0) {
			perror("ERROR wait for child");
			break;
		}

		if (WIFEXITED(status))
			break;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	/* Parsing de "ls -al | wc | awk '{ print $2 }'" */
	char *program_name[] = {
		"/bin/ls",
		"/usr/bin/wc",
		"/usr/bin/awk",
	};

	char *ls_argv[]  = {"ls", "-al", NULL};
	char *wc_argv[]  = {"wc", NULL};
	char *awk_argv[] = {"awk", "{ print $2; }", NULL};

	char **program_argv[] = {
		ls_argv,
		wc_argv,
		awk_argv,
	};

	unsigned int count = 3;

	int status = run(program_name, program_argv, count);

	printf("[+] Status : %d\n", status);

	return 0;
}
