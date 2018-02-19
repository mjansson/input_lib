/* input_windows.c  -  Input library Windows implementation  -  Public Domain  -  2017 Mattias Jansson / Rampant Pixels
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

#include <input/input.h>
#include <input/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS

#include <foundation/time.h>
#include <foundation/log.h>
#include <foundation/windows.h>

#include <window/event.h>

#if FOUNDATION_COMPILER_MSVC
#  pragma warning ( disable : 4996 )
#endif
#undef ERROR

static int            mouse_x;
static int            mouse_y;
static unsigned int   mouse_buttons;
static unsigned char  keystate_next[256];
static unsigned char  keystate_prev[256];
static bool           unichar = false;
static HWND           last_hwnd;

static unsigned int
translate_key(unsigned int key) {
	if (key >= 0x41 && key <= 0x5A)
		return KEY_A + (key - 0x41);
	if (key >= 0x30 && key <= 0x39)
		return KEY_0 + (key - 0x30);
	if (key >= VK_F1 && key <= VK_F24)
		return KEY_F1 + (key - VK_F1);
	if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9)
		return KEY_NP_0 + (key - VK_NUMPAD0);
	switch (key) {
	case VK_ESCAPE: return KEY_ESCAPE;
	case VK_SPACE: return KEY_SPACE;
	case VK_RETURN: return KEY_RETURN;
	case VK_UP: return KEY_UP;
	case VK_DOWN: return KEY_DOWN;
	case VK_LEFT: return KEY_LEFT;
	case VK_RIGHT: return KEY_RIGHT;
	case VK_BACK: return KEY_BACKSPACE;
	case VK_ADD: return KEY_NP_PLUS;
	case VK_SUBTRACT: return KEY_NP_MINUS;
	case VK_DECIMAL: return KEY_NP_DECIMAL;
	case VK_DIVIDE: return KEY_NP_DIVIDE;
	case VK_MULTIPLY: return KEY_NP_MULTIPLY;
	case VK_NUMLOCK: return KEY_NP_NUMLOCK;
	case VK_TAB: return KEY_TAB;
	case VK_CAPITAL: return KEY_CAPSLOCK;
	case VK_LSHIFT: return KEY_LSHIFT;
	case VK_RSHIFT: return KEY_RSHIFT;
	case VK_LCONTROL: return KEY_LCTRL;
	case VK_RCONTROL: return KEY_RCTRL;
	case VK_LMENU: return KEY_LALT;
	case VK_RMENU: return KEY_RALT;
	case VK_LWIN: return KEY_LMETA;
	case VK_RWIN: return KEY_RMETA;
	case VK_INSERT: return KEY_INSERT;
	case VK_DELETE: return KEY_DELETE;
	case VK_HOME: return KEY_HOME;
	case VK_END: return KEY_END;
	case VK_PRIOR: return KEY_PAGEUP;
	case VK_NEXT: return KEY_PAGEDOWN;
	case VK_SNAPSHOT: return KEY_PRINTSCREEN;
	case VK_PAUSE: return KEY_PAUSE;
	case VK_OEM_PLUS: return KEY_PLUS;
	case VK_OEM_COMMA: return KEY_COMMA;
	case VK_OEM_MINUS: return KEY_MINUS;
	case VK_OEM_PERIOD: return KEY_PERIOD;

	case 0xDC: return KEY_GRAVEACCENT;

	default: break;
	}
	return KEY_UNKNOWN;
}

int
input_module_initialize_native(void) {
	POINT pt;
	if (GetCursorPos(&pt)) {
		mouse_x = pt.x;
		mouse_y = pt.y;
	}
	memset(keystate_prev, 0, sizeof(unsigned char) * 256);
	return 0;
}

void
input_module_finalize_native(void) {
}

void
input_event_process(void) {
	GetKeyboardState(keystate_next);
	if (keystate_next[VK_RMENU] & 0x80) {
		// Hack to fix keyboards where left-control is set when right-alt is pressed
		if (!(keystate_prev[VK_LCONTROL] & 0x80))
			keystate_next[VK_LCONTROL] &= 0x7F;
	}
	for (int i = 0; i < 256; ++i) {
		if ((i == VK_MENU) ||
		        (i == VK_SHIFT) ||
		        (i == VK_CONTROL) ||
		        (i == VK_LBUTTON) ||
		        (i == VK_MBUTTON) ||
		        (i == VK_RBUTTON))
			continue;
		if ((keystate_next[i] & 0x80) && !(keystate_prev[i] & 0x80))
			input_event_post_key(INPUTEVENT_KEYDOWN, translate_key(i), i);
		else if (!(keystate_next[i] & 0x80) && (keystate_prev[i] & 0x80))
			input_event_post_key(INPUTEVENT_KEYUP, translate_key(i), i);
	}
	memcpy(keystate_prev, keystate_next, sizeof(unsigned char) * 256);

	POINT pt;
	if (last_hwnd && GetCursorPos(&pt)) {
		if (ScreenToClient(last_hwnd, &pt)) {
			/*if( _p_window->isCursorLocked() )
			{
				gui::Point size = _p_window->getSize();

				lastX = size.x / 2;
				lastY = size.y / 2;

				_p_window->setCursorPos( lastX, lastY );
			}*/

			if ((pt.x != mouse_x) || (pt.y != mouse_y))
				input_event_post_mouse(INPUTEVENT_MOUSEMOVE, pt.x, pt.y, (real)((int)pt.x - mouse_x),
				                       (real)((int)pt.y - mouse_y), 0, 0, mouse_buttons);

			mouse_x = pt.x;
			mouse_y = pt.y;
		}
	}
}

void
input_event_handle_window(event_t* event) {
	//Extract data from native message
	if (event->id != WINDOWEVENT_NATIVE)
		return;

	struct {
		window_t* window;
		HWND hwnd;
		unsigned int msg;
		uintptr_t wparam;
		uintptr_t lparam;
	} *data = (void*)event->payload;

	//TODO: Proper hwnd management
	if (data->hwnd)
		last_hwnd = data->hwnd;

	static POINT mouse_down[8];
	static time_t mouse_down_time[8];

	int x, y;
	int dx, dy, dz;

	switch (data->msg) {
	//********* MOUSE **********//
	case WM_LBUTTONDOWN:
		mouse_buttons |= MOUSEBUTTON_LEFT;
		mouse_down[MOUSEBUTTON_LEFT].x = (int)(short)LOWORD(data->lparam);
		mouse_down[MOUSEBUTTON_LEFT].y = (int)(short)HIWORD(data->lparam);
		mouse_down_time[MOUSEBUTTON_LEFT] = time_current();
		input_event_post_mouse(INPUTEVENT_MOUSEDOWN, mouse_down[MOUSEBUTTON_LEFT].x,
		                       mouse_down[MOUSEBUTTON_LEFT].y, 0, 0, 0, MOUSEBUTTON_LEFT, mouse_buttons);
		break;

	case WM_LBUTTONUP:
		mouse_buttons &= ~MOUSEBUTTON_LEFT;
		x = LOWORD(data->lparam);
		y = HIWORD(data->lparam);
		dx = x - mouse_down[MOUSEBUTTON_LEFT].x;
		dy = y - mouse_down[MOUSEBUTTON_LEFT].y;
		input_event_post_mouse(INPUTEVENT_MOUSEUP, x, y, (real)dx, (real)dy,
		                       time_elapsed(mouse_down_time[MOUSEBUTTON_LEFT]), MOUSEBUTTON_LEFT, mouse_buttons);
		break;

	case WM_RBUTTONDOWN:
		mouse_buttons |= MOUSEBUTTON_RIGHT;
		mouse_down[MOUSEBUTTON_RIGHT].x = LOWORD(data->lparam);
		mouse_down[MOUSEBUTTON_RIGHT].y = HIWORD(data->lparam);
		mouse_down_time[MOUSEBUTTON_RIGHT] = time_current();
		input_event_post_mouse(INPUTEVENT_MOUSEDOWN, mouse_down[MOUSEBUTTON_RIGHT].x,
		                       mouse_down[MOUSEBUTTON_RIGHT].y, 0, 0, 0, MOUSEBUTTON_RIGHT, mouse_buttons);
		break;

	case WM_RBUTTONUP:
		mouse_buttons &= ~MOUSEBUTTON_RIGHT;
		x = LOWORD(data->lparam);
		y = HIWORD(data->lparam);
		dx = x - mouse_down[MOUSEBUTTON_RIGHT].x;
		dy = y - mouse_down[MOUSEBUTTON_RIGHT].y;
		input_event_post_mouse(INPUTEVENT_MOUSEUP, x, y, (real)dx, (real)dy,
		                       time_elapsed(mouse_down_time[MOUSEBUTTON_RIGHT]), MOUSEBUTTON_RIGHT, mouse_buttons);
		break;

	case WM_MBUTTONDOWN:
		mouse_buttons |= MOUSEBUTTON_MIDDLE;
		mouse_down[MOUSEBUTTON_MIDDLE].x = (int)(short)LOWORD(data->lparam);
		mouse_down[MOUSEBUTTON_MIDDLE].y = (int)(short)HIWORD(data->lparam);
		mouse_down_time[MOUSEBUTTON_MIDDLE] = time_current();
		input_event_post_mouse(INPUTEVENT_MOUSEDOWN, mouse_down[MOUSEBUTTON_MIDDLE].x,
		                       mouse_down[MOUSEBUTTON_MIDDLE].y, 0, 0, 0, MOUSEBUTTON_MIDDLE, mouse_buttons);
		break;

	case WM_MBUTTONUP:
		mouse_buttons &= ~MOUSEBUTTON_MIDDLE;
		x = LOWORD(data->lparam);
		y = HIWORD(data->lparam);
		dx = x - mouse_down[MOUSEBUTTON_MIDDLE].x;
		dy = y - mouse_down[MOUSEBUTTON_MIDDLE].y;
		input_event_post_mouse(INPUTEVENT_MOUSEUP, x, y, (real)dx, (real)dy,
		                       time_elapsed(mouse_down_time[MOUSEBUTTON_MIDDLE]), MOUSEBUTTON_MIDDLE, mouse_buttons);
		break;

	case WM_MOUSEWHEEL:
		dz = GET_WHEEL_DELTA_WPARAM(data->wparam);
		if (dz != 0) {
			POINT pt;
			if (GetCursorPos(&pt))
				input_event_post_mouse(INPUTEVENT_MOUSEMOVE, pt.x, pt.y, 0, 0, (real)dz / (real)WHEEL_DELTA, 0,
				                       mouse_buttons);
		}
		break;

	//********* TEXT **********//
	case WM_CHAR:
		if (!unichar) {
			if (data->wparam < 0xFFFF) {
				unsigned int keycode = (unsigned int)data->wparam;
				if (keycode == 13)
					keycode = 10;
				input_event_post_key(INPUTEVENT_CHAR, keycode, ((unsigned int)data->lparam >> 16) & 0xFF);
			}
			else
				log_warnf(HASH_INPUT, WARNING_UNSUPPORTED, "NOT IMPLEMENTED: Got WM_CHAR with wparam 0x%llx",
				          (uintptr_t)data->wparam);
		}
		break;

	case WM_UNICHAR:
		if (data->wparam == UNICODE_NOCHAR)
			unichar = true;
		if (unichar)
			input_event_post_key(INPUTEVENT_CHAR, (unsigned int)data->wparam, (unsigned int)(data->lparam >> 16) & 0xFF);
		break;

	case WM_KEYDOWN:
		if (data->wparam == VK_DELETE)
			input_event_post_key(INPUTEVENT_CHAR, 0x7F, 0);
		break;

	default:
		break;
	}
}

#endif
