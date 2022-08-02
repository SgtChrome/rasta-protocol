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
#include <rasta_new.h>
#include "rasta_new.h"
#include <errno.h>

#define PORT	20002
#define MAXLINE 1024
#define ID_S1 0x62

typedef struct {
    struct internalUDPhandle udp;
    struct rasta_handle * handle;
} passHack;

void addRastaString(struct RastaMessageData * data, int pos, char * str) {
    int size =  strlen(str) + 1;

    struct RastaByteArray msg ;
    allocateRastaByteArray(&msg, size);
    rmemcpy(msg.bytes, str, size);

    data->data_array[pos] = msg;
}

int initUDPReceiver(struct internalUDPhandle *udp) {
	// Creating socket file descriptor
	if ( (udp->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if ( bind(udp->sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket set up!\n");
	}
	return 1;
}

void *receiveMessages(void *pH) {
	int len, n;
		char buffer[MAXLINE];
		passHack *actualHandlers = pH;

	while (1) {
		n = recvfrom(actualHandlers->udp.sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, NULL, NULL);
		if (n < 0) {
			printf("Oh: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		buffer[n] = '\0';
		printf("Client: %d - %s\n", n, buffer);

		// temp remote ID
		/* unsigned long remote_id = ID_S1;

		struct RastaMessageData messageData1;
		allocateRastaMessageData(&messageData1, 1);

		addRastaString(&messageData1,0,(char*)buffer);

		sr_send(actualHandlers->handle, ID_S1, messageData1); */
	}
}

int startInternalReceiver(struct internalUDPhandle udp, struct rasta_handle handle) {
	pthread_t caller_thread;

	passHack *pH = malloc(sizeof *pH);

	//create container
    pH->udp = udp;
    pH->handle = &handle;


	pthread_create(&caller_thread, NULL, receiveMessages, pH);
	return 1;
}

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
