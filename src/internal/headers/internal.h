#include "rastahandle.h"
#include <netdb.h>
#include "config.h"

struct rastaConnection {
    struct RastaIPData ipdata;
    unsigned long rastaID;
    int connectionUp;
};

struct internalUDPhandle {
    int sockfd;
    struct rastaConnection *connections;
    struct addrinfo addrinfo;
    int connectionsCount;
};

int initUDPReceiver(struct internalUDPhandle*);
int initUDPSender(struct internalUDPhandle*);

int sendMessageToOC(struct internalUDPhandle, char * message);
int startInternalReceiver(struct internalUDPhandle, struct internalUDPhandle, struct rasta_handle);

/* typedef void(*on__internal_receive_ptr)(char *result);

struct internalConfig {
    on_internal_receive_ptr onReceive;
}; */