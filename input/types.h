/* types.h  -  Input library data tyypes  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file types.h
    Input data types */

#include <foundation/platform.h>
#include <foundation/types.h>

#include <input/build.h>
#include <input/hashstrings.h>

#if defined(INPUT_COMPILE) && INPUT_COMPILE
#ifdef __cplusplus
#define INPUT_EXTERN extern "C"
#define INPUT_API extern "C"
#else
#define INPUT_EXTERN extern
#define INPUT_API extern
#endif
#else
#ifdef __cplusplus
#define INPUT_EXTERN extern "C"
#define INPUT_API extern "C"
#else
#define INPUT_EXTERN extern
#define INPUT_API extern
#endif
#endif

typedef enum input_event_id {
	INPUTEVENT_KEYDOWN = 1,
	INPUTEVENT_KEYUP,
	INPUTEVENT_CHAR,
	INPUTEVENT_MOUSEDOWN,
	INPUTEVENT_MOUSEUP,
	INPUTEVENT_MOUSEMOVE,
	INPUTEVENT_TOUCHBEGIN,
	INPUTEVENT_TOUCHEND,
	INPUTEVENT_TOUCHCANCEL,
	INPUTEVENT_TOUCHMOVE,
	INPUTEVENT_TOUCHSWIPE,
	INPUTEVENT_ACCELERATION
} input_event_id;

typedef enum input_mouse_button_id {
	MOUSEBUTTON_LEFT = 0x01,
	MOUSEBUTTON_RIGHT = 0x02,
	MOUSEBUTTON_MIDDLE = 0x04,
	MOUSEBUTTON_3 = 0x08,
	MOUSEBUTTON_4 = 0x10,
	MOUSEBUTTON_5 = 0x20,
	MOUSEBUTTON_6 = 0x40,
	MOUSEBUTTON_7 = 0x80
} input_mouse_button_id;

