#include "mt.h"

int main(int argc, char* argv[]) {
	int                 socket_fd;
	struct sockaddr_in  remote;
	struct hostent*     hp;
	char                buf[MAX_MSG_LENGTH];

	/* Crea un socket de tipo INET con SOCK_DGRAM */
	if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("ERROR socket");
		exit(1);
	}

	hp = gethostbyname(argv[1]);

	memset((char*) &remote, 0, sizeof(remote));

	remote.sin_family = AF_INET;     // IPv4
	remote.sin_port   = htons(PORT);
	memcpy((void *) &remote.sin_addr, hp->h_addr_list[0], hp->h_length);

	while (printf("> "), fgets(buf, MAX_MSG_LENGTH, stdin), !feof(stdin)) {
		if (sendto(socket_fd,
			         buf, strlen(buf),
			         0,
			         (struct sockaddr *) &remote, sizeof(remote)) < 0) {
			perror("ERROR sendto");
			exit(1);
		}
	}

	/* Cerrar el socket. */
	close(socket_fd);
	return 0;
}
