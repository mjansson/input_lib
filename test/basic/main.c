/* main.c  -  Input library basic tests  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform memory allocation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/rampantpixels/memory_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>
#include <test/test.h>

#include <input/input.h>

#include <stdio.h>

static application_t
test_basic_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Input basic tests"));
	app.short_name = string_const(STRING_CONST("test_basic"));
	app.company = string_const(STRING_CONST("Rampant Pixels"));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static foundation_config_t
test_basic_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static memory_system_t
test_basic_memory_system(void) {
	return memory_system_malloc();
}

static int 
test_basic_initialize(void) {
	log_set_suppress(HASH_MEMORY, ERRORLEVEL_DEBUG);
	return 0;
}

static void 
test_basic_finalize(void) {
}

DECLARE_TEST(basic, initfini) {
	return 0;
}

static void 
test_basic_declare(void) {
	ADD_TEST(basic, initfini);
}

static test_suite_t test_basic_suite = {
	test_basic_application,
	test_basic_memory_system,
	test_basic_config,
	test_basic_declare,
	test_basic_initialize,
	test_basic_finalize,
	0
};

#if FOUNDATION_PLATFORM_ANDROID

int
test_basic_run(void);

int
test_basic_run(void) {
	test_suite = test_basic_suite;
	return test_run_all();
}

#else

test_suite_t 
test_suite_define(void);

test_suite_t 
test_suite_define(void) {
	return test_basic_suite;
}

#endif
