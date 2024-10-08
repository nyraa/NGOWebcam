#include "window.h"
#include "stream.h"
#include "webcamManager.h"

#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/keysym.h>
#include <unistd.h>

// from webcamManager
extern cairo_surface_t **frames;
extern animation_t currentAnimation;
extern int total_frames;

extern double current_frame_timeout;
extern int current_frame;
extern unsigned long long start_time;


cairo_surface_t *bg = NULL;
cairo_surface_t **screenSaver = NULL;
int current_screensaver = 0;

int height;
int width;

// surface link to window
cairo_surface_t *surface;
cairo_t *cr;
int screen;

int handleWindow(Display *display, Window window)
{
    // set close event
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
                drawImage(window, bg, width, height, screenSaver[current_screensaver], frames[current_frame]);
            }

            if(event.type == KeyPress)
            {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if(key == XK_q)
                {
                    break;
                }
                if(key == XK_Right)
                {
                    changeAnimation(currentAnimation + 1);
                }
                if(key == XK_Left)
                {
                    changeAnimation(currentAnimation - 1);
                }
                if(key == XK_Up)
                {
                    current_screensaver++;
                    if(current_screensaver >= screensaver_count)
                    {
                        current_screensaver = 0;
                    }
                    // force update when change screensaver
                    drawImage(window, bg, width, height, screenSaver[current_screensaver], frames[current_frame]);
                }
                if(key == XK_Down)
                {
                    current_screensaver--;
                    if(current_screensaver < 0)
                    {
                        current_screensaver = screensaver_count - 1;
                    }
                    drawImage(window, bg, width, height, screenSaver[current_screensaver], frames[current_frame]);
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
            drawImage(window, bg, width, height, screenSaver[current_screensaver], frames[current_frame]);
        }
        usleep(10000);
    }
    return 0;
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


void loadBackground()
{
    bg = cairo_image_surface_create_from_png("image/bg/bg_stream.png");
    screenSaver = malloc(sizeof(cairo_surface_t *) * screensaver_count);
    for(int i = 0; i < screensaver_count; ++i)
    {
        screenSaver[i] = cairo_image_surface_create_from_png(screensavers[i]);
        if(cairo_surface_status(screenSaver[i]) != CAIRO_STATUS_SUCCESS)
        {
            fprintf(stderr, "Cannot load image %s\n", screensavers[i]);
            exit(1);
        }
    }
    if(cairo_surface_status(bg) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Cannot load background image\n");
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
    drawImage(*window, bg, width, height, screenSaver[current_screensaver], frames[current_frame]);
    start_time = getMilliseconds();
    return 0;
}

void destroy()
{
    unloadFrames();
    cairo_surface_destroy(bg);
    for(int i = 0; i < screensaver_count; ++i)
        cairo_surface_destroy(screenSaver[i]);
    free(screenSaver);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}