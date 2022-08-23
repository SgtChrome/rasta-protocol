#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rasta_new.h>
#include "rasta_new.h"
#include "rmemory.h"
#include <unistd.h>
#include "internal.h"
#include "myRasta.h"

void addRastaString(struct RastaMessageData * data, int pos, char * str) {
    int size =  strlen(str) + 1;

    struct RastaByteArray msg;
    allocateRastaByteArray(&msg, size);
    rmemcpy(msg.bytes, str, size);

    data->data_array[pos] = msg;
}

struct RastaIPData * getServerDataFromConfig(struct rastaConnection *connection) {
    // write the collected ip addresses to new RastaIPData struct
    // because it doesn't work if put in sr_connect right away
    struct RastaIPData *serverData = rmalloc(sizeof(struct RastaIPData) *2);

    strcpy(serverData[0].ip, connection->ipdata.ip);
    strcpy(serverData[1].ip, connection->ipdata.ip);

    // assumes the redundancy ports are always 8888 and 8889
    serverData[0].port = 8888;
    serverData[1].port = 8889;

    return serverData;
}


void setConnection(int upOrDown, struct rasta_notification_result *result, struct internalUDPhandle udpReceiver) {
    for (unsigned int i = 0; i < udpReceiver.connectionsCount; i++) {
        if (udpReceiver.connections[i].rastaID == result->connection.my_id) {
            udpReceiver.connections[i].connectionUp = upOrDown;
            break;
        }
    }
}

void notifyOCAboutConnectionChange(struct rasta_notification_result *result, struct internalUDPhandle udpSender) {

}

void onConnectionStateChangeProxy(struct rasta_notification_result *result, struct internalUDPhandle udpReceiver, struct internalUDPhandle udpSender) {
    printf("\n Connectionstate change (remote: %d)", result->connection.remote_id);
    char *message;

    switch (result->connection.current_state) {
        case RASTA_CONNECTION_CLOSED:
            printf("\nCONNECTION_CLOSED \n\n");
            setConnection(0, result, udpReceiver);
            asprintf(&message, "1;%d;%d", result->connection.my_id, RASTA_CONNECTION_CLOSED);
            sendMessageToOC(udpSender, message);
            break;
        case RASTA_CONNECTION_START:
            printf("\nCONNECTION_START \n\n");
            break;
        case RASTA_CONNECTION_DOWN:
            printf("\nCONNECTION_DOWN \n\n");
            setConnection(0, result, udpReceiver);
            asprintf(&message, "1;%d;%d", result->connection.my_id, RASTA_CONNECTION_DOWN);
            sendMessageToOC(udpSender, message);
            break;
        case RASTA_CONNECTION_UP:
            printf("\nCONNECTION_UP \n\n");
            setConnection(1, result, udpReceiver);
            asprintf(&message, "1;%d;%d", result->connection.my_id, RASTA_CONNECTION_UP);
            sendMessageToOC(udpSender, message);
            // use if message source is important
            // if (result->connection.my_id == ID_S1) { //Client 1
            break;
        case RASTA_CONNECTION_RETRREQ:
            printf("\nCONNECTION_RETRREQ \n\n");
            break;
        case RASTA_CONNECTION_RETRRUN:
            printf("\nCONNECTION_RETRRUN \n\n");
            break;
        //case default:
    }
    free(message);
}

void onHandshakeCompletedProxy(struct rasta_notification_result *result){
    printf("Handshake complete, state is now UP (with ID 0x%lX)\n", result->connection.remote_id);
}

void onTimeoutProxy(struct rasta_notification_result *result){
    printf("Entity 0x%lX had a heartbeat timeout!\n", result->connection.remote_id);
}

void onReceiveProxy(struct rasta_notification_result *result, struct internalUDPhandle udpSender) {
    rastaApplicationMessage p;

    printf("Received data from Client %lu\n", result->connection.remote_id);

    p = sr_get_received_data(result->handle,&result->connection);

    printf("Packet is from %lu\n", p.id);
    printf("Msg: %s\n", p.appMessage.bytes);

    printf("Forwarding to OC\n");

    sendMessageToOC(udpSender, p.appMessage.bytes);

}