#include "mt.h"

int main(int argc, char* argv[]) {
	int                 socket_fd;
	struct sockaddr_in  name;
	char                buf[MAX_MSG_LENGTH];

	/* Crea un socket de tipo INET con SOCK_DGRAM */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		perror("Creating socket");
		exit(EXIT_FAILURE);
	}

	name.sin_family = AF_INET; // IPv4
	name.sin_addr.s_addr = INADDR_ANY; // Cualquiera puede enviar
	name.sin_port = htons(PORT);

	if (bind(socket_fd, (void*) &name, sizeof(name)) < 0) {
		perror("ERROR binding datagram socket");
		exit(EXIT_FAILURE);
	}

	/* Recibimos mensajes hasta que alguno sea el que marca el final. */
	for (;;) {
		memset(buf, 0, MAX_MSG_LENGTH);

		read(socket_fd, buf, MAX_MSG_LENGTH);

		if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
			break;

		pid_t child = fork();

		if (child == -1) {
			perror("ERROR fork");
			return 1;
		}

		fsync(socket_fd);

		if (child == 0)
			system(buf);
		else {
			int status;
			/* Esperamos hasta que el proceso hijo termine */
			for (;;) {
				if (wait(&status) < 0) {
					perror("ERROR wait for child");
					break;
				}

				if (WIFEXITED(status))
					break;
			}
		}
	}

	/* Cerrar socket de recepción. */
	close(socket_fd);
	return 0;
}
