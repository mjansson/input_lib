/* input_linux.c  -  Input library Linux implementation  -  Public Domain  -  2017 Mattias Jansson
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
#include <input/internal.h>

#if FOUNDATION_PLATFORM_LINUX

#include <foundation/time.h>
#include <foundation/log.h>
#include <foundation/posix.h>

#include <window/event.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>

static unsigned long
lookup_key(KeySym sym) {
	if (sym >= XK_a && sym <= XK_z)
		return KEY_A + (sym - XK_a);
	if (sym >= XK_A && sym <= XK_Z)
		return KEY_A + (sym - XK_A);
	if (sym >= XK_0 && sym <= XK_9)
		return KEY_0 + (sym - XK_0);
	if (sym >= XK_F1 && sym <= XK_F24)
		return KEY_F1 + (sym - XK_F1);
	if (sym >= XK_KP_0 && sym <= XK_KP_9)
		return KEY_NP_0 + (sym - XK_KP_0);
	switch (sym) {
		case XK_Escape:
			return KEY_ESCAPE;
		case XK_space:
			return KEY_SPACE;
		case XK_Return:
			return KEY_RETURN;
		case XK_KP_Enter:
			return KEY_RETURN;
		case XK_Up:
			return KEY_UP;
		case XK_Down:
			return KEY_DOWN;
		case XK_Left:
			return KEY_LEFT;
		case XK_Right:
			return KEY_RIGHT;
		case XK_BackSpace:
			return KEY_BACKSPACE;
		case XK_KP_Add:
			return KEY_NP_PLUS;
		case XK_KP_Subtract:
			return KEY_NP_MINUS;
		case XK_KP_Separator:
			return KEY_NP_DECIMAL;
		case XK_KP_Divide:
			return KEY_NP_DIVIDE;
		case XK_KP_Multiply:
			return KEY_NP_MULTIPLY;
		case XK_Num_Lock:
			return KEY_NP_NUMLOCK;
		case XK_KP_Insert:
			return KEY_NP_0;
		case XK_KP_End:
			return KEY_NP_1;
		case XK_KP_Down:
			return KEY_NP_2;
		case XK_KP_Page_Down:
			return KEY_NP_3;
		case XK_KP_Left:
			return KEY_NP_4;
		case XK_KP_Begin:
			return KEY_NP_5;
		case XK_KP_Right:
			return KEY_NP_6;
		case XK_KP_Home:
			return KEY_NP_7;
		case XK_KP_Up:
			return KEY_NP_8;
		case XK_KP_Page_Up:
			return KEY_NP_9;
		case XK_Tab:
			return KEY_TAB;
		case XK_Caps_Lock:
			return KEY_CAPSLOCK;
		case XK_Shift_L:
			return KEY_LSHIFT;
		case XK_Shift_R:
			return KEY_RSHIFT;
		case XK_Control_L:
			return KEY_LCTRL;
		case XK_Control_R:
			return KEY_RCTRL;
		case XK_Alt_L:
			return KEY_LALT;
		case XK_Alt_R:
			return KEY_RALT;
		case 0xFE03:
			return KEY_RALT;
		case XK_Super_L:
			return KEY_LMETA;
		case XK_Super_R:
			return KEY_RMETA;
		case XK_Menu:
			return KEY_MENU;
		case XK_less:
			return KEY_LESS;
		case XK_Insert:
			return KEY_INSERT;
		case XK_Delete:
			return KEY_DELETE;
		case XK_Home:
			return KEY_HOME;
		case XK_End:
			return KEY_END;
		case XK_Page_Up:
			return KEY_PAGEUP;
		case XK_Page_Down:
			return KEY_PAGEDOWN;
		case XK_Print:
			return KEY_PRINTSCREEN;
		case XK_Scroll_Lock:
			return KEY_SCROLLLOCK;
		case XK_Pause:
			return KEY_PAUSE;
		case XK_plus:
			return KEY_PLUS;
		case XK_grave:
			return KEY_GRAVEACCENT;
		// case XK_dead_acute: return KEY_GRAVEACCENT;
		case XK_apostrophe:
			return KEY_APOSTROPHE;
		case XK_comma:
			return KEY_COMMA;
		case XK_minus:
			return KEY_MINUS;
		case XK_period:
			return KEY_PERIOD;
		case XK_slash:
			return KEY_SLASH;
		case XK_backslash:
			return KEY_BACKSLASH;
		case XK_bracketleft:
			return KEY_LEFTBRACKET;
		case XK_bracketright:
			return KEY_RIGHTBRACKET;
		case XK_colon:
			return KEY_COLON;
		case XK_semicolon:
			return KEY_SEMICOLON;
		case XK_numbersign:
			return KEY_HASH;
		case XK_equal:
			return KEY_EQUAL;
	}
	return KEY_UNKNOWN;
}

static int mouse_x, mouse_y;
static unsigned int mouse_buttons;
static int mouse_down_x[8];
static int mouse_down_y[8];
static time_t mouse_down_time[8];

int
input_module_initialize_native(void) {
	mouse_x = -1;
	mouse_y = -1;
	mouse_buttons = 0;
	return 0;
}

void
input_module_finalize_native(void) {
}

void
input_event_process(void) {
}

void
input_event_handle_window(event_t* event) {
	// Extract data from native message
	if (event->id != WINDOWEVENT_NATIVE)
		return;

	struct {
		window_t* window;
		XEvent xevent;
	}* data = (void*)event->payload;

	unsigned int button = 0;
	KeySym sym;

	XPointerMovedEvent* moveevent = (XPointerMovedEvent*)&data->xevent;
	XButtonEvent* buttonevent = (XButtonEvent*)&data->xevent;
	XKeyEvent* keyevent = (XKeyEvent*)&data->xevent;
	XMappingEvent* mapevent = (XMappingEvent*)&data->xevent;

	switch (data->xevent.type) {
		case MotionNotify:
			if (mouse_x < 0) {
				mouse_x = moveevent->x;
				mouse_y = moveevent->y;
			}

			input_event_post_mouse(INPUTEVENT_MOUSEMOVE, moveevent->x, moveevent->y,
			                       (real)((int)moveevent->x - mouse_x), (real)((int)moveevent->y - mouse_y), 0, 0,
			                       mouse_buttons);

			mouse_x = moveevent->x;
			mouse_y = moveevent->y;
			break;

		case ButtonPress:
		case ButtonRelease:
			switch (buttonevent->button) {
				case 1:
					button = MOUSEBUTTON_LEFT;
					break;
				case 2:
					button = MOUSEBUTTON_MIDDLE;
					break;
				case 3:
					button = MOUSEBUTTON_RIGHT;
					break;
				default:
					// button = p_buttonevent->button - 1;
					break;
			}

			if (data->xevent.type == ButtonPress) {
				mouse_buttons |= button;
				mouse_down_x[button] = (int)moveevent->x;
				mouse_down_y[button] = (int)moveevent->y;
				mouse_down_time[button] = time_current();
				input_event_post_mouse(INPUTEVENT_MOUSEDOWN, mouse_down_x[button], mouse_down_y[button], 0, 0, 0,
				                       button, mouse_buttons);
			} else {
				int dx = moveevent->x - mouse_down_x[button];
				int dy = moveevent->y - mouse_down_y[button];
				mouse_buttons &= ~button;
				input_event_post_mouse(INPUTEVENT_MOUSEUP, moveevent->x, moveevent->y, dx, dy,
				                       time_elapsed(mouse_down_time[button]), button, mouse_buttons);
			}

			mouse_x = moveevent->x;
			mouse_y = moveevent->y;

			break;

		case MappingNotify:
			if ((mapevent->request == MappingModifier) || (mapevent->request == MappingKeyboard)) {
				XRefreshKeyboardMapping((void*)mapevent);
			}
			break;

		case KeyRelease:
		case KeyPress:
			if (data->xevent.type == KeyPress) {
				static const int bufsize = 128;
				static char buf[128];
				wchar_t* str = 0;
				size_t len = 0;
				if (data->window->xic) {
					Status status;
					int num = Xutf8LookupString((XIC)data->window->xic, keyevent, buf, bufsize, 0, &status);
					if ((status == XLookupChars) && (num > 0)) {
						// Convert to UTF-16
						str = wstring_allocate_from_string(buf, (size_t)num);
					}
				} else {
					// Fallback to Latin-1 processing
					static bool init_compose = true;
					static XComposeStatus compose;
					if (init_compose) {
						memset(&compose, 0, sizeof(compose));
						init_compose = false;
					}
					int num = XLookupString(keyevent, buf, bufsize, 0, &compose);
					if (num > 0)
						str = wstring_allocate_from_string(buf, (size_t)num);
				}
				if (str)
					len = wstring_length(str);
				for (size_t i = 0; i < len; ++i)
					input_event_post_key(INPUTEVENT_CHAR, (unsigned int)str[i], 0, 0);
			}

			sym = XLookupKeysym(keyevent, 0);
			if (data->xevent.type == KeyPress)
				input_event_post_key(INPUTEVENT_KEYDOWN, (unsigned int)lookup_key(sym), keyevent->keycode, 0);
			else
				input_event_post_key(INPUTEVENT_KEYUP, (unsigned int)lookup_key(sym), keyevent->keycode, 0);
			break;
	}
}

#endif
