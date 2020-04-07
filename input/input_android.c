/* input_android.h  -  Input library Android implementation  -  Public Domain  -  2017 Mattias Jansson
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

#if FOUNDATION_PLATFORM_ANDROID

#include "hashstrings.h"
#include "event.h"

#include "core/time.h"

#include "math/core.h"

#include "debug/log.h"

#include "thread/thread.h"

#include "app/bits/global.h"
#include "app/android/android_native_app_glue.h"

#include <android/native_activity.h>
#include <android/input.h>
#include <android/sensor.h>

#define NUM_TRANSLATED_KEYS 97

static key_id key_translator[NUM_TRANSLATED_KEYS] = {
    KEY_UNKNOWN,           // AKEYCODE_UNKNOWN         = 0,
    KEY_LEFT,              // AKEYCODE_SOFT_LEFT       = 1,
    KEY_RIGHT,             // AKEYCODE_SOFT_RIGHT      = 2,
    KEY_HOME,              // AKEYCODE_HOME            = 3,
    KEY_BACK,              // AKEYCODE_BACK            = 4,
    KEY_CALL,              // AKEYCODE_CALL            = 5,
    KEY_ENDCALL,           // AKEYCODE_ENDCALL         = 6,
    KEY_0,                 // AKEYCODE_0               = 7,
    KEY_1,                 // AKEYCODE_1               = 8,
    KEY_2,                 // AKEYCODE_2               = 9,
    KEY_3,                 // AKEYCODE_3               = 10,
    KEY_4,                 // AKEYCODE_4               = 11,
    KEY_5,                 // AKEYCODE_5               = 12,
    KEY_6,                 // AKEYCODE_6               = 13,
    KEY_7,                 // AKEYCODE_7               = 14,
    KEY_8,                 // AKEYCODE_8               = 15,
    KEY_9,                 // AKEYCODE_9               = 16,
    KEY_ASTERISK,          // AKEYCODE_STAR            = 17,
    KEY_POUND,             // AKEYCODE_POUND           = 18,
    KEY_UP,                // AKEYCODE_DPAD_UP         = 19,
    KEY_DOWN,              // AKEYCODE_DPAD_DOWN       = 20,
    KEY_LEFT,              // AKEYCODE_DPAD_LEFT       = 21,
    KEY_RIGHT,             // AKEYCODE_DPAD_RIGHT      = 22,
    KEY_CENTER,            // AKEYCODE_DPAD_CENTER     = 23,
    KEY_VOLUMEUP,          // AKEYCODE_VOLUME_UP       = 24,
    KEY_VOLUMEDOWN,        // AKEYCODE_VOLUME_DOWN     = 25,
    KEY_POWER,             // AKEYCODE_POWER           = 26,
    KEY_CAMERA,            // AKEYCODE_CAMERA          = 27,
    KEY_CLEAR,             // AKEYCODE_CLEAR           = 28,
    KEY_A,                 // AKEYCODE_A               = 29,
    KEY_B,                 // AKEYCODE_B               = 30,
    KEY_C,                 // AKEYCODE_C               = 31,
    KEY_D,                 // AKEYCODE_D               = 32,
    KEY_E,                 // AKEYCODE_E               = 33,
    KEY_F,                 // AKEYCODE_F               = 34,
    KEY_G,                 // AKEYCODE_G               = 35,
    KEY_H,                 // AKEYCODE_H               = 36,
    KEY_I,                 // AKEYCODE_I               = 37,
    KEY_J,                 // AKEYCODE_J               = 38,
    KEY_K,                 // AKEYCODE_K               = 39,
    KEY_L,                 // AKEYCODE_L               = 40,
    KEY_M,                 // AKEYCODE_M               = 41,
    KEY_N,                 // AKEYCODE_N               = 42,
    KEY_O,                 // AKEYCODE_O               = 43,
    KEY_P,                 // AKEYCODE_P               = 44,
    KEY_Q,                 // AKEYCODE_Q               = 45,
    KEY_R,                 // AKEYCODE_R               = 46,
    KEY_S,                 // AKEYCODE_S               = 47,
    KEY_T,                 // AKEYCODE_T               = 48,
    KEY_U,                 // AKEYCODE_U               = 49,
    KEY_V,                 // AKEYCODE_V               = 50,
    KEY_W,                 // AKEYCODE_W               = 51,
    KEY_X,                 // AKEYCODE_X               = 52,
    KEY_Y,                 // AKEYCODE_Y               = 53,
    KEY_Z,                 // AKEYCODE_Z               = 54,
    KEY_COMMA,             // AKEYCODE_COMMA           = 55,
    KEY_PERIOD,            // AKEYCODE_PERIOD          = 56,
    KEY_LEFT,              // AKEYCODE_ALT_LEFT        = 57,
    KEY_RIGHT,             // AKEYCODE_ALT_RIGHT       = 58,
    KEY_LEFT,              // AKEYCODE_SHIFT_LEFT      = 59,
    KEY_RIGHT,             // AKEYCODE_SHIFT_RIGHT     = 60,
    KEY_TAB,               // AKEYCODE_TAB             = 61,
    KEY_SPACE,             // AKEYCODE_SPACE           = 62,
    KEY_SYM,               // AKEYCODE_SYM             = 63,
    KEY_EXPLORER,          // AKEYCODE_EXPLORER        = 64,
    KEY_ENVELOPE,          // AKEYCODE_ENVELOPE        = 65,
    KEY_ENTER,             // AKEYCODE_ENTER           = 66,
    KEY_BACKSPACE,         // AKEYCODE_DEL             = 67,
    KEY_GRAVEACCENT,       // AKEYCODE_GRAVE           = 68,
    KEY_MINUS,             // AKEYCODE_MINUS           = 69,
    KEY_EQUAL,             // AKEYCODE_EQUALS          = 70,
    KEY_LEFTBRACKET,       // AKEYCODE_LEFT_BRACKET    = 71,
    KEY_RIGHTBRACKET,      // AKEYCODE_RIGHT_BRACKET   = 72,
    KEY_BACKSLASH,         // AKEYCODE_BACKSLASH       = 73,
    KEY_SEMICOLON,         // AKEYCODE_SEMICOLON       = 74,
    KEY_APOSTROPHE,        // AKEYCODE_APOSTROPHE      = 75,
    KEY_SLASH,             // AKEYCODE_SLASH           = 76,
    KEY_AT,                // AKEYCODE_AT              = 77,
    KEY_NUM,               // AKEYCODE_NUM             = 78,
    KEY_HEADSETHOOK,       // AKEYCODE_HEADSETHOOK     = 79,
    KEY_CAMERAFOCUS,       // AKEYCODE_FOCUS           = 80,   // *Camera* focus
    KEY_PLUS,              // AKEYCODE_PLUS            = 81,
    KEY_MENU,              // AKEYCODE_MENU            = 82,
    KEY_NOTIFICATION,      // AKEYCODE_NOTIFICATION    = 83,
    KEY_SEARCH,            // AKEYCODE_SEARCH          = 84,
    KEY_MEDIAPLAYPAUSE,    // AKEYCODE_MEDIA_PLAY_PAUSE= 85,
    KEY_MEDIASTOP,         // AKEYCODE_MEDIA_STOP      = 86,
    KEY_MEDIANEXT,         // AKEYCODE_MEDIA_NEXT      = 87,
    KEY_MEDIAPREVIOUS,     // AKEYCODE_MEDIA_PREVIOUS  = 88,
    KEY_MEDIAREWIND,       // AKEYCODE_MEDIA_REWIND    = 89,
    KEY_MEDIAFASTFORWARD,  // AKEYCODE_MEDIA_FAST_FORWARD = 90,
    KEY_MUTE,              // AKEYCODE_MUTE            = 91,
    KEY_PAGEUP,            // AKEYCODE_PAGE_UP         = 92,
    KEY_PAGEDOWN,          // AKEYCODE_PAGE_DOWN       = 93,
    KEY_PICTSYMBOLS,       // AKEYCODE_PICTSYMBOLS     = 94,
    KEY_SWITCHCHARSET,     // AKEYCODE_SWITCH_CHARSET  = 95,

    KEY_UNKNOWN
    /* Following should be handled by pad input
    AKEYCODE_BUTTON_A        = 96,
    AKEYCODE_BUTTON_B        = 97,
    AKEYCODE_BUTTON_C        = 98,
    AKEYCODE_BUTTON_X        = 99,
    AKEYCODE_BUTTON_Y        = 100,
    AKEYCODE_BUTTON_Z        = 101,
    AKEYCODE_BUTTON_L1       = 102,
    AKEYCODE_BUTTON_R1       = 103,
    AKEYCODE_BUTTON_L2       = 104,
    AKEYCODE_BUTTON_R2       = 105,
    AKEYCODE_BUTTON_THUMBL   = 106,
    AKEYCODE_BUTTON_THUMBR   = 107,
    AKEYCODE_BUTTON_START    = 108,
    AKEYCODE_BUTTON_SELECT   = 109,
    AKEYCODE_BUTTON_MODE     = 110,
    */
};

