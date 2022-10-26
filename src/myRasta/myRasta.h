#include <rasta_new.h>
#include "rasta_new.h"

void addRastaString(struct RastaMessageData * data, int pos, char * str);

void onConnectionStateChangeProxy(struct rasta_notification_result *result, struct internalUDPhandle udpReceiver, struct internalUDPhandle udpSender);
void onHandshakeCompletedProxy(struct rasta_notification_result *result);
void onTimeoutProxy(struct rasta_notification_result *result);
void onReceiveProxy(struct rasta_notification_result *result, struct internalUDPhandle udpSender);

struct RastaIPData * getServerDataFromConfig(struct rastaConnection *connection);

void sendRastaMessage(struct rasta_handle *h, unsigned long remote_id, char *message, int bufferlength);
void packMyRaSTAMessage(char *output, int internal, unsigned long rastaSender, unsigned long rastaReceiver, char *message);

typedef enum {
    /**
     * Request restart
     */
            REQUEST_RECONNECT,
    /**
     * Request list of connections
     */
            REQUEST_CONNECTIONLIST,
} message_from_oc;


