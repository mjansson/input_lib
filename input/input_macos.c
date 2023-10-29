/* input_macos.c  -  Input library macOS implementation  -  Public Domain  -  2017 Mattias Jansson
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

#if FOUNDATION_PLATFORM_MACOS

#include <input/internal.h>

#include <foundation/apple.h>
#include <foundation/log.h>

#include <window/types.h>

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wfloat-equal")
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif
#endif

// Apple keycode constants, from Inside Macintosh
#define MK_ESCAPE 0x35
#define MK_F1 0x7A
#define MK_F2 0x78
#define MK_F3 0x63
#define MK_F4 0x76
#define MK_F5 0x60
#define MK_F6 0x61
#define MK_F7 0x62
#define MK_F8 0x64
#define MK_F9 0x65
#define MK_F10 0x6D
#define MK_F11 0x67
#define MK_F12 0x6F
#define MK_F13 0x69
#define MK_F14 0x6B
#define MK_F15 0x71
#define MK_F16 0x6A
#define MK_POWER 0x7F
#define MK_GRAVE 0x32
#define MK_1 0x12
#define MK_2 0x13
#define MK_3 0x14
#define MK_4 0x15
#define MK_5 0x17
#define MK_6 0x16
#define MK_7 0x1A
#define MK_8 0x1C
#define MK_9 0x19
#define MK_0 0x1D
#define MK_MINUS 0x1B
#define MK_EQUALS 0x18
#define MK_BACKSPACE 0x33
#define MK_HELP 0x72
#define MK_HOME 0x73
#define MK_PAGEUP 0x74
#define MK_NUMLOCK 0x47
#define MK_KP_EQUALS 0x51
#define MK_KP_DIVIDE 0x4B
#define MK_KP_MULTIPLY 0x43
#define MK_TAB 0x30
#define MK_Q 0x0C
#define MK_W 0x0D
#define MK_E 0x0E
#define MK_R 0x0F
#define MK_T 0x11
#define MK_Y 0x10
#define MK_U 0x20
#define MK_I 0x22
#define MK_O 0x1F
#define MK_P 0x23
#define MK_LEFTBRACKET 0x21
#define MK_RIGHTBRACKET 0x1E
#define MK_BACKSLASH 0x2A
#define MK_DELETE 0x75
#define MK_END 0x77
#define MK_PAGEDOWN 0x79
#define MK_KP7 0x59
#define MK_KP8 0x5B
#define MK_KP9 0x5C
#define MK_KP_MINUS 0x4E
#define MK_CAPSLOCK 0x39
#define MK_A 0x00
#define MK_S 0x01
#define MK_D 0x02
#define MK_F 0x03
#define MK_G 0x05
#define MK_H 0x04
#define MK_J 0x26
#define MK_K 0x28
#define MK_L 0x25
#define MK_SEMICOLON 0x29
#define MK_QUOTE 0x27
#define MK_RETURN 0x24
#define MK_KP4 0x56
#define MK_KP5 0x57
#define MK_KP6 0x58
#define MK_KP_PLUS 0x45
#define MK_SHIFT 0x38
#define MK_Z 0x06
#define MK_X 0x07
#define MK_C 0x08
#define MK_V 0x09
#define MK_B 0x0B
#define MK_N 0x2D
#define MK_M 0x2E
#define MK_COMMA 0x2B
#define MK_PERIOD 0x2F
#define MK_SLASH 0x2C
#define MK_UP 0x7E
#define MK_KP1 0x53
#define MK_KP2 0x54
#define MK_KP3 0x55
#define MK_KP_ENTER 0x4C
#define MK_CTRL 0x3B
#define MK_ALT 0x3A
#define MK_APPLE 0x37
#define MK_SPACE 0x31
#define MK_LEFT 0x7B
#define MK_DOWN 0x7D
#define MK_RIGHT 0x7C
#define MK_KP0 0x52
#define MK_KP_PERIOD 0x41
/*#define MK_IBOOK_ENTER          0x34
 #define MK_IBOOK_LEFT           0x3B
 #define MK_IBOOK_RIGHT          0x3C
 #define MK_IBOOK_DOWN           0x3D
 #define MK_IBOOK_UP             0x3E*/
