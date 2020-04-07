/* input_ios.c  -  Input library iOS implementation  -  Public Domain  -  2017 Mattias Jansson
 *
 * This library provides a cross-platform input handling in C11 providing for projects based on our
 * foundation library. The latest source code is always available at
 *
 * https://github.com/mjansson/input_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <input/input.h>

#if FOUNDATION_PLATFORM_IOS

void
_input_service_initialize_native(void) {
}

void
_input_service_poll_native(void) {
}

#endif
