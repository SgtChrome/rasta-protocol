#include <rasta_new.h>
#include "rasta_new.h"

void addRastaString(struct RastaMessageData * data, int pos, char * str);

void onConnectionStateChangeProxy(struct rasta_notification_result *result, struct internalUDPhandle udpReceiver, struct internalUDPhandle udpSender);
void onHandshakeCompletedProxy(struct rasta_notification_result *result);
void onTimeoutProxy(struct rasta_notification_result *result);
void onReceiveProxy(struct rasta_notification_result *result, struct internalUDPhandle udpSender);

struct RastaIPData * getServerDataFromConfig(struct rastaConnection *connection);

typedef enum {
    /**
     * Request restart
     */
            REQUEST_RECONNECT
} message_from_oc;


