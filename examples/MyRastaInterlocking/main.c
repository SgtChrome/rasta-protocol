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
#include "config.h"

#define CONFIG_PATH "../../../rasta_client.cfg"

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
    h.notifications.on_connection_state_change = onConnectionStateChange;
    h.notifications.on_receive = onReceive;
    h.notifications.on_handshake_complete = onHandshakeCompleted;
    h.notifications.on_heartbeat_timeout = onTimeout;
    logger_log(&h.logger,LOG_LEVEL_DEBUG, "Startup", "All notification handlers initiated\n");

    startInternalReceiver(udpReceiver, udpSender, &h);

    struct DictionaryEntry rastaIDsEntry = config_get(&h.config, "RASTA_IDs");
    struct DictionaryEntry rastaIPsEntry = config_get(&h.config, "RASTA_IPs");

    //check validity of config
    if (rastaIPsEntry.value.array.count != rastaIDsEntry.value.array.count * 2) {
        logger_log(&h.logger,LOG_LEVEL_ERROR, "Startup", "Wrong number of ids and ips\n");
        exit(2);
    }

    // read rasta clients this client is supposed to connect to from config
    udpReceiver.connections = malloc(sizeof(struct rastaConnection) * rastaIDsEntry.value.array.count);
    udpReceiver.connectionsCount = rastaIDsEntry.value.array.count;

    if (rastaIPsEntry.type != DICTIONARY_ARRAY || rastaIPsEntry.value.array.count == 0) {
        logger_log(&h.logger,LOG_LEVEL_ERROR, "Startup", "Error in config\n");
        exit(2);
    }
    else {
        struct rastaConnection connection;

        // check valid format
        // loop over id's. Every ID has two IP entries belonging to it
        for (unsigned int i = 0; i < rastaIDsEntry.value.array.count; i++) {
            connection.blueIPdata = extractIPData(rastaIPsEntry.value.array.data[2*i].c, i);
            connection.greyIPdata = extractIPData(rastaIPsEntry.value.array.data[2*i+1].c, i);
            connection.rastaID = (unsigned long) strtoul(rastaIDsEntry.value.array.data[i].c, NULL, 0);
            if (connection.blueIPdata.port == 0 || connection.greyIPdata.port == 0) {
                logger_log(&h.logger,LOG_LEVEL_ERROR, __FILE__, "RASTA_REDUNDANCY_CONNECTIONS may only contain strings in format ip:port or *:port");
                rfree(rastaIPsEntry.value.array.data);
                rastaIPsEntry.value.array.count = 0;
                break;
            }
            udpReceiver.connections[i] = connection;
        }
    }

    for (unsigned int i = 0; i < udpReceiver.connectionsCount; i++) {
        logger_log(&h.logger,LOG_LEVEL_INFO, "Startup", "Client %d is: %s:%d and %s:%d\n",
            i, udpReceiver.connections[i].blueIPdata.ip, udpReceiver.connections[i].blueIPdata.port,
            udpReceiver.connections[i].greyIPdata.ip, udpReceiver.connections[i].greyIPdata.port);
    }

    struct RastaIPData *thisServer = &h.config.values.redundancy.connections.data[0];

    printf("Server at %s:%d\n", thisServer[0].ip, thisServer[0].port);
    printf("->ID = 0x%lX\n", (unsigned long) config_get(&h.config, "RASTA_ID").value.number);

    // Wait for everything to "settle"
    // sleep(2);
    // go through all the collected ip address and try to connect to them

    for (unsigned int i = 0; i < udpReceiver.connectionsCount; i++) {
        struct RastaIPData *test = getServerDataFromConfig(&udpReceiver.connections[i]);
        printf("Trying to connect to Client %d on %s:%d and %s:%d with RastaID %lX\n", i, test[0].ip,test[0].port, test[1].ip, test[1].port, udpReceiver.connections[i].rastaID);
        sr_connect(&h, udpReceiver.connections[i].rastaID, test);
    }
    // Testcode
    // 0/1 Message/Internal; RastaID_Sender; RastaID_Receiver; message

    /*
    sleep(2);
    char *message;
    asprintf(&message, "0;%x;%lX;0-startup", config_get(&h.config, "RASTA_ID").value.number, udpReceiver.connections[0].rastaID);
    sendRastaMessage(&h, udpReceiver.connections[0].rastaID, message);
    free(message); */

    pause();
    printf("Starting clean up\n");
    sr_cleanup(&h);

    return 0;
}

