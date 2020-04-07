/* input.h  -  Input library entry points  -  Public Domain  -  2017 Mattias Jansson
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

#pragma once

/*! \file input.h
    Input library */

#include <input/types.h>
#include <input/event.h>
#include <input/hashstrings.h>

INPUT_API int
input_module_initialize(const input_config_t config);

INPUT_API void
input_module_finalize(void);

INPUT_API version_t
input_module_version(void);
