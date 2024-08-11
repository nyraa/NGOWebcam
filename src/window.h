#include "stream.h"

#include <X11/Xlib.h>
#include <cairo/cairo.h>

void drawImage(Display *display, Window window, cairo_surface_t *image, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame);
void changeAnimation(animation_t target);
void unloadFrames();
void loadFrames();
int init(Display **display, Window *window, int width, int height);