void
_input_service_initialize_native(void) {
}

void
_input_service_poll_native(void) {
}

static uint16_t
_keyevent_to_unicode(int64_t down_time, int64_t event_time, int32_t action, int32_t keycode, int32_t repeat,
                     int32_t metastate, int32_t device_id, int32_t scancode, int32_t flags, int32_t source) {
	thread_attach_jvm();

	JavaVM* java_vm = _global_app->activity->vm;
	JNIEnv* jni_env = _global_app->activity->env;

	static jclass class_keyevent = 0;
	static jmethodID method_ctor = 0;
	static jmethodID method_getunicodechar = 0;
	if (!method_getunicodechar) {
		class_keyevent = (*jni_env)->FindClass(jni_env, "android/view/KeyEvent");
		if (!class_keyevent) {
			warn_logf(WARNING_SYSTEM_CALL_FAIL, "Unable to get android Java KeyEvent class");
			return 0;
		}
		method_ctor = (*jni_env)->GetMethodID(jni_env, class_keyevent, "<init>", "(JJIIIIIIII)V");
		if (!method_ctor) {
			debug_logf("Unable to get Java KeyEvent ctor");
			return 0;
		}
		method_getunicodechar = (*jni_env)->GetMethodID(jni_env, class_keyevent, "getUnicodeChar", "()I");
		if (!method_getunicodechar) {
			debug_logf("Unable to get Java KeyEvent getUnicodeChar method");
			return 0;
		}
	}

	jobject keyevent = (*jni_env)->NewObject(jni_env, class_keyevent, method_ctor, down_time, event_time, action,
	                                         keycode, repeat, metastate, device_id, scancode, flags, source);
	if (!keyevent) {
		debug_logf("Unable to create Java KeyEvent object");
		return 0;
	}

	return (*jni_env)->CallIntMethod(jni_env, keyevent, method_getunicodechar);
}

