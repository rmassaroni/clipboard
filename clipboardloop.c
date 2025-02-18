#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>

void get_clipboard_content(Display *display, Window window, Atom clipboard, Atom utf8_string) {
    Atom type;
    int format;
    unsigned long num_items, bytes_after;
    unsigned char *data = NULL;

    XConvertSelection(display, clipboard, utf8_string, clipboard, window, CurrentTime);
    XFlush(display);
    XGetWindowProperty(display, window, clipboard, 0, 1024, False, utf8_string, &type, &format, &num_items, &bytes_after, &data);

    if (data) {
        printf("Clipboard: %s\n", data);
        XFree(data);
    } else {
        printf("No clipboard data found.\n");
    }
}

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        printf("Failed to open X display.\n");
        return 1;
    }

    Window window = XDefaultRootWindow(display);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
    
    unsigned char *previous_clipboard_content = NULL;

    while (1) {
        unsigned char *current_clipboard_content = NULL;
        get_clipboard_content(display, window, clipboard, utf8_string);

        // You may want to compare current_clipboard_content with previous_clipboard_content
        // to detect changes. Here's a simple example (not comparing exact content for brevity):
        if (current_clipboard_content != previous_clipboard_content) {
            printf("Clipboard changed!\n");
            previous_clipboard_content = current_clipboard_content;
        }

        // Sleep for a while before checking again
        sleep(1); // Sleep for 1 second (adjust as needed)
    }

    XCloseDisplay(display);
    return 0;
}

