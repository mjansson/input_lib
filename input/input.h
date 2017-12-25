/* input.h  -  Input library entry points  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file input.h
    Input library */

#include <input/types.h>

INPUT_API int
input_module_initialize(void);

INPUT_API void
input_module_finalize(void);

INPUT_API version_t
input_module_version(void);
