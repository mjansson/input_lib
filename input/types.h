/* types.h  -  Input library data tyypes  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file types.h
    Input data types */

#include <foundation/platform.h>
#include <foundation/types.h>

#include <input/build.h>


#if defined(INPUT_COMPILE) && INPUT_COMPILE
#  ifdef __cplusplus
#  define INPUT_EXTERN extern "C"
#  define INPUT_API extern "C"
#  else
#  define INPUT_EXTERN extern
#  define INPUT_API extern
#  endif
#else
#  ifdef __cplusplus
#  define INPUT_EXTERN extern "C"
#  define INPUT_API extern "C"
#  else
#  define INPUT_EXTERN extern
#  define INPUT_API extern
#  endif
#endif

typedef struct input_config_t input_config_t;

struct input_config_t {
	unsigned int _unused;
};
