#include <libwebsockets.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 8080

// WebSocket callback function
static int callback_http(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason,
                         void *user, void *in, size_t len) {

    unsigned char buf[LWS_PRE + 512];
    int n;

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("Connection established\n");
            break;

        case LWS_CALLBACK_RECEIVE:
            printf("Received message: %s\n", (char *)in);

            // Replace this with your clipboard checking logic
            snprintf((char *)buf + LWS_PRE, sizeof(buf) - LWS_PRE, "Clipboard updated");

            n = libwebsocket_write(wsi, buf + LWS_PRE, strlen((char *)buf + LWS_PRE), LWS_WRITE_TEXT);
            if (n < 0) {
                perror("Error writing to WebSocket");
                return -1;
            }
            break;

        case LWS_CALLBACK_CLOSED:
            printf("Connection closed\n");
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            snprintf((char *)buf + LWS_PRE, sizeof(buf) - LWS_PRE, "Sending data to client");
            n = libwebsocket_write(wsi, buf + LWS_PRE, strlen((char *)buf + LWS_PRE), LWS_WRITE_TEXT);
            if (n < 0) {
                perror("Error writing to WebSocket");
                return -1;
            }
            break;

        default:
            break;
    }

    return 0;
}

// Define protocol
static struct libwebsocket_protocols protocols[] = {
    {
        .name = "http-only",
        .callback = callback_http,
        .per_session_data_size = 0,
        .rx_buffer_size = 0,
    },
    { NULL, NULL, 0, 0 }  // Terminator for the array
};

// Main function
int main() {
    struct libwebsocket_context *context;

    // Initialize WebSocket context
    context = libwebsocket_create_context(protocols, NULL, NULL, NULL, PORT);
    if (!context) {
        fprintf(stderr, "libwebsocket_create_context failed\n");
        return -1;
    }

    // Main event loop
    while (1) {
        libwebsocket_service(context, 0);
    }

    // Clean up
    libwebsocket_context_destroy(context);
    return 0;
}

