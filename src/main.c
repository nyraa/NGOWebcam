#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>

void drawImage(Display *display, Window window, cairo_surface_t *image);
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

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 500, 700, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    XMapWindow(display, window);

    cairo_surface_t *image = cairo_image_surface_create_from_png("image.png");
    if(cairo_surface_status(image) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Cannot load image\n");
        exit(1);
    }

    while(1)
    {
        XNextEvent(display, &event);

        if(event.type == Expose)
        {
            drawImage(display, window, image);
        }

        if(event.type == KeyPress)
        {
            break;
        }
    }

    XCloseDisplay(display);
    return 0;
}
void drawImage(Display *display, Window window, cairo_surface_t *image)
{
    int screen = DefaultScreen(display);
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_xlib_surface_create(display, window, DefaultVisual(display, screen), 640, 480);
    cr = cairo_create(surface);

    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}