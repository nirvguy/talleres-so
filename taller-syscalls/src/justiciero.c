#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __x86_64__
#  define SYSCALL_NR_OFFSET (ORIG_RAX * 8)
#else
#  define SYSCALL_NR_OFFSET (ORIG_EAX * 4)
#endif


int main(int argc, char* argv[]) {
	int status;
	pid_t child;

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s commando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	child = fork();

	if (child == -1) {
		perror("ERROR fork");
		return 1;
	}

	if (child == 0) {
		/* Solo se ejecuta en el hijo */
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
			perror("ERROR child ptrace(PTRACE_TRACEME,...)");
			return 1;
		}
		raise(SIGSTOP);

		execvp(argv[1], argv+1);

		/* Si vuelve de exec() hubo un error */
		perror("ERROR child exec(...)");
		exit(1);
	} else {
		/* Solo se ejecuta en el padre */
		while (1) {
			/* Resume hasta el próximo syscall del hijo */
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);

			if (wait(&status) < 0) {
				perror("ERROR wait for child");
				break;
			}

			/* Si el proceso hijo termino
			 * el padre tambien termina
			 */
			if (WIFEXITED(status))
				break;

			int sysno = ptrace(PTRACE_PEEKUSER, child, SYSCALL_NR_OFFSET, NULL);

			/* En caso de que la syscall (del hijo) sea kill
			 * hace justicia matando al proceso
			 */
			if (sysno == SYS_kill) {
				ptrace(PTRACE_KILL, child, 0, 0);
				printf("Se ha hecho justicia!\n");
				break;
			}
		}
		ptrace(PTRACE_DETACH, child, NULL, NULL);
	}
	return 0;
}
