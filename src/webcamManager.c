#include "webcamManager.h"
#include "stream.h"

#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


// from window
extern int width, height;

animation_t currentAnimation = stream_ame_idle_happy_a;
int total_frames = 0;
bool isLoop = true;

cairo_surface_t **frames = NULL;
double current_frame_timeout = 0.0;
int current_frame = 0;
unsigned long long start_time = 0;


void changeAnimation(animation_t target)
{
    // unload current frames
    unloadFrames();

    // check if target is out of bounds
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
    current_frame = 0;
    current_frame_timeout = durations[currentAnimation][0];
    isLoop = loop_list[currentAnimation];
    start_time = getMilliseconds();

    // load new frames
    loadFrames();
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

        // resize image if needed (some assets ratio is strange, not work for all)
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

void unloadFrames()
{
    for(int i = 0; i < total_frames; i++)
    {
        cairo_surface_destroy(frames[i]);
    }
    free(frames);
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

unsigned long long getMilliseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}