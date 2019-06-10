/* input_windows.c  -  Input library Windows implementation  -  Public Domain  -  2017 Mattias
 * Jansson / Rampant Pixels
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
 * This library is put in the public domain; you can redistribute it and/or modify it without any
 * restrictions.
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
#pragma warning(disable : 4996)
#endif
#undef ERROR

static int mouse_x;
static int mouse_y;
static unsigned int mouse_buttons;
static bool unichar = false;

static unsigned int
translate_key(unsigned int scancode, unsigned int key, unsigned int flags) {
	FOUNDATION_UNUSED(flags);
	if (key >= 0x41 && key <= 0x5A)
		return KEY_A + (key - 0x41);
	if (key >= 0x30 && key <= 0x39)
		return KEY_0 + (key - 0x30);
	if (key >= VK_F1 && key <= VK_F24)
		return KEY_F1 + (key - VK_F1);
	if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9)
		return KEY_NP_0 + (key - VK_NUMPAD0);
	switch (key) {
		case VK_ESCAPE:
			return KEY_ESCAPE;
		case VK_SPACE:
			return KEY_SPACE;
		case VK_RETURN:
			return KEY_RETURN;
		case VK_UP:
			return KEY_UP;
		case VK_DOWN:
			return KEY_DOWN;
		case VK_LEFT:
			return KEY_LEFT;
		case VK_RIGHT:
			return KEY_RIGHT;
		case VK_BACK:
			return KEY_BACKSPACE;
		case VK_ADD:
			return KEY_NP_PLUS;
		case VK_SUBTRACT:
			return KEY_NP_MINUS;
		case VK_DECIMAL:
			return KEY_NP_DECIMAL;
		case VK_DIVIDE:
			return KEY_NP_DIVIDE;
		case VK_MULTIPLY:
			return KEY_NP_MULTIPLY;
		case VK_NUMLOCK:
			return KEY_NP_NUMLOCK;
		case VK_TAB:
			return KEY_TAB;
		case VK_CAPITAL:
			return KEY_CAPSLOCK;
		case VK_SHIFT:
			if (scancode < 0x30)
				return KEY_LSHIFT;
			return KEY_RSHIFT;
		case VK_LSHIFT:
			return KEY_LSHIFT;
		case VK_RSHIFT:
			return KEY_RSHIFT;
		case VK_CONTROL:
			if (!(flags & 2))
				return KEY_LCTRL;
			return KEY_RCTRL;
		case VK_LCONTROL:
			return KEY_LCTRL;
		case VK_RCONTROL:
			return KEY_RCTRL;
		case VK_MENU:
			if (!(flags & 2))
				return KEY_LALT;
			return KEY_RALT;
		case VK_LMENU:
			return KEY_LALT;
		case VK_RMENU:
			return KEY_RALT;
		case VK_LWIN:
			return KEY_LMETA;
		case VK_RWIN:
			return KEY_RMETA;
		case VK_INSERT:
			return KEY_INSERT;
		case VK_DELETE:
			return KEY_DELETE;
		case VK_HOME:
			return KEY_HOME;
		case VK_END:
			return KEY_END;
		case VK_PRIOR:
			return KEY_PAGEUP;
		case VK_NEXT:
			return KEY_PAGEDOWN;
		case VK_SNAPSHOT:
			return KEY_PRINTSCREEN;
		case VK_PAUSE:
			return KEY_PAUSE;
		case VK_OEM_PLUS:
			return KEY_PLUS;
		case VK_OEM_COMMA:
			return KEY_COMMA;
		case VK_OEM_MINUS:
			return KEY_MINUS;
		case VK_OEM_PERIOD:
			return KEY_PERIOD;

		case 0xDC:
			return KEY_GRAVEACCENT;

		default:
			break;
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
	return 0;
}

void
input_module_finalize_native(void) {
}

void
input_event_process(void) {
}

void
input_mouse_current_client_point(HWND hwnd, POINT* pt) {
	if (hwnd && GetCursorPos(pt)) {
		if (ScreenToClient(hwnd, pt)) {
			/*if( _p_window->isCursorLocked() )
			{
			    gui::Point size = _p_window->getSize();
			    lastX = size.x / 2;
			    lastY = size.y / 2;
			    _p_window->setCursorPos( lastX, lastY );
			}*/
		}
	}
}

static POINT mouse_down[8];
static time_t mouse_down_time[8];

static int ri_down_flag[5][2] = {{RI_MOUSE_BUTTON_1_DOWN, MOUSEBUTTON_LEFT},
                                 {RI_MOUSE_BUTTON_2_DOWN, MOUSEBUTTON_RIGHT},
                                 {RI_MOUSE_BUTTON_3_DOWN, MOUSEBUTTON_MIDDLE},
                                 {RI_MOUSE_BUTTON_4_DOWN, MOUSEBUTTON_3},
                                 {RI_MOUSE_BUTTON_5_DOWN, MOUSEBUTTON_4}};

static int ri_up_flag[5][2] = {{RI_MOUSE_BUTTON_1_UP, MOUSEBUTTON_LEFT},
                               {RI_MOUSE_BUTTON_2_UP, MOUSEBUTTON_RIGHT},
                               {RI_MOUSE_BUTTON_3_UP, MOUSEBUTTON_MIDDLE},
                               {RI_MOUSE_BUTTON_4_UP, MOUSEBUTTON_3},
                               {RI_MOUSE_BUTTON_5_UP, MOUSEBUTTON_4}};