#define MK_MACBOOK_FN 0x3F

static bool keydown[256];
static const UCKeyboardLayout* keyboard_layout;
static uint32_t deadkeys;
static CGEventSourceRef key_event_source;
static uint32_t keytranslator[0x200] = {0};
static double mouse_x;
static double mouse_y;
static uint mouse_buttons;
static CGPoint mouse_down[8];
static time_t mouse_down_time[8];

static unsigned int
translate_key(unsigned int key, unsigned int modifiers) {
	if (!keyboard_layout)
		return key;

	UniCharCount len = 0;
	UniChar character[2];
	UInt32 dead = deadkeys;
	UCKeyTranslate(keyboard_layout, (UInt16)key, kUCKeyActionDown, modifiers,
	               CGEventSourceGetKeyboardType(key_event_source), 0, &dead, 1, &len, character);
	deadkeys = dead;
	return character[0];
	// return( KeyTranslate( _p_kchr, ( key & 0x7F ) | modifiers, (UInt32*)&_deadkeys ) );
}

static void
build_translator_table(void) {
	// Default to a standard US keyboard
	keytranslator[MK_ESCAPE] = KEY_ESCAPE;
	keytranslator[MK_F1] = KEY_F1;
	keytranslator[MK_F2] = KEY_F2;
	keytranslator[MK_F3] = KEY_F3;
	keytranslator[MK_F4] = KEY_F4;
	keytranslator[MK_F5] = KEY_F5;
	keytranslator[MK_F6] = KEY_F6;
	keytranslator[MK_F7] = KEY_F7;
	keytranslator[MK_F8] = KEY_F8;
	keytranslator[MK_F9] = KEY_F9;
	keytranslator[MK_F10] = KEY_F10;
	keytranslator[MK_F11] = KEY_F11;
	keytranslator[MK_F12] = KEY_F12;
	keytranslator[MK_F13] = KEY_F13;
	keytranslator[MK_F14] = KEY_F14;
	keytranslator[MK_F15] = KEY_F15;
	keytranslator[MK_F16] = KEY_F16;
	keytranslator[MK_POWER] = KEY_UNKNOWN;
	keytranslator[MK_GRAVE] = KEY_GRAVEACCENT;
	keytranslator[MK_1] = KEY_0;
	keytranslator[MK_2] = KEY_1;
	keytranslator[MK_3] = KEY_2;
	keytranslator[MK_4] = KEY_3;
	keytranslator[MK_5] = KEY_4;
	keytranslator[MK_6] = KEY_5;
	keytranslator[MK_7] = KEY_6;
	keytranslator[MK_8] = KEY_7;
	keytranslator[MK_9] = KEY_8;
	keytranslator[MK_0] = KEY_9;
	keytranslator[MK_MINUS] = KEY_MINUS;
	keytranslator[MK_EQUALS] = KEY_EQUAL;
	keytranslator[MK_BACKSPACE] = KEY_BACKSPACE;
	keytranslator[MK_HELP] = KEY_UNKNOWN;
	keytranslator[MK_HOME] = KEY_HOME;
	keytranslator[MK_PAGEUP] = KEY_PAGEUP;
	keytranslator[MK_NUMLOCK] = KEY_NP_NUMLOCK;
	keytranslator[MK_KP_EQUALS] = KEY_NP_EQUAL;
	keytranslator[MK_KP_DIVIDE] = KEY_NP_DIVIDE;
	keytranslator[MK_KP_MULTIPLY] = KEY_NP_MULTIPLY;
	keytranslator[MK_TAB] = KEY_TAB;
	keytranslator[MK_Q] = KEY_Q;
	keytranslator[MK_W] = KEY_W;
	keytranslator[MK_E] = KEY_E;
	keytranslator[MK_R] = KEY_R;
	keytranslator[MK_T] = KEY_T;
	keytranslator[MK_Y] = KEY_Y;
	keytranslator[MK_U] = KEY_U;
	keytranslator[MK_I] = KEY_I;
	keytranslator[MK_O] = KEY_O;
	keytranslator[MK_P] = KEY_P;
	keytranslator[MK_LEFTBRACKET] = KEY_LEFTBRACKET;
	keytranslator[MK_RIGHTBRACKET] = KEY_RIGHTBRACKET;
	keytranslator[MK_BACKSLASH] = KEY_BACKSLASH;
	keytranslator[MK_DELETE] = KEY_DELETE;
	keytranslator[MK_END] = KEY_END;
	keytranslator[MK_PAGEDOWN] = KEY_PAGEDOWN;
	keytranslator[MK_KP7] = KEY_NP_7;
	keytranslator[MK_KP8] = KEY_NP_8;
	keytranslator[MK_KP9] = KEY_NP_9;
	keytranslator[MK_KP_MINUS] = KEY_NP_MINUS;
	keytranslator[MK_CAPSLOCK] = KEY_CAPSLOCK;
	keytranslator[MK_A] = KEY_A;
	keytranslator[MK_S] = KEY_S;
	keytranslator[MK_D] = KEY_D;
	keytranslator[MK_F] = KEY_F;
	keytranslator[MK_G] = KEY_G;
	keytranslator[MK_H] = KEY_H;
	keytranslator[MK_J] = KEY_J;
	keytranslator[MK_K] = KEY_K;
	keytranslator[MK_L] = KEY_L;
	keytranslator[MK_SEMICOLON] = KEY_SEMICOLON;
	keytranslator[MK_QUOTE] = KEY_QUOTE;
	keytranslator[MK_RETURN] = KEY_RETURN;
	keytranslator[MK_KP4] = KEY_NP_4;
	keytranslator[MK_KP5] = KEY_NP_5;
	keytranslator[MK_KP6] = KEY_NP_6;
	keytranslator[MK_KP_PLUS] = KEY_NP_PLUS;
	keytranslator[MK_SHIFT] = KEY_LSHIFT;
	keytranslator[MK_Z] = KEY_Z;
	keytranslator[MK_X] = KEY_X;
	keytranslator[MK_C] = KEY_C;
	keytranslator[MK_V] = KEY_V;
	keytranslator[MK_B] = KEY_B;
	keytranslator[MK_N] = KEY_N;
	keytranslator[MK_M] = KEY_M;
	keytranslator[MK_COMMA] = KEY_COMMA;
	keytranslator[MK_PERIOD] = KEY_PERIOD;
	keytranslator[MK_SLASH] = KEY_SLASH;
	keytranslator[MK_UP] = KEY_UP;
	keytranslator[MK_KP1] = KEY_NP_1;
	keytranslator[MK_KP2] = KEY_NP_2;
	keytranslator[MK_KP3] = KEY_NP_3;
	keytranslator[MK_KP_ENTER] = KEY_NP_ENTER;
	keytranslator[MK_CTRL] = KEY_LCTRL;
	keytranslator[MK_ALT] = KEY_LALT;
	keytranslator[MK_APPLE] = KEY_LMETA;
	keytranslator[MK_SPACE] = KEY_SPACE;
	keytranslator[MK_LEFT] = KEY_LEFT;
	keytranslator[MK_DOWN] = KEY_DOWN;
	keytranslator[MK_RIGHT] = KEY_RIGHT;
	keytranslator[MK_KP0] = KEY_NP_0;
	keytranslator[MK_KP_PERIOD] = KEY_NP_DECIMAL;
	/*keytranslator[ MK_IBOOK_ENTER ] = KEY_RETURN;
	 keytranslator[ MK_IBOOK_LEFT ]  = KEY_LEFT;
	 keytranslator[ MK_IBOOK_RIGHT ] = KEY_RIGHT;
	 keytranslator[ MK_IBOOK_DOWN ]  = KEY_DOWN;
	 keytranslator[ MK_IBOOK_UP ]    = KEY_UP;*/
	keytranslator[MK_MACBOOK_FN] = KEY_FN;

	// Now lookup keysyms using the current KCHR resource
	deadkeys = 0;
	for (uint i = 0; i < 0x200; ++i) {
		uint key = translate_key(i, 0) & 0xFF;
		if (!key) {
			// Dead key, repeat
			key = translate_key(i, 0) & 0xFF;
		}

		if (key >= 127)  // Non-ASCII char
		{
			if (key > KEY_UNKNOWN)
				keytranslator[i] = KEY_UNKNOWN;  // Unknown unicode?
			else
				keytranslator[i] = KEY_NONASCIIKEYCODE | (key & KEY_UNKNOWN);  // We have some non-US keys mapped...
		} else if (key >= 32)                                                  // ASCII char (not control char)
		{
			if ((key >= 97) && (key <= 122))
				key -= 32;  // lower -> upper case
			keytranslator[i] = key;
		}
	}

	// Reinsert overwritten numpad mappings (maps to ascii number chars above)
	keytranslator[MK_NUMLOCK] = KEY_NP_NUMLOCK;
	keytranslator[MK_KP_EQUALS] = KEY_NP_EQUAL;
	keytranslator[MK_KP_DIVIDE] = KEY_NP_DIVIDE;
	keytranslator[MK_KP_MULTIPLY] = KEY_NP_MULTIPLY;
	keytranslator[MK_KP7] = KEY_NP_7;
	keytranslator[MK_KP8] = KEY_NP_8;
	keytranslator[MK_KP9] = KEY_NP_9;
	keytranslator[MK_KP_MINUS] = KEY_NP_MINUS;
	keytranslator[MK_KP4] = KEY_NP_4;
	keytranslator[MK_KP5] = KEY_NP_5;
	keytranslator[MK_KP6] = KEY_NP_6;
	keytranslator[MK_KP_PLUS] = KEY_NP_PLUS;
	keytranslator[MK_KP1] = KEY_NP_1;
	keytranslator[MK_KP2] = KEY_NP_2;
	keytranslator[MK_KP3] = KEY_NP_3;
	keytranslator[MK_KP_ENTER] = KEY_NP_ENTER;
	keytranslator[MK_KP0] = KEY_NP_0;
	keytranslator[MK_KP_PERIOD] = KEY_NP_DECIMAL;
}

