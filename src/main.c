#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>

void drawImage(Display *display, Window window, cairo_surface_t *image, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame);
int main()
{
    Display *display;
    Window window;
    XEvent event;
    int screen;

    display = XOpenDisplay(NULL);
    if(display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    screen = DefaultScreen(display);


    cairo_surface_t *bg = cairo_image_surface_create_from_png("image/bg/bg_stream.png");
    int width = cairo_image_surface_get_width(bg);
    int height = cairo_image_surface_get_height(bg);

    cairo_surface_t *screenSaver = cairo_image_surface_create_from_png("image/bg/bg_stream_screensaver_3.png");

    cairo_surface_t *ame = cairo_image_surface_create_from_png("image/Stream_V2/Ame/A/stream_ame_game/stream_ame_game_000.png");

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    
    XStoreName(display, window, "Webcam");

    XSelectInput(display, window, ExposureMask | KeyPressMask);

    XMapWindow(display, window);

    if(cairo_surface_status(bg) != CAIRO_STATUS_SUCCESS || cairo_surface_status(screenSaver) != CAIRO_STATUS_SUCCESS || cairo_surface_status(ame) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Cannot load image\n");
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
    }

    XCloseDisplay(display);
    return 0;
}
void drawImage(Display *display, Window window, cairo_surface_t *bg, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame)
{
    int screen = DefaultScreen(display);
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_xlib_surface_create(display, window, DefaultVisual(display, screen), width, height);
    cr = cairo_create(surface);

    cairo_set_source_surface(cr, bg, 0, 0);
    cairo_paint(cr);

    cairo_set_source_surface(cr, screenSaver, 0, 0);
    cairo_paint(cr);

    cairo_set_source_surface(cr, ame, 0, 0);
    cairo_paint(cr);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}