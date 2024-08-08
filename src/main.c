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
    XEvent event;


    cairo_surface_t *bg = cairo_image_surface_create_from_png("image/bg/bg_stream.png");
    int width = cairo_image_surface_get_width(bg);
    int height = cairo_image_surface_get_height(bg);

    cairo_surface_t *screenSaver = cairo_image_surface_create_from_png("image/bg/bg_stream_screensaver_3.png");

    cairo_surface_t *ame = cairo_image_surface_create_from_png("image/Stream_V2/Ame/A/stream_ame_game/stream_ame_game_000.png");

    if(cairo_surface_status(bg) != CAIRO_STATUS_SUCCESS || cairo_surface_status(screenSaver) != CAIRO_STATUS_SUCCESS || cairo_surface_status(ame) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Cannot load image\n");
        exit(1);
    }

    if(init(&display, &window, width, height) != 0)
    {
        exit(1);
    }
    
    while(1)
    {
        XNextEvent(display, &event);

        if(event.type == Expose)
        {
            drawImage(display, window, bg, width, height, screenSaver, ame);
        }

        if(event.type == KeyPress)
        {
            break;
        }
        if(event.type == DestroyNotify)
        {
            break;
        }
    }

    XCloseDisplay(display);
    return 0;
}