static void
input_mouse_down(input_mouse_button_id button, double x, double y) {
	mouse_buttons |= button;
	mouse_down[button].x = x;
	mouse_down[button].y = y;
	mouse_down_time[button] = time_current();
	input_event_post_mouse(INPUTEVENT_MOUSEDOWN, (int)mouse_down[button].x, (int)mouse_down[button].y, 0, 0, 0, button,
	                       mouse_buttons);
	// log_infof(0, STRING_CONST("Mouse down %d"), button);
}

static void
input_mouse_up(input_mouse_button_id button, double x, double y) {
	mouse_buttons &= ~button;
	double dx = x - mouse_down[button].x;
	double dy = y - mouse_down[button].y;
	input_event_post_mouse(INPUTEVENT_MOUSEUP, (int)x, (int)y, (real)dx, (real)dy,
	                       time_elapsed(mouse_down_time[button]), button, mouse_buttons);
	// log_infof(0, STRING_CONST("Mouse up %d"), button);
}

/*
static CGEventRef
event_tap_callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) {
    if (type == kCGEventMouseMoved) {
    }

    return event;
}
*/

int
input_module_initialize_native(void) {
	memset(keydown, 0, sizeof(bool) * 256);

	TISInputSourceRef current_keyboard = TISCopyCurrentKeyboardInputSource();
	CFDataRef layoutref = (CFDataRef)TISGetInputSourceProperty(current_keyboard, kTISPropertyUnicodeKeyLayoutData);
	keyboard_layout = (layoutref ? (const void*)CFDataGetBytePtr(layoutref) : 0);
	deadkeys = 0;
	key_event_source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);

	build_translator_table();

	/*
	CFMachPortRef event_tap =
	    CGEventTapCreate(kCGAnnotatedSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionListenOnly,
	                     kCGEventMaskForAllEvents, event_tap_callback, 0);
	CGEventTapEnable(event_tap, true);
	*/

	return 0;
}