typedef enum input_key_id {
	KEY_SPACE = 32,
	KEY_EXCLAMATION = 33,
	KEY_DOUBLEQUOTE = 34,
	KEY_HASH = 35,
	KEY_DOLLAR = 36,
	KEY_PERCENT = 37,
	KEY_AMPERSAND = 38,
	KEY_QUOTE = 39,
	KEY_LEFTPARENTHESIS = 40,
	KEY_RIGHTPARENTHESIS = 41,
	KEY_ASTERISK = 42,
	KEY_PLUS = 43,
	KEY_COMMA = 44,
	KEY_MINUS = 45,
	KEY_PERIOD = 46,
	KEY_SLASH = 47,
	KEY_0 = 48,
	KEY_1 = 49,
	KEY_2 = 50,
	KEY_3 = 51,
	KEY_4 = 52,
	KEY_5 = 53,
	KEY_6 = 54,
	KEY_7 = 55,
	KEY_8 = 56,
	KEY_9 = 57,
	KEY_COLON = 58,
	KEY_SEMICOLON = 59,
	KEY_LESS = 60,
	KEY_EQUAL = 61,
	KEY_GREATER = 62,
	KEY_QUESTION = 63,
	KEY_AT = 64,
	KEY_A = 65,
	KEY_B = 66,
	KEY_C = 67,
	KEY_D = 68,
	KEY_E = 69,
	KEY_F = 70,
	KEY_G = 71,
	KEY_H = 72,
	KEY_I = 73,
	KEY_J = 74,
	KEY_K = 75,
	KEY_L = 76,
	KEY_M = 77,
	KEY_N = 78,
	KEY_O = 79,
	KEY_P = 80,
	KEY_Q = 81,
	KEY_R = 82,
	KEY_S = 83,
	KEY_T = 84,
	KEY_U = 85,
	KEY_V = 86,
	KEY_W = 87,
	KEY_X = 88,
	KEY_Y = 89,
	KEY_Z = 90,
	KEY_LEFTBRACKET = 91,
	KEY_BACKSLASH = 92,
	KEY_RIGHTBRACKET = 93,
	KEY_POWER = 94,
	KEY_UNDERSCORE = 95,
	KEY_GRAVEACCENT = 96,
	KEY_LEFTCURL = 123,
	KEY_BAR = 124,
	KEY_RIGHTCURL = 125,
	KEY_TILDE = 126,

	KEY_RETURN = 0x100,
	KEY_ESCAPE = 0x101,
	KEY_BACKSPACE = 0x102,
	KEY_UP = 0x103,
	KEY_DOWN = 0x104,
	KEY_LEFT = 0x105,
	KEY_RIGHT = 0x106,
	KEY_F1 = 0x107,
	KEY_F2 = 0x108,
	KEY_F3 = 0x109,
	KEY_F4 = 0x10a,
	KEY_F5 = 0x10b,
	KEY_F6 = 0x10c,
	KEY_F7 = 0x10d,
	KEY_F8 = 0x10e,
	KEY_F9 = 0x10f,
	KEY_F10 = 0x110,
	KEY_F11 = 0x111,
	KEY_F12 = 0x112,
	KEY_F13 = 0x113,
	KEY_F14 = 0x114,
	KEY_F15 = 0x115,
	KEY_F16 = 0x116,
	KEY_F17 = 0x117,
	KEY_F18 = 0x118,
	KEY_F19 = 0x119,
	KEY_F20 = 0x11a,
	KEY_F21 = 0x11b,
	KEY_F22 = 0x11c,
	KEY_F23 = 0x11d,
	KEY_F24 = 0x11e,
	KEY_NP_0 = 0x11f,
	KEY_NP_1 = 0x120,
	KEY_NP_2 = 0x121,
	KEY_NP_3 = 0x122,
	KEY_NP_4 = 0x123,
	KEY_NP_5 = 0x124,
	KEY_NP_6 = 0x125,
	KEY_NP_7 = 0x126,
	KEY_NP_8 = 0x127,
	KEY_NP_9 = 0x128,
	KEY_NP_PLUS = 0x129,
	KEY_NP_MINUS = 0x12a,
	KEY_NP_DECIMAL = 0x12b,
	KEY_NP_DIVIDE = 0x12c,
	KEY_NP_MULTIPLY = 0x12d,
	KEY_NP_NUMLOCK = 0x12e,
	KEY_NP_EQUAL = 0x12f,
	KEY_NP_ENTER = 0x130,

	// Modifier keys
	KEY_CAPSLOCK = 0x131,
	KEY_LSHIFT = 0x132,
	KEY_LCTRL = 0x133,
	KEY_LALT = 0x134,
	KEY_LMETA = 0x135,
	KEY_RSHIFT = 0x136,
	KEY_RCTRL = 0x137,
	KEY_RALT = 0x138,
	KEY_RMETA = 0x139,
	KEY_MENU = 0x13a,
	KEY_FN = 0x13b,
	// end modifier keys

	KEY_INSERT = 0x13c,
	KEY_DELETE = 0x13d,
	KEY_HOME = 0x13e,
	KEY_END = 0x13f,
	KEY_PAGEUP = 0x140,
	KEY_PAGEDOWN = 0x141,
	KEY_PRINTSCREEN = 0x142,
	KEY_SCROLLLOCK = 0x143,
	KEY_PAUSE = 0x144,
	KEY_TAB = 0x145,
	KEY_BACK = 0x146,
	KEY_CALL = 0x147,
	KEY_ENDCALL = 0x148,
	KEY_SEARCH = 0x149,
	KEY_CENTER = 0x14a,
	KEY_VOLUMEUP = 0x14b,
	KEY_VOLUMEDOWN = 0x14c,
	KEY_CAMERA = 0x14d,
	KEY_CLEAR = 0x14e,
	KEY_SYM = 0x14f,
	KEY_EXPLORER = 0x1b0,
	KEY_ENVELOPE = 0x1b1,
	KEY_ENTER = 0x1b2,
	KEY_NUM = 0x1b3,
	KEY_HEADSETHOOK = 0x1b4,
	KEY_CAMERAFOCUS = 0x1b5,
	KEY_NOTIFICATION = 0x1b6,
	KEY_MEDIAPLAYPAUSE = 0x1b7,
	KEY_MEDIASTOP = 0x1b8,
	KEY_MEDIANEXT = 0x1b9,
	KEY_MEDIAPREVIOUS = 0x1ba,
	KEY_MEDIAREWIND = 0x1bb,
	KEY_MEDIAFASTFORWARD = 0x1bc,
	KEY_MUTE = 0x1bd,
	KEY_PICTSYMBOLS = 0x1be,
	KEY_SWITCHCHARSET = 0x1bf,

	KEY_UNKNOWN = 0xffff,

	KEY_NONASCIIKEYCODE = 0x10000,
	KEY_APOSTROPHE = 0x10001,
	KEY_POUND = 0x10002,
	KEY_EURO = 0x10003,

	KEY_PARAGRAPH = 0x100a4,
	KEY_ACUTEACCENT = 0x100ab,
	KEY_UML = 0x100ac,
	KEY_AUML = 0x1008a,
	KEY_ARING = 0x1008c,
	KEY_OUML = 0x1009a,

	KEY_LASTRESERVEDKEYCODE = 0x1ffff
} input_key_id;

typedef struct input_config_t input_config_t;
typedef struct input_mouse_event_t input_mouse_event_t;
typedef struct input_touch_event_t input_touch_event_t;
typedef struct input_key_event_t input_key_event_t;
typedef struct input_acceleration_event_t input_acceleration_event_t;

struct input_config_t {
	unsigned int _unused;
};

struct input_mouse_event_t {
	int x;
	int y;
	real dx;
	real dy;
	real dz;
	unsigned int button;
	unsigned int buttons;
};

struct input_touch_event_t {
	int x;
	int y;
	real dx;
	real dy;
	real velocity;
	unsigned int touch;
	unsigned int touches;
};

struct input_key_event_t {
	unsigned int key;
	unsigned int scancode;
	unsigned int flags;
};

struct input_acceleration_event_t {
	real x;
	real y;
	real z;
};

typedef union input_event_payload_t {
	input_mouse_event_t mouse;
	input_touch_event_t touch;
	input_key_event_t key;
	input_acceleration_event_t acceleration;
} input_event_payload_t;
