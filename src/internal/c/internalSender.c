#include "internal.h"
// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define PORT	"20001"
#define MAXLINE 1024


int initUDPSender(struct internalUDPhandle *udp) {
	int s;
	const char* name = "object_controller";

	struct addrinfo *result, *rp;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(name, PORT, &hints, &result);

	if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

	// Creating socket file descriptor
	int sfd = 0;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);

		if (sfd == -1) {
			continue;
		} else {
			//printf("Sender socket found!%d\n", sfd);
		}
	}
	if (sfd < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	udp->sockfd = sfd;
	//printf("in function: %d", udp->sockfd);
	udp->addrinfo = *result;

	return 1;
}

int sendMessageToOC(struct internalUDPhandle udp, char * message) {
	int n;

	n = sendto(udp.sockfd, (const char *)message, strlen(message),
		NULL, udp.addrinfo.ai_addr, udp.addrinfo.ai_addrlen);

	if (n < 0) {
        printf("Oh: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
	//printf("Message sent: %d - %s\n", n, message);

	return 1;
}
