#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rasta_new.h>
#include "rasta_new.h"
#include "rmemory.h"
#include <unistd.h>
#include "internal.h"

void addRastaString(struct RastaMessageData * data, int pos, char * str) {
    int size =  strlen(str) + 1;

    struct RastaByteArray msg;
    allocateRastaByteArray(&msg, size);
    rmemcpy(msg.bytes, str, size);

    data->data_array[pos] = msg;
}

void onConnectionStateChangeProxy(struct rasta_notification_result *result, struct internalUDPhandle udpReceiver) {
    printf("\n Connectionstate change (remote: %lu)", result->connection.remote_id);

    switch (result->connection.current_state) {
        case RASTA_CONNECTION_CLOSED:
            printf("\nCONNECTION_CLOSED \n\n");
            break;
        case RASTA_CONNECTION_START:
            printf("\nCONNECTION_START \n\n");
            break;
        case RASTA_CONNECTION_DOWN:
            printf("\nCONNECTION_DOWN \n\n");
            break;
        case RASTA_CONNECTION_UP:
            printf("\nCONNECTION_UP \n\n");
            // Initialize listener
            startInternalReceiver(udpReceiver, *result->handle);
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