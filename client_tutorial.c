#include <libwebsockets.h>
#include <stdio.h>
#include <string.h>

#define EXAMPLE_SERVER_ADDR "localhost"
#define EXAMPLE_SERVER_PORT 9000

static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason,
                              void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:  // Connection established
            printf("Connected to server\n");
            // Send a message after connection is established
            const char *msg = "Hello, WebSocket Server!";
            lws_write(wsi, (unsigned char *)msg, strlen(msg), LWS_WRITE_TEXT);
            break;
        case LWS_CALLBACK_RECEIVE:  // Message received from server
            printf("Received message from server: %s\n", (char *)in);
            break;
        default:
            break;
    }
    return 0;
}

// WebSocket protocol definition
static struct lws_protocols protocols[] = {
    {
        "example-protocol",   // Protocol name
        callback_websocket,   // Callback function for events
        0,                    // Per-session user data size
    },
    { NULL, NULL, 0, 0 }  // End of list
};

int main() {
    struct lws_context_creation_info info;
    struct lws_context *context;
    struct lws *wsi;

    // Initialize the context creation info
    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN;    // We don't need to listen here
    info.protocols = protocols;            // Protocols array
    info.gid = -1;
    info.uid = -1;

    // Create the WebSocket context
    context = lws_create_context(&info);
    if (context == NULL) {
        printf("libwebsocket init failed\n");
        return -1;
    }

    // Create WebSocket client connection
    struct lws_client_connect_info i = {0};
    i.context = context;
    i.address = EXAMPLE_SERVER_ADDR;
    i.port = EXAMPLE_SERVER_PORT;
    i.path = "/";
    i.protocol = protocols[0].name;
    i.ssl_connection = 0;

    wsi = lws_client_connect_via_info(&i);
    if (wsi == NULL) {
        printf("Failed to connect to server\n");
        lws_context_destroy(context);
        return -1;
    }

    // Main event loop
    while (1) {
        lws_service(context, 100);  // Process WebSocket events with 100ms timeout
    }

    // Cleanup
    lws_context_destroy(context);
    return 0;
}