void
input_module_finalize_native(void) {
}

void
input_event_handle_window(event_t* event) {
	// Extract data from native message
	if (event->id != WINDOWEVENT_NATIVE)
		return;
}

static void
input_event_process_main_queue(void) {
	TISInputSourceRef current_keyboard = TISCopyCurrentKeyboardInputSource();
	CFDataRef layoutref = (CFDataRef)TISGetInputSourceProperty(current_keyboard, kTISPropertyUnicodeKeyLayoutData);
	const void* layout = (layoutref ? (const void*)CFDataGetBytePtr(layoutref) : 0);

	if (layout != keyboard_layout) {
		deadkeys = 0;
		keyboard_layout = layout;

		log_info(HASH_INPUT, STRING_CONST("Switched UCHR resource"));
		build_translator_table();
	}

	CGEventSourceStateID event_source = kCGEventSourceStateCombinedSessionState;

	// TODO: Look into using event taps instead, this is silly
	for (uint i = 0; i < 256; ++i) {
		if (CGEventSourceKeyState(event_source, (CGKeyCode)i)) {
			if (!keydown[i]) {
				unsigned int keycode = keytranslator[i];
				if (!keycode)
					keycode = KEY_UNKNOWN;
				// printf( "DEBUG TRACE: Key %d down, translated to '%c'\n", i, (char)keycode );

				input_event_post_key(INPUTEVENT_KEYDOWN, keycode, i, 0);

				keydown[i] = true;
			}
		} else {
			if (keydown[i]) {
				unsigned int keycode = keytranslator[i];
				if (!keycode)
					keycode = KEY_UNKNOWN;
				// printf( "DEBUG TRACE: Key %d up, translated to '%c'\n", i, (char)keycode );

				input_event_post_key(INPUTEVENT_KEYUP, keycode, i, 0);

				keydown[i] = false;
			}
		}
	}

	CGEventRef event = CGEventCreate(0);
	CGPoint loc = CGEventGetLocation(event);
	CFRelease(event);

	double dx = mouse_x - loc.x;
	double dy = mouse_y - loc.y;
	mouse_x = loc.x;
	mouse_y = loc.y;

	bool left_down = CGEventSourceButtonState(event_source, kCGMouseButtonLeft);
	bool right_down = CGEventSourceButtonState(event_source, kCGMouseButtonRight);
	bool center_down = CGEventSourceButtonState(event_source, kCGMouseButtonCenter);

	bool was_left_down = ((mouse_buttons & MOUSEBUTTON_LEFT) != 0);
	bool was_right_down = ((mouse_buttons & MOUSEBUTTON_RIGHT) != 0);
	bool was_center_down = ((mouse_buttons & MOUSEBUTTON_MIDDLE) != 0);

	if (left_down && !was_left_down)
		input_mouse_down(MOUSEBUTTON_LEFT, loc.x, loc.y);
	if (!left_down && was_left_down)
		input_mouse_up(MOUSEBUTTON_LEFT, loc.x, loc.y);
	if (right_down && !was_right_down)
		input_mouse_down(MOUSEBUTTON_RIGHT, loc.x, loc.y);
	if (!right_down && was_right_down)
		input_mouse_up(MOUSEBUTTON_RIGHT, loc.x, loc.y);
	if (center_down && !was_center_down)
		input_mouse_down(MOUSEBUTTON_MIDDLE, loc.x, loc.y);
	if (!center_down && was_center_down)
		input_mouse_up(MOUSEBUTTON_MIDDLE, loc.x, loc.y);

	if ((dx != 0) || (dy != 0)) {
		input_event_post_mouse(INPUTEVENT_MOUSEMOVE, (int)mouse_x, (int)mouse_y, (real)dx, (real)dy, 0, 0,
		                       mouse_buttons);
		// log_infof(0, STRING_CONST("Mouse at %d,%d (%.2f, %.2f)"), (int)mouse_x, (int)mouse_y, dx, dy);
	}
}

void
input_event_process(void) {
	dispatch_sync(dispatch_get_main_queue(), ^{
	  input_event_process_main_queue();
	});
}

#endif