int
android_sensor_callback(int fd, int events, void* data) {
	ASensorEvent eventbuffer[16];
	int num_events = ASensorEventQueue_getEvents(_global_sensor_queue, eventbuffer, 16);
	for (int i = 0; i < num_events; ++i) {
		ASensorEvent* sensor_event = eventbuffer + i;

		// Only one sensor which is accelerometer for now
		// debug_logf( "Sensor vector:  %.3f  %.3f  %.3f", sensor_event->acceleration.x, sensor_event->acceleration.y,
		// sensor_event->acceleration.z );
		input_event_post_acceleration(INPUTEVENT_ACCELERATION, sensor_event->acceleration.x,
		                              sensor_event->acceleration.y, sensor_event->acceleration.z);
	}
	return 1;
}

int32_t
android_handle_input(struct android_app* app, AInputEvent* event) {
	static int last_x[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
	static int last_y[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

	static int begin_x[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
	static int begin_y[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

	static time_t begin_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	static uint32_t current_fingers = 0;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		int32_t action = AMotionEvent_getAction(event);
		int32_t flags = AMotionEvent_getMetaState(event);
		size_t pointers = AMotionEvent_getPointerCount(event);

		// debug_logf( "Motion event: action=%d flags=0x%x pointers=%d", action, flags, pointers );
		for (size_t pointer = 0; pointer < pointers; ++pointer) {
			input_event_id id = 0;
			unsigned int finger = AMotionEvent_getPointerId(event, pointer);
			int x = 0, y = 0;
			real dx = 0, dy = 0;

			if (finger >= 8)
				continue;

			switch (action & AMOTION_EVENT_ACTION_MASK) {
				case AMOTION_EVENT_ACTION_DOWN: {
					current_fingers |= (1 << finger);
					id = INPUTEVENT_TOUCHBEGIN;
					break;
				}

				case AMOTION_EVENT_ACTION_CANCEL: {
					current_fingers &= ~(1 << finger);
					id = INPUTEVENT_TOUCHCANCEL;
					break;
				}

				case AMOTION_EVENT_ACTION_UP: {
					current_fingers &= ~(1 << finger);
					id = INPUTEVENT_TOUCHEND;
					break;
				}

				case AMOTION_EVENT_ACTION_MOVE: {
					id = INPUTEVENT_TOUCHMOVE;
					break;
				}

				default:
					continue;
			}

			x = AMotionEvent_getX(event, finger);
			y = AMotionEvent_getY(event, finger);

			if (id == INPUTEVENT_TOUCHBEGIN) {
				begin_x[finger] = x;
				begin_y[finger] = y;

				last_x[finger] = x;
				last_y[finger] = y;

				begin_time[finger] = time_current();
			}

			dx = x - last_x[finger];
			dy = y - last_y[finger];

			real dx_begin = x - begin_x[finger];
			real dy_begin = y - begin_y[finger];

			real velocity = 0;
			real t = time_elapsed(begin_time[finger]);
			if (t > 0)
				velocity = neo_sqrt(dx_begin * dx_begin + dy_begin * dy_begin) / t;

			last_x[finger] = x;
			last_y[finger] = y;

			if (id == INPUTEVENT_TOUCHCANCEL)
				dx = dy = t = velocity = 0;

			if (id == INPUTEVENT_TOUCHEND) {
				dx = dx_begin;
				dy = dy_begin;
			}

			input_event_post_touch(id, x, y, dx, dy, (id == INPUTEVENT_TOUCHEND) ? t : velocity, finger,
			                       current_fingers);

			if (id == INPUTEVENT_TOUCHEND)
				input_event_post_touch(INPUTEVENT_TOUCHSWIPE, x, y, dx, dy, velocity, 0, 0);
		}

		return 1;
	} else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
		int64_t down_time = AKeyEvent_getDownTime(event);
		int64_t event_time = AKeyEvent_getEventTime(event);
		int32_t action = AKeyEvent_getAction(event);
		int32_t keycode = AKeyEvent_getKeyCode(event);
		int32_t repeat = AKeyEvent_getRepeatCount(event);
		int32_t metastate = AKeyEvent_getMetaState(event);
		int32_t device_id = AInputEvent_getDeviceId(event);
		int32_t scancode = AKeyEvent_getScanCode(event);
		int32_t flags = AKeyEvent_getFlags(event);
		int32_t source = AInputEvent_getSource(event);

		info_logf("Key event: action=%d keycode=%d metastate=0x%x flags=%x", action, keycode, metastate, flags);

		if (keycode < NUM_TRANSLATED_KEYS) {
			input_event_post_key((action == AKEY_EVENT_ACTION_DOWN) ? INPUTEVENT_KEYDOWN : INPUTEVENT_KEYUP,
			                     key_translator[keycode], keycode);

			if (action == AKEY_EVENT_ACTION_UP) {
				uint16_t unicode_char = _keyevent_to_unicode(down_time, event_time, action, keycode, repeat, metastate,
				                                             device_id, scancode, flags, source);
				if (unicode_char) {
					// info_logf( "Input unicode char: %d", (int)unicode_char );
					input_event_post_key(INPUTEVENT_CHAR, unicode_char, keycode);
				}
			}
		}

		return 1;
	}
	return 0;
}

#endif
