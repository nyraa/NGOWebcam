#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "window.h"

void drawImage(Display *display, Window window, cairo_surface_t *image, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame);
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

    XCloseDisplay(display);
    return 0;
}