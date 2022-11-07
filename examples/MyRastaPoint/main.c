//
// Created by tobia on 24.02.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rasta_new.h>
#include "rasta_new.h"
#include "rmemory.h"
#include <unistd.h>
#include "internal.h"
#include "myRasta.h"

#define CONFIG_PATH "../../../rasta_client.cfg"

#define ID_R 0x61
#define ID_S1 0x62
#define ID_S2 0x63

struct internalUDPhandle udpSender;
struct internalUDPhandle udpReceiver;

void onConnectionStateChange(struct rasta_notification_result *result) {
    onConnectionStateChangeProxy(result, udpReceiver, udpSender);
}
void onHandshakeCompleted(struct rasta_notification_result *result) {
    onHandshakeCompletedProxy(result);
}
void onTimeout(struct rasta_notification_result *result) {
    onTimeoutProxy(result);
}
void onReceive(struct rasta_notification_result *result) {
    onReceiveProxy(result, udpSender);
}

int main(){
    initUDPReceiver(&udpReceiver);
    initUDPSender(&udpSender);

    struct rasta_handle h;

    sr_init_handle(&h, CONFIG_PATH);

    struct RastaIPData *thisServer = &h.config.values.redundancy.connections.data[0];
    printf("Server at %s:%d\n", thisServer[0].ip, thisServer[0].port);
    printf("->ID = 0x%lX\n", (unsigned long) config_get(&h.config, "RASTA_ID").value.number);

    h.notifications.on_connection_state_change = onConnectionStateChange;
    h.notifications.on_receive = onReceive;
    h.notifications.on_handshake_complete = onHandshakeCompleted;
    h.notifications.on_heartbeat_timeout = onTimeout;

    startInternalReceiver(udpReceiver, udpSender, &h);
    logger_log(&h.logger,LOG_LEVEL_DEBUG, "Startup", "All notification handlers initiated\n");

    char *message;
    asprintf(&message, "boot;%lX", (unsigned long) config_get(&h.config, "RASTA_ID").value.number);
    int s;
    struct addrinfo *result;
    const char* name = "10.0.2.2";
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(name, "20000", &hints, &result);
    printf("Sending to %c:%d", result->ai_addr);
    sendto(udpSender.sockfd, (const char *)message, strlen(message),
		NULL, result->ai_addr, result->ai_addrlen);

    pause();
    printf("Starting clean up\n");
    sr_cleanup(&h);

    return 0;
}

