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
	printf("%s:%s\n", name, PORT);
	if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    } else {
		printf("Cool%s\n", result->ai_addr->sa_data);
	}

	// Creating socket file descriptor
	int sfd = 0;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);

		if (sfd == -1) {
			continue;
		} else {
			printf("Sender socket found!%d\n", sfd);
		}
	}
	if (sfd < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	udp->sockfd = sfd;
	printf("in function: %d", udp->sockfd);
	udp->addrinfo = *result;

	/* struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if ( bind(udp.sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket set up!\n");
	} */
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
	printf("Message sent: Code %d - %s\n", n, message);

	return 1;
}

/* void startInternalReceiver(struct internalUDPhandle udp) {
	int n;
	n = recvfrom(udp.sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	buffer[n] = '\0';
	printf("Client : %s\n", buffer);
} */

// Driver code
/* int main() {


	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	buffer[n] = '\0';
	printf("Client : %s\n", buffer);
	sendto(sockfd, (const char *)hello, strlen(hello),
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
			len);
	printf("Hello message sent.\n");

	return 0;
} */