void
input_mouse_down(input_mouse_button_id button, int x, int y) {
	mouse_buttons |= button;
	mouse_down[button].x = x;
	mouse_down[button].y = y;
	mouse_down_time[button] = time_current();
	input_event_post_mouse(INPUTEVENT_MOUSEDOWN, mouse_down[button].x, mouse_down[button].y, 0, 0,
	                       0, button, mouse_buttons);
}

void
input_mouse_up(input_mouse_button_id button, int x, int y) {
	mouse_buttons &= ~button;
	int dx = x - mouse_down[button].x;
	int dy = y - mouse_down[button].y;
	input_event_post_mouse(INPUTEVENT_MOUSEUP, x, y, (real)dx, (real)dy,
	                       time_elapsed(mouse_down_time[button]), button, mouse_buttons);
}

void
input_event_handle_window(event_t* event) {
	// Extract data from native message
	if (event->id != WINDOWEVENT_NATIVE)
		return;

	struct payload_t {
		window_t* window;
		void* hwnd;
		uintptr_t msg;
		uintptr_t wparam;
		uintptr_t lparam;
		uintptr_t buffer[FOUNDATION_FLEXIBLE_ARRAY];
	};
	struct payload_t* data = (struct payload_t*)event->payload;
	size_t buffer_size = event->size - (sizeof(event_t) + sizeof(struct payload_t));

	switch (data->msg) {
		case WM_KILLFOCUS:
			for (unsigned int ibutton = 0; ibutton < 8; ++ibutton) {
				if (mouse_buttons & (1 << ibutton))
					input_mouse_up(1 << ibutton, mouse_x, mouse_y);
			}
			break;

		case WM_INPUT:
			if (buffer_size >= sizeof(RAWINPUTHEADER)) {
				RAWINPUT* raw = (RAWINPUT*)&data->buffer[0];
				if (raw->header.dwType == RIM_TYPEKEYBOARD) {
					//********* KEYBOARD **********//
					unsigned int scancode = raw->data.keyboard.MakeCode;
					unsigned int flags = raw->data.keyboard.Flags & ~1;
					unsigned int vkey = raw->data.keyboard.VKey;
					unsigned int key = translate_key(scancode, vkey, flags);
					if (raw->data.keyboard.Flags & RI_KEY_BREAK) {
						input_event_post_key(INPUTEVENT_KEYUP, key, scancode, flags);
						/*log_debugf(HASH_INPUT,
						           STRING_CONST("Key: %u up, scancode %x, flags %x, vkey %x"), key,
						           scancode, flags, vkey);*/
					} else {
						input_event_post_key(INPUTEVENT_KEYDOWN, key, scancode, flags);
						/*log_debugf(HASH_INPUT,
						           STRING_CONST("Key: %u down, scancode %x, flags %x, vkey %x"),
						           key, scancode, flags, vkey);*/
					}
				} else if (raw->header.dwType == RIM_TYPEMOUSE) {
					//********* MOUSE **********//
					POINT current = {mouse_x, mouse_y};
					input_mouse_current_client_point(data->hwnd, &current);
					bool had_event = false;
					for (int ibutton = 0; ibutton < 5; ++ibutton) {
						if (raw->data.mouse.usButtonFlags & ri_down_flag[ibutton][0]) {
							input_mouse_down(ri_down_flag[ibutton][1], (int)current.x,
							                 (int)current.y);
							had_event = true;
						}
						if (raw->data.mouse.usButtonFlags & ri_up_flag[ibutton][0]) {
							input_mouse_up(ri_up_flag[ibutton][1], (int)current.x, (int)current.y);
							had_event = true;
						}
					}

					int delta_x = (int)current.x - mouse_x;
					int delta_y = (int)current.y - mouse_y;
					int delta_z = 0;
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
						delta_z = (short)raw->data.mouse.usButtonData;
					}
					if (data->window->is_resizing) {
						delta_x = 0;
						delta_y = 0;
					}
					if (delta_x || delta_y || delta_z) {
						input_event_post_mouse(INPUTEVENT_MOUSEMOVE, current.x, current.y,
						                       (real)delta_x, (real)delta_y,
						                       (real)delta_z / (real)WHEEL_DELTA, 0, mouse_buttons);
						had_event = true;
					}
					/*if (had_event) {
					    log_debugf(HASH_INPUT,
					               STRING_CONST("Mouse: current=%d,%d delta=%d,%d,%d buttons=0x%x"),
					               (int)current.x, (int)current.y, delta_x, delta_y, delta_z,
					               mouse_buttons);
					}*/
					mouse_x = current.x;
					mouse_y = current.y;
				}
			}
			break;

		//********* TEXT **********//
		case WM_CHAR:
			if (!unichar) {
				if (data->wparam < 0xFFFF) {
					unsigned int keycode = (unsigned int)data->wparam;
					if (keycode == 13)
						keycode = 10;
					input_event_post_key(INPUTEVENT_CHAR, keycode,
					                     ((unsigned int)data->lparam >> 16) & 0xFF, 0);
				} else
					log_warnf(HASH_INPUT, WARNING_UNSUPPORTED,
					          "NOT IMPLEMENTED: Got WM_CHAR with wparam 0x%llx",
					          (uintptr_t)data->wparam);
			}
			break;

		case WM_UNICHAR:
			if (data->wparam == UNICODE_NOCHAR)
				unichar = true;
			if (unichar)
				input_event_post_key(INPUTEVENT_CHAR, (unsigned int)data->wparam,
				                     (unsigned int)(data->lparam >> 16) & 0xFF, 0);
			break;

		case WM_KEYDOWN:
			if (data->wparam == VK_DELETE)
				input_event_post_key(INPUTEVENT_CHAR, 0x7F, 0, 0);
			break;

		default:
			break;
	}
}

#endif
