#include <X11/Xlib.h>
#include <cairo/cairo.h>

void updateFrame();
void drawImage(Display *display, Window window, cairo_surface_t *image, int width, int height, cairo_surface_t *screenSaver, cairo_surface_t *ame);
void changeAnimation(int target);
void unloadFrames();
void loadFrames();
void loadBackground();
int handleWindow(Display *display, Window window);
int init(Display **display, Window *window);