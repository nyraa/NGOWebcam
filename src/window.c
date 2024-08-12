#include "window.h"
#include "stream.h"

#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/keysym.h>

animation_t currentAnimation = stream_ame_hair_long;
int total_frames = 0;

cairo_surface_t **frames = NULL;
double current_frame_timeout = 0.0;
int current_frame = 0;
long start_time = 0;

cairo_surface_t *bg = NULL;
cairo_surface_t *screenSaver = NULL;

int height;
int width;
cairo_surface_t *surface;
cairo_t *cr;
int screen;

int handleWindow(Display *display, Window window)
{
    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDeleteMessage, 1);
    XEvent event;
    while(1)
    {
        if(XPending(display) > 0)
        {
            XNextEvent(display, &event);
            if(event.type == Expose)
            {
                drawImage(window, bg, width, height, screenSaver, frames[current_frame]);
            }

            if(event.type == KeyPress)
            {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if(key == XK_q)
                {
                    break;
                }
                if(key == XK_Down || key == XK_Right)
                {
                    changeAnimation(currentAnimation + 1);
                }
                if(key == XK_Up || key == XK_Left)
                {
                    changeAnimation(currentAnimation - 1);
                }
            }
            if(event.type == ClientMessage)
            {
                if(event.xclient.data.l[0] == wmDeleteMessage)
                {
                    printf("Exiting\n");
                    break;
                }
            }
        }
        if(updateFrame())
        {
            drawImage(window, bg, width, height, screenSaver, frames[current_frame]);
        }
    }
    return 0;
}

int updateFrame()
{
    if(getMilliseconds() - start_time >= current_frame_timeout)
    {
        start_time = getMilliseconds();
        current_frame++;
        printf("Frame: %d\n", current_frame);
        if(current_frame >= total_frames)
        {
            current_frame = 0;
        }
        current_frame_timeout = durations[currentAnimation][current_frame] * 1000;
        return 1;
    }
    else
    {
        return 0;
    }
}

void drawImage(Window window, cairo_surface_t *bg, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame)
{
    printf("Drawing frame %d\n", current_frame);

    cairo_push_group(cr);
    cairo_set_source_surface(cr, bg, 0, 0);
    cairo_paint(cr);

    cairo_set_source_surface(cr, screenSaver, 0, 0);
    cairo_paint(cr);

    cairo_set_source_surface(cr, ame, 0, 0);
    cairo_paint(cr);

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);
    cairo_surface_flush(surface);
}

void changeAnimation(animation_t target)
{
    unloadFrames();
    if(target >= ANIMATION_FINISHED)
    {
        target = 0;
    }
    else if(target < 0)
    {
        target = ANIMATION_FINISHED - 1;
    }
    currentAnimation = target;
    total_frames = frame_counts[currentAnimation];
    while(total_frames <= 0)
    {
        currentAnimation++;
        if(currentAnimation >= ANIMATION_FINISHED)
        {
            currentAnimation = 0;
        }
        total_frames = frame_counts[currentAnimation];
    }
    current_frame = 0;
    current_frame_timeout = durations[currentAnimation][0];
    start_time = getMilliseconds();
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
    int w, h;
    frames = malloc(sizeof(cairo_surface_t *) * total_frames);
    for(int i = 0; i < total_frames; i++)
    {
        cairo_surface_t *frame = cairo_image_surface_create_from_png(frames_list[currentAnimation][i]);
        if(cairo_surface_status(frame) != CAIRO_STATUS_SUCCESS)
        {
            fprintf(stderr, "Cannot load image\n");
            exit(1);
        }
        w = cairo_image_surface_get_width(frame);
        h = cairo_image_surface_get_height(frame);
        if(w != width || h != height)
        {
            cairo_surface_t *scaled = cairo_image_surface_create(cairo_image_surface_get_format(frame), width, height);
            cairo_t *cr = cairo_create(scaled);
            cairo_scale(cr, (double)width / w, (double)height / h);
            cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
            cairo_set_source_surface(cr, frame, 0, 0);
            cairo_paint(cr);
            cairo_destroy(cr);
            cairo_surface_destroy(frame);
            frames[i] = scaled;
        }
        else
        {
            frames[i] = frame;
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
    screen = DefaultScreen(*display);

    Visual *visual = DefaultVisual(*display, screen);

    *window = XCreateSimpleWindow(*display, RootWindow(*display, screen), 0, 0, width, height, 0, BlackPixel(*display, screen), WhitePixel(*display, screen));

    // set window title
    XStoreName(*display, *window, "Webcam");

    // mask for events
    XSelectInput(*display, *window, ExposureMask | KeyPressMask | DestroyNotify);

    // show window
    XMapWindow(*display, *window);

    // load init animation
    changeAnimation(currentAnimation);

    // create cairo surface
    surface = cairo_xlib_surface_create(*display, *window, visual, width, height);
    cr = cairo_create(surface);

    // init draw
    drawImage(*window, bg, width, height, screenSaver, frames[current_frame]);
    start_time = getMilliseconds();
    return 0;
}

unsigned long long getMilliseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

void destroy()
{
    unloadFrames();
    cairo_surface_destroy(bg);
    cairo_surface_destroy(screenSaver);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}