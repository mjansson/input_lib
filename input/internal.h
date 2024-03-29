/* internal.h  -  Input internals  -  Public Domain  -  2017 Mattias Jansson
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

INPUT_EXTERN event_stream_t* input_event_stream_current;

INPUT_API int
input_module_initialize_native(void);

INPUT_API void
input_module_finalize_native(void);

INPUT_API int
input_event_initialize(void);

INPUT_API void
input_event_finalize(void);
