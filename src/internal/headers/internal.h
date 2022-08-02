#include "rastahandle.h"
#include <netdb.h>

struct internalUDPhandle {
    int sockfd;
    struct addrinfo addrinfo;
};

int initUDPReceiver(struct internalUDPhandle*);
int initUDPSender(struct internalUDPhandle*);

int sendMessageToOC(struct internalUDPhandle, char * message);
int startInternalReceiver(struct internalUDPhandle, struct rasta_handle);

/* typedef void(*on__internal_receive_ptr)(char *result);

struct internalConfig {
    on_internal_receive_ptr onReceive;
}; */