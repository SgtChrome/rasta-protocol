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


int client1 = 1;
int client2 = 1;

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
    struct RastaIPData toServer[2];

    strcpy(toServer[0].ip, "10.152.2.20");
    strcpy(toServer[1].ip, "10.152.2.20");

    toServer[0].port = 8888;
    toServer[1].port = 8889;

    printf("Server at %s:%d and %s:%d\n", toServer[0].ip, toServer[0].port, toServer[1].ip, toServer[1].port);

    printf("->   S1 (ID = 0x%lX)\n", (unsigned long)ID_S1);

    sr_init_handle(&h, CONFIG_PATH);
    h.notifications.on_connection_state_change = onConnectionStateChange;
    h.notifications.on_receive = onReceive;
    h.notifications.on_handshake_complete = onHandshakeCompleted;
    h.notifications.on_heartbeat_timeout = onTimeout;
    /* printf("->   Press Enter to connect\n");
    getchar(); */
    printf("All notification handlers initiated\n");

    startInternalReceiver(udpReceiver, udpSender, &h);
    printf("Internal Receiver started\n");

    pause();
    printf("test\n");
    getchar();
    printf("Starting clean up\n");
    sr_cleanup(&h);

    return 0;
}

