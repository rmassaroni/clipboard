#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>

#define EXAMPLE_SERVER_PORT 9000

// This will handle WebSocket events like connection, message reception, etc.
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason,
                              void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:  // Client connected
            printf("Client connected\n");
            break;
        case LWS_CALLBACK_RECEIVE:  // Message received from client
            printf("Received message: %s\n", (char *)in);
            // Echo the received message back to the client
            lws_write(wsi, in, len, LWS_WRITE_TEXT);
            break;
        case LWS_CALLBACK_CLOSED:  // Client disconnected
            printf("Client disconnected\n");
            break;
        default:
            break;
    }
    return 0;
}

// WebSocket protocol definition
static struct lws_protocols protocols[] = {
    {
        "example-protocol",   // Name of the protocol
        callback_websocket,   // Callback function to handle events
        0,                    // Per-session user data size
    },
    { NULL, NULL, 0, 0 }  // End of list
};

int main() {
    struct lws_context_creation_info info;
    struct lws_context *context;

    // Initialize the context creation info
    memset(&info, 0, sizeof(info));
    info.port = EXAMPLE_SERVER_PORT;           // Port number to listen on
    info.protocols = protocols;                // Protocols array
    info.gid = -1;
    info.uid = -1;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;  // Optional SSL support

    // Create the WebSocket server context
    context = lws_create_context(&info);
    if (context == NULL) {
        printf("libwebsocket init failed\n");
        return -1;
    }

    printf("WebSocket server started on port %d\n", EXAMPLE_SERVER_PORT);

    // Main server loop
    while (1) {
        lws_service(context, 100);  // Process WebSocket events with 100ms timeout
    }

    // Cleanup
    lws_context_destroy(context);
    return 0;
}

