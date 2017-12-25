/* input.c  -  Input library entry points  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>

#include "input.h"

int
input_module_initialize(const input_config_t config) {
	FOUNDATION_UNUSED(config);
	return 0;
}

void
input_module_finalize(void) {
}
