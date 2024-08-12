#include "stream.h"

#include <X11/Xlib.h>
#include <cairo/cairo.h>

int updateFrame();
void drawImage(Window window, cairo_surface_t *image, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame);
void changeAnimation(animation_t target);
void unloadFrames();
void loadFrames();
void loadBackground();
int handleWindow(Display *display, Window window);
int init(Display **display, Window *window);
void destroy();
unsigned long long getMilliseconds();