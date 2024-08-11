#include "window.h"
#include "stream.h"

#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

animation_t currentAnimation = stream_ame_game_a;
int total_frames = 0;

cairo_surface_t **frames = NULL;
double current_frame_timeout = 0.0;
int current_frame = 0;

cairo_surface_t *bg = NULL;
cairo_surface_t *screenSaver = NULL;

int height;
int width;

int handleWindow(Display *display, Window window)
{
    XEvent event;
    while(1)
    {
        XNextEvent(display, &event);

        if(event.type == Expose)
        {
            drawImage(display, window, bg, width, height, screenSaver, frames[current_frame]);
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

void changeAnimation(animation_t target)
{
    unloadFrames();
    currentAnimation = target;
    total_frames = frame_counts[currentAnimation];
    current_frame = 0;
    current_frame_timeout = durations[currentAnimation][0];
    loadFrames();
}

void unloadFrames()
{
    for(int i = 0; i < total_frames; i++)
    {
        cairo_surface_destroy(frames[i]);
    }
    free(frames);
}

void loadFrames()
{
    frames = malloc(sizeof(cairo_surface_t *) * total_frames);
    for(int i = 0; i < total_frames; i++)
    {
        frames[i] = cairo_image_surface_create_from_png(frames_list[currentAnimation][i]);
        if(cairo_surface_status(frames[i]) != CAIRO_STATUS_SUCCESS)
        {
            fprintf(stderr, "Cannot load image\n");
            exit(1);
        }
    }
}

void loadBackground()
{
    bg = cairo_image_surface_create_from_png("image/bg/bg_stream.png");
    screenSaver = cairo_image_surface_create_from_png("image/bg/bg_stream_screensaver_3.png");
    if(cairo_surface_status(bg) != CAIRO_STATUS_SUCCESS || cairo_surface_status(screenSaver) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Cannot load image\n");
        exit(1);
    }
}

int init(Display **display, Window *window)
{
    loadBackground();
    width = cairo_image_surface_get_width(bg);
    height = cairo_image_surface_get_height(bg);
    *display = XOpenDisplay(NULL);
    if(*display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    int screen = DefaultScreen(*display);

    *window = XCreateSimpleWindow(*display, RootWindow(*display, screen), 10, 10, width, height, 1, BlackPixel(*display, screen), WhitePixel(*display, screen));

    // set window title
    XStoreName(*display, *window, "Webcam");

    // mask for events
    XSelectInput(*display, *window, ExposureMask | KeyPressMask | DestroyNotify);

    // show window
    XMapWindow(*display, *window);

    // load init animation
    changeAnimation(currentAnimation);
    return 0;
}

void destroy()
{
    unloadFrames();
    cairo_surface_destroy(bg);
    cairo_surface_destroy(screenSaver);
}