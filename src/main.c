#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "window.h"

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Failed to fork\n");
            exit(1);
        }
        else if (pid > 0)
        {
            // Parent process, terminate
            exit(0);
        }
    }
    Display *display;
    Window window;

    if(init(&display, &window) != 0)
    {
        fprintf(stderr, "Failed to initialize window\n");
        exit(1);
    }

    handleWindow(display, window);

    destroy();
    XCloseDisplay(display);
    return 0;
}