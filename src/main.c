#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "window.h"

int main()
{
    Display *display;
    Window window;

    if(init(&display, &window) != 0)
    {
        fprintf(stderr, "Failed to initialize window\n");
        exit(1);
    }

    handleWindow(display, window);

    destroy();
    XCloseDisplay(display);
    return 0;
}