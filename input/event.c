/* event.c  -  Input events  -  Public Domain  -  2017 Mattias Jansson / Rampant Pixels
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

#include <input/event.h>
#include <input/internal.h>

#include <foundation/event.h>
#include <foundation/log.h>

event_stream_t* _input_event_stream;

int
input_event_initialize(void) {
	_input_event_stream = event_stream_allocate(1024);
	return 0;
}

void
input_event_finalize(void) {
	event_stream_deallocate(_input_event_stream);
}

event_stream_t*
input_event_stream(void) {
	return _input_event_stream;
}

void
input_event_post(input_event_id id) {
	event_post(_input_event_stream, (int)id, 0, 0, 0, 0);
}

void
input_event_post_key(input_event_id id, unsigned int key, unsigned int scancode) {
	input_event_payload_t payload;
	payload.key.key = key;
	payload.key.scancode = scancode;
	event_post(_input_event_stream, (int)id, 0, 0, &payload, sizeof(payload));
}

void
input_event_post_mouse(input_event_id id, int x, int y, real dx, real dy, real dz,
                       unsigned int button, unsigned int buttons) {
	input_event_payload_t payload;
	payload.mouse.x = x;
	payload.mouse.y = y;
	payload.mouse.dx = dx;
	payload.mouse.dy = dy;
	payload.mouse.dz = dz;
	payload.mouse.button = button;
	payload.mouse.buttons = buttons;
	event_post(_input_event_stream, (int)id, 0, 0, &payload, sizeof(payload));
}

void
input_event_post_touch(input_event_id id, int x, int y, real dx, real dy, real velocity,
                       unsigned int touch, unsigned int touches) {
	input_event_payload_t payload;
	payload.touch.x = x;
	payload.touch.y = y;
	payload.touch.dx = dx;
	payload.touch.dy = dy;
	payload.touch.velocity = velocity;
	payload.touch.touch = touch;
	payload.touch.touches = touches;
	event_post(_input_event_stream, (int)id, 0, 0, &payload, sizeof(payload));
}

void
input_event_post_acceleration(input_event_id id, real x, real y, real z) {
	input_event_payload_t payload;
	payload.acceleration.x = x;
	payload.acceleration.y = y;
	payload.acceleration.z = z;
	event_post(_input_event_stream, (int)id, 0, 0, &payload, sizeof(payload));
}
