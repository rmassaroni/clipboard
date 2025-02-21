#include <libwebsockets.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 8080

static unsigned char *previous_clipboard_content = NULL;
static struct lws *websocket_wsi = NULL;
static int clipboard_update_interval = 500;

void get_clipboard_content(Display *display, Window window, Atom clipboard, Atom utf8_string, int *last_checked_time) {
    int current_time = (int)time(NULL) * 1000;  // Get current time in milliseconds
    if (current_time - *last_checked_time >= clipboard_update_interval) {
        *last_checked_time = current_time;

    Atom type;
    int format;
    unsigned long num_items, bytes_after;
    unsigned char *data = NULL;

    XConvertSelection(display, clipboard, utf8_string, clipboard, window, CurrentTime);
    XFlush(display);
    XGetWindowProperty(display, window, clipboard, 0, 1024, False, utf8_string, &type, &format, &num_items, &bytes_after, &data);

    if (data) {
        // Check if the clipboard content has changed
            // printf("%s\n", data);
        if (previous_clipboard_content == NULL || strcmp((char *)previous_clipboard_content, (char *)data) != 0) {
            printf("Clipboard changed: %s\n", data);
            if (websocket_wsi) {
                unsigned char buf[LWS_PRE + 512];
                snprintf((char *)buf + LWS_PRE, sizeof(buf) - LWS_PRE, "Clipboard: %s", data);
                int n = lws_write(websocket_wsi, buf + LWS_PRE, strlen((char *)buf + LWS_PRE), LWS_WRITE_TEXT);
                if (n < 0) {
                    perror("Error writing to WebSocket");
                }
            }

            // Update previous clipboard content
            if (previous_clipboard_content) {
                free(previous_clipboard_content);
            }
            previous_clipboard_content = strdup((char *)data);  // Save the new clipboard content
        }
        XFree(data);
    } else {
        printf("No clipboard data found.\n");
    }
    }
}

static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason,
                              void *user, void *in, size_t len) {
    unsigned char buf[LWS_PRE + 512];
    int n;
    // printf("switch\n");

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("Connection established\n");
            websocket_wsi = wsi;
            break;

        case LWS_CALLBACK_RECEIVE:
            printf("Received message: %s\n", (char *)in);

            // Handle client message (for this example, we don't do anything specific with it)
            snprintf((char *)buf + LWS_PRE, sizeof(buf) - LWS_PRE, "Clipboard updated");
            n = lws_write(wsi, buf + LWS_PRE, strlen((char *)buf + LWS_PRE), LWS_WRITE_TEXT);
            if (n < 0) {
                perror("Error writing to WebSocket");
                return -1;
            }
            break;

        case LWS_CALLBACK_CLOSED:
            printf("Connection closed\n");
            websocket_wsi = NULL;
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            // snprintf((char *)buf + LWS_PRE, sizeof(buf) - LWS_PRE, "Sending data to client");
            // n = lws_write(wsi, buf + LWS_PRE, strlen((char *)buf + LWS_PRE), LWS_WRITE_TEXT);
            // if (n < 0) {
            //     perror("Error writing to WebSocket");
            //     return -1;
            // }
            break;

        default:
            // printf("callback_websocket: unhandled event %d\n", reason);
            break;
    }

    return 0;
}


static struct lws_protocols protocols[] = {
    {
        "example-protocol",   // Name of the protocol
        callback_websocket,   // Callback function to handle events
        0,                    // Per-session user data size
    },
    { NULL, NULL, 0, 0 }  // End of list
};
void* clipboard_thread(void* arg) {
    struct lws_context_creation_info info;
    struct lws_context *context;

    // Initialize the context creation info
    memset(&info, 0, sizeof(info));
    info.port = PORT;           // Port number to listen on
    info.protocols = protocols;                // Protocols array
    info.gid = -1;
    info.uid = -1;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;  // Optional SSL support

    // Create the WebSocket server context
    context = lws_create_context(&info);
    if (context == NULL) {
        printf("libwebsocket init failed\n");
        // return -1;
    }
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        printf("Failed to open X display.\n");
        lws_context_destroy(context);
        // return 1;
    }

    Window window = XDefaultRootWindow(display);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);

    int last_checked_time = (int)time(NULL) * 1000;

    while (1) {
        get_clipboard_content(display, window, clipboard, utf8_string, &last_checked_time);
        usleep(100000);  // Sleep for 100ms between checks
    }
    return NULL;
    // Cleanup
    XCloseDisplay(display);
    lws_context_destroy(context);
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, clipboard_thread, NULL);
    pthread_detach(thread);


    struct lws_context_creation_info info;
    struct lws_context *context;

    // Initialize the context creation info
    memset(&info, 0, sizeof(info));
    info.port = PORT;           // Port number to listen on
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

    printf("WebSocket server started on port %d\n", PORT);

    // Display *display = XOpenDisplay(NULL);
    // if (!display) {
    //     printf("Failed to open X display.\n");
    //     lws_context_destroy(context);
    //     return 1;
    // }
    //
    // Window window = XDefaultRootWindow(display);
    // Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    // Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);

    int last_checked_time = (int)time(NULL) * 1000;

    // Main server loop
    while (1) {
        // printf("lws_service\n");
        lws_service(context, 10);  // Process WebSocket events with 100ms timeout
        // printf("get_clipboard_content\n");
        // get_clipboard_content(display, window, clipboard, utf8_string);
        // printf("sleep\n");
        // sleep(1);
        // printf("unsleep\n");
        // // usleep(100000);
    }

    // // Cleanup
    // XCloseDisplay(display);
    // lws_context_destroy(context);
    return 0;
}
