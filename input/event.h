/* event.h  -  Input events  -  Public Domain  -  2017 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform input handling in C11 providing for projects based on our
 * foundation library. The latest source code is always available at
 *
 * https://github.com/rampantpixels/input_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#pragma once

/*! \file event.h
    Input events */

#include <input/types.h>

INPUT_API void
input_event_post_key(input_event_id id, unsigned int key, unsigned int scancode);

INPUT_API void
input_event_post_mouse(input_event_id id, int x, int y, real dx, real dy, real dz, unsigned int button, unsigned int buttons);

INPUT_API void
input_event_post_touch(input_event_id id, int x, int y, real dx, real dy, real velocity, unsigned int touch, unsigned int touches);

INPUT_API void
input_event_post_acceleration(input_event_id id, real x, real y, real z);

INPUT_API void
input_event_process(void);

INPUT_API event_stream_t*
input_event_stream(void);

/*! Handle window events. No other event types should be
passed to this function.
\param event Window event */
INPUT_API void
input_event_handle_window(event_t* event);
