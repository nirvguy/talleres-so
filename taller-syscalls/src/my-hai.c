#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define TIMES 5

int finish=0;

void sigurg_handler(int signum)
{
	static int status=0;
	puts("ya va!");

	status++;
	if(status > TIMES)
		finish=1;
}

void sigint_handler(int signum) {
	finish=1;
}

int main(int argc, char* argv[]) {
	int status;
	pid_t child;

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s commando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	/* Fork en dos procesos */
	child = fork();

	if (child == -1) {
		perror("ERROR fork");
		return 1;
	}

	if (child == 0) {
		/* Solo se ejecuta en el hijo */
		signal(SIGURG, sigurg_handler);

		while (!finish)
			;

		if (kill(getppid(), SIGINT) == -1) {
			perror("No se le pudo enviar SIGINT al proceso padre");
			exit(1);
		}

		execvp(argv[1], argv+1);

		/* Si vuelve de exec() hubo un error */
		perror("ERROR child exec(...)");
		exit(1);
	} else {
		/* Solo se ejecuta en el hijo */
		signal(SIGINT, sigint_handler);

		/* Envia tantas veces la señal SIGURG
		 * al hijo hasta que este le responda SIGINT
		 */
		while (!finish) {
			sleep(1);
			puts("sup!");

			if (kill(child, SIGURG) == -1) {
				perror("ERROR sending SIGURG to child process");
				exit(1);
			}
		}

		/* Espera a que el proceso hijo termine */
		while (1) {
			if (wait(&status) < 0) {
				perror("ERROR wait for child");
				break;
			}
			if (WIFEXITED(status))
				break;
		}
	}
	return 0;
}
