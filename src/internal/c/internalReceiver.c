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
#include "myRasta.h"

#define PORT	20002
#define MAXLINE 1024
#define ID_S1 0x62

typedef struct {
    struct internalUDPhandle udpReceiver;
    struct internalUDPhandle udpSender;
    struct rasta_handle * handle;
} passHack;


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

void informOCOfConnections() {

}

void sendMessage(struct rasta_handle *h, unsigned long remote_id, char *message) {
	struct RastaMessageData messageData1;
	allocateRastaMessageData(&messageData1, 1);
	addRastaString(&messageData1,0,(char*) message);

	sr_send(h, remote_id, messageData1);
	printf("Rasta message sent to %lX: %s", remote_id, message);
}

void *receiveMessages(void *pH) {
	int n;
	char buffer[MAXLINE];
	passHack *actualHandlers = pH;

	while (1) {
		n = recvfrom(actualHandlers->udpReceiver.sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, NULL, NULL);
		if (n < 0) {
			printf("Oh: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		buffer[n] = '\0';
		printf("Internal message arrived: %d - %s\n", n, buffer);
	}
	//buffer
	// don't know if this actually works
	/* if (actualHandlers->udp.rastaConnection == 1) {
		sendMessage(actualHandlers->handle, buffer);
	} else {
		printf("No Rasta connection available\n");
	} */

	// Messages structure:
	// 0/1 Message/Internal; RastaID; message
    int i = 0;
    char *t = strtok (buffer, ";");
    char *array[3];

    while (t != NULL)
    {
        array[i++] = t;
        t = strtok (NULL, ";");
    }

    for (i = 0; i < 3; ++i) {
		printf("%s\n", array[i]);
	}

	unsigned long rastaid = strtoul(array[1], NULL, 0);
	printf("%lX", rastaid);

    if (array[0] == 0) {
		// send message to rasta client
		printf("Message kind: %d", array[0]);
		sendMessage(actualHandlers->handle, rastaid, array[2]);
	} else {
        // convert integer: in ASCII code, the numbers (digits) start from 48
        switch ((int)*array[2] - 48)
        {
        case REQUEST_RECONNECT:
            for (unsigned int i = 0; i < actualHandlers->udpReceiver.connectionsCount; i++) {
                if (actualHandlers->udpReceiver.connections[i].rastaID == rastaid) {
					if (actualHandlers->udpReceiver.connections[i].connectionUp == 0) {
						printf("Reconnect request received for %lX\n", rastaid);
						sr_connect(actualHandlers->handle, (unsigned long) array[1], getServerDataFromConfig(&actualHandlers->udpReceiver.connections[i]));
					} else {
						printf("Connection to %lX appears to be established already\n", rastaid);
					}
					break;
				}
            }
            break;
		case REQUEST_CONNECTIONLIST:
			for (unsigned int i = 0; i < actualHandlers->udpReceiver.connectionsCount; i++) {
				struct RastaIPData *data = &actualHandlers->udpReceiver.connections[i].ipdata;
				char *message;
				asprintf(&message, "%lX, %d, %c",
						actualHandlers->udpReceiver.connections[i].rastaID,
						actualHandlers->udpReceiver.connections[i].connectionUp,
						data[0].ip);
				sendMessageToOC(actualHandlers->udpSender, message);
			}
			break;

        default:
            break;
        }
    }
}

int startInternalReceiver(struct internalUDPhandle udpReceiver, struct internalUDPhandle udpSender, struct rasta_handle handle) {
	pthread_t caller_thread;

	passHack *pH = malloc(sizeof *pH);

	//create container
    pH->udpReceiver = udpReceiver;
	pH->udpSender = udpSender;
    pH->handle = &handle;

	pthread_create(&caller_thread, NULL, receiveMessages, pH);

	return 1;
}
