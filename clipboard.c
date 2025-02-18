#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

void get_clipboard_content() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        printf("Failed to open X display.\n");
        return;
    }

    Window window = XDefaultRootWindow(display);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
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

    XCloseDisplay(display);
}

int main() {
    get_clipboard_content();
    return 0;
}
