/* input_macos.c  -  Input library macOS implementation  -  Public Domain  -  2017 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_PLATFORM_MACOS

#include <CoreServices/CoreServices.h>
#include <Carbon/Carbon.h>

// Apple keycode constants, from Inside Macintosh
#define MK_ESCAPE               0x35
#define MK_F1                   0x7A
#define MK_F2                   0x78
#define MK_F3                   0x63
#define MK_F4                   0x76
#define MK_F5                   0x60
#define MK_F6                   0x61
#define MK_F7                   0x62
#define MK_F8                   0x64
#define MK_F9                   0x65
#define MK_F10                  0x6D
#define MK_F11                  0x67
#define MK_F12                  0x6F
#define MK_F13                  0x69
#define MK_F14                  0x6B
#define MK_F15                  0x71
#define MK_F16                  0x6A
#define MK_POWER                0x7F
#define MK_GRAVE                0x32
#define MK_1                    0x12
#define MK_2                    0x13
#define MK_3                    0x14
#define MK_4                    0x15
#define MK_5                    0x17
#define MK_6                    0x16
#define MK_7                    0x1A
#define MK_8                    0x1C
#define MK_9                    0x19
#define MK_0                    0x1D
#define MK_MINUS                0x1B
#define MK_EQUALS               0x18
#define MK_BACKSPACE            0x33
#define MK_HELP                 0x72
#define MK_HOME                 0x73
#define MK_PAGEUP               0x74
#define MK_NUMLOCK              0x47
#define MK_KP_EQUALS            0x51
#define MK_KP_DIVIDE            0x4B
#define MK_KP_MULTIPLY          0x43
#define MK_TAB                  0x30
#define MK_Q                    0x0C
#define MK_W                    0x0D
#define MK_E                    0x0E
#define MK_R                    0x0F
#define MK_T                    0x11
#define MK_Y                    0x10
#define MK_U                    0x20
#define MK_I                    0x22
#define MK_O                    0x1F
#define MK_P                    0x23
#define MK_LEFTBRACKET          0x21
#define MK_RIGHTBRACKET         0x1E
#define MK_BACKSLASH            0x2A
#define MK_DELETE               0x75
#define MK_END                  0x77
#define MK_PAGEDOWN             0x79
#define MK_KP7                  0x59
#define MK_KP8                  0x5B
#define MK_KP9                  0x5C
#define MK_KP_MINUS             0x4E
#define MK_CAPSLOCK             0x39
#define MK_A                    0x00
#define MK_S                    0x01
#define MK_D                    0x02
#define MK_F                    0x03
#define MK_G                    0x05
#define MK_H                    0x04
#define MK_J                    0x26
#define MK_K                    0x28
#define MK_L                    0x25
#define MK_SEMICOLON            0x29
#define MK_QUOTE                0x27
#define MK_RETURN               0x24
#define MK_KP4                  0x56
#define MK_KP5                  0x57
#define MK_KP6                  0x58
#define MK_KP_PLUS              0x45
#define MK_SHIFT                0x38
#define MK_Z                    0x06
#define MK_X                    0x07
#define MK_C                    0x08
#define MK_V                    0x09
#define MK_B                    0x0B
#define MK_N                    0x2D
#define MK_M                    0x2E
#define MK_COMMA                0x2B
#define MK_PERIOD               0x2F
#define MK_SLASH                0x2C
#define MK_UP                   0x7E
#define MK_KP1                  0x53
#define MK_KP2                  0x54
#define MK_KP3                  0x55
#define MK_KP_ENTER             0x4C
#define MK_CTRL                 0x3B
#define MK_ALT                  0x3A
#define MK_APPLE                0x37
#define MK_SPACE                0x31
#define MK_LEFT                 0x7B
#define MK_DOWN                 0x7D
#define MK_RIGHT                0x7C
#define MK_KP0                  0x52
#define MK_KP_PERIOD            0x41
/*#define MK_IBOOK_ENTER          0x34
 #define MK_IBOOK_LEFT           0x3B
 #define MK_IBOOK_RIGHT          0x3C
 #define MK_IBOOK_DOWN           0x3D
 #define MK_IBOOK_UP             0x3E*/
#define MK_MACBOOK_FN           0x3F


static bool                     _keydown[256];
void*                           _uchr;
uint32_t                        _deadkeys;
void*                           _key_event_source;
uint32_t                        _keytranslator[0x200] = {0};


static unsigned int translate_key( unsigned int key, unsigned int modifiers )
{
	if( !_uchr )
		return key;
	
	UniCharCount len = 0;
	UniChar character[2];
	UInt32 dead = _deadkeys;
	UCKeyTranslate( (const UCKeyboardLayout*)_uchr, key, kUCKeyActionDown, modifiers, CGEventSourceGetKeyboardType( (CGEventSourceRef)_key_event_source ), 0, &dead, 1, &len, character );
	_deadkeys = dead;
	return character[0];
	//return( KeyTranslate( _p_kchr, ( key & 0x7F ) | modifiers, (UInt32*)&_deadkeys ) );
}


static void _build_translator_table( void )
{
	//Default to a standard US keyboard
	_keytranslator[ MK_ESCAPE ]      = KEY_ESCAPE;
	_keytranslator[ MK_F1 ]          = KEY_F1;
	_keytranslator[ MK_F2 ]          = KEY_F2;
	_keytranslator[ MK_F3 ]          = KEY_F3;
	_keytranslator[ MK_F4 ]          = KEY_F4;
	_keytranslator[ MK_F5 ]          = KEY_F5;
	_keytranslator[ MK_F6 ]          = KEY_F6;
	_keytranslator[ MK_F7 ]          = KEY_F7;
	_keytranslator[ MK_F8 ]          = KEY_F8;
	_keytranslator[ MK_F9 ]          = KEY_F9;
	_keytranslator[ MK_F10 ]         = KEY_F10;
	_keytranslator[ MK_F11 ]         = KEY_F11;
	_keytranslator[ MK_F12 ]         = KEY_F12;
	_keytranslator[ MK_F13 ]         = KEY_F13;
	_keytranslator[ MK_F14 ]         = KEY_F14;
	_keytranslator[ MK_F15 ]         = KEY_F15;
	_keytranslator[ MK_F16 ]         = KEY_F16;
	_keytranslator[ MK_POWER ]       = KEY_UNKNOWN;
	_keytranslator[ MK_GRAVE ]       = KEY_GRAVEACCENT;
	_keytranslator[ MK_1 ]           = KEY_0;
	_keytranslator[ MK_2 ]           = KEY_1;
	_keytranslator[ MK_3 ]           = KEY_2;
	_keytranslator[ MK_4 ]           = KEY_3;
	_keytranslator[ MK_5 ]           = KEY_4;
	_keytranslator[ MK_6 ]           = KEY_5;
	_keytranslator[ MK_7 ]           = KEY_6;
	_keytranslator[ MK_8 ]           = KEY_7;
	_keytranslator[ MK_9 ]           = KEY_8;
	_keytranslator[ MK_0 ]           = KEY_9;
	_keytranslator[ MK_MINUS ]       = KEY_MINUS;
	_keytranslator[ MK_EQUALS ]      = KEY_EQUAL;
	_keytranslator[ MK_BACKSPACE ]   = KEY_BACKSPACE;
	_keytranslator[ MK_HELP ]        = KEY_UNKNOWN;
	_keytranslator[ MK_HOME ]        = KEY_HOME;
	_keytranslator[ MK_PAGEUP ]      = KEY_PAGEUP;
	_keytranslator[ MK_NUMLOCK ]     = KEY_NP_NUMLOCK;
	_keytranslator[ MK_KP_EQUALS ]   = KEY_NP_EQUAL;
	_keytranslator[ MK_KP_DIVIDE ]   = KEY_NP_DIVIDE;
	_keytranslator[ MK_KP_MULTIPLY ] = KEY_NP_MULTIPLY;
	_keytranslator[ MK_TAB ]         = KEY_TAB;
	_keytranslator[ MK_Q ]           = KEY_Q;
	_keytranslator[ MK_W ]           = KEY_W;
	_keytranslator[ MK_E ]           = KEY_E;
	_keytranslator[ MK_R ]           = KEY_R;
	_keytranslator[ MK_T ]           = KEY_T;
	_keytranslator[ MK_Y ]           = KEY_Y;
	_keytranslator[ MK_U ]           = KEY_U;
	_keytranslator[ MK_I ]           = KEY_I;
	_keytranslator[ MK_O ]           = KEY_O;
	_keytranslator[ MK_P ]           = KEY_P;
	_keytranslator[ MK_LEFTBRACKET ] = KEY_LEFTBRACKET;
	_keytranslator[ MK_RIGHTBRACKET ] = KEY_RIGHTBRACKET;
	_keytranslator[ MK_BACKSLASH ]   = KEY_BACKSLASH;
	_keytranslator[ MK_DELETE ]      = KEY_DELETE;
	_keytranslator[ MK_END ]         = KEY_END;
	_keytranslator[ MK_PAGEDOWN ]    = KEY_PAGEDOWN;
	_keytranslator[ MK_KP7 ]         = KEY_NP_7;
	_keytranslator[ MK_KP8 ]         = KEY_NP_8;
	_keytranslator[ MK_KP9 ]         = KEY_NP_9;
	_keytranslator[ MK_KP_MINUS ]    = KEY_NP_MINUS;
	_keytranslator[ MK_CAPSLOCK ]    = KEY_CAPSLOCK;
	_keytranslator[ MK_A ]           = KEY_A;
	_keytranslator[ MK_S ]           = KEY_S;
	_keytranslator[ MK_D ]           = KEY_D;
	_keytranslator[ MK_F ]           = KEY_F;
	_keytranslator[ MK_G ]           = KEY_G;
	_keytranslator[ MK_H ]           = KEY_H;
	_keytranslator[ MK_J ]           = KEY_J;
	_keytranslator[ MK_K ]           = KEY_K;
	_keytranslator[ MK_L ]           = KEY_L;
	_keytranslator[ MK_SEMICOLON ]   = KEY_SEMICOLON;
	_keytranslator[ MK_QUOTE ]       = KEY_QUOTE;
	_keytranslator[ MK_RETURN ]      = KEY_RETURN;
	_keytranslator[ MK_KP4 ]         = KEY_NP_4;
	_keytranslator[ MK_KP5 ]         = KEY_NP_5;
	_keytranslator[ MK_KP6 ]         = KEY_NP_6;
	_keytranslator[ MK_KP_PLUS ]     = KEY_NP_PLUS;
	_keytranslator[ MK_SHIFT ]       = KEY_LSHIFT;
	_keytranslator[ MK_Z ]           = KEY_Z;
	_keytranslator[ MK_X ]           = KEY_X;
	_keytranslator[ MK_C ]           = KEY_C;
	_keytranslator[ MK_V ]           = KEY_V;
	_keytranslator[ MK_B ]           = KEY_B;
	_keytranslator[ MK_N ]           = KEY_N;
	_keytranslator[ MK_M ]           = KEY_M;
	_keytranslator[ MK_COMMA ]       = KEY_COMMA;
	_keytranslator[ MK_PERIOD ]      = KEY_PERIOD;
	_keytranslator[ MK_SLASH ]       = KEY_SLASH;
	_keytranslator[ MK_UP ]          = KEY_UP;
	_keytranslator[ MK_KP1 ]         = KEY_NP_1;
	_keytranslator[ MK_KP2 ]         = KEY_NP_2;
	_keytranslator[ MK_KP3 ]         = KEY_NP_3;
	_keytranslator[ MK_KP_ENTER ]    = KEY_NP_ENTER;
	_keytranslator[ MK_CTRL ]        = KEY_LCTRL;
	_keytranslator[ MK_ALT ]         = KEY_LALT;
	_keytranslator[ MK_APPLE ]       = KEY_LMETA;
	_keytranslator[ MK_SPACE ]       = KEY_SPACE;
	_keytranslator[ MK_LEFT ]        = KEY_LEFT;
	_keytranslator[ MK_DOWN ]        = KEY_DOWN;
	_keytranslator[ MK_RIGHT ]       = KEY_RIGHT;
	_keytranslator[ MK_KP0 ]         = KEY_NP_0;
	_keytranslator[ MK_KP_PERIOD ]   = KEY_NP_DECIMAL;
	/*_keytranslator[ MK_IBOOK_ENTER ] = KEY_RETURN;
	 _keytranslator[ MK_IBOOK_LEFT ]  = KEY_LEFT;
	 _keytranslator[ MK_IBOOK_RIGHT ] = KEY_RIGHT;
	 _keytranslator[ MK_IBOOK_DOWN ]  = KEY_DOWN;
	 _keytranslator[ MK_IBOOK_UP ]    = KEY_UP;*/
	_keytranslator[ MK_MACBOOK_FN ]  = KEY_FN;
	
	//Now lookup keysyms using the current KCHR resource
	for( int i = 0; i < 0x200; ++i )
	{
		_deadkeys = 0;
		unsigned int key = translate_key( i, 0 ) & 0xFF;
		if( !key )
		{
			//Dead key, repeat
			key = translate_key( i, 0 ) & 0xFF;
		}
		
		if( key >= 127 ) //Non-ASCII char
		{
			if( key > KEY_UNKNOWN )
				_keytranslator[ i ] = KEY_UNKNOWN; //Unknown unicode?
			else
				_keytranslator[ i ] = KEY_NONASCIIKEYCODE | ( key & KEY_UNKNOWN ); //We have some non-US keys mapped...
		}
		else if( key >= 32 )  //ASCII char (not control char)
		{
			if( ( key >= 97 ) && ( key <= 122 ) )
				key -= 32; // lower -> upper case
			_keytranslator[ i ] = key;
		}
	}
	
	//Reinsert overwritten numpad mappings (maps to ascii number chars above)
	_keytranslator[ MK_NUMLOCK ]     = KEY_NP_NUMLOCK;
	_keytranslator[ MK_KP_EQUALS ]   = KEY_NP_EQUAL;
	_keytranslator[ MK_KP_DIVIDE ]   = KEY_NP_DIVIDE;
	_keytranslator[ MK_KP_MULTIPLY ] = KEY_NP_MULTIPLY;
	_keytranslator[ MK_KP7 ]         = KEY_NP_7;
	_keytranslator[ MK_KP8 ]         = KEY_NP_8;
	_keytranslator[ MK_KP9 ]         = KEY_NP_9;
	_keytranslator[ MK_KP_MINUS ]    = KEY_NP_MINUS;
	_keytranslator[ MK_KP4 ]         = KEY_NP_4;
	_keytranslator[ MK_KP5 ]         = KEY_NP_5;
	_keytranslator[ MK_KP6 ]         = KEY_NP_6;
	_keytranslator[ MK_KP_PLUS ]     = KEY_NP_PLUS;
	_keytranslator[ MK_KP1 ]         = KEY_NP_1;
	_keytranslator[ MK_KP2 ]         = KEY_NP_2;
	_keytranslator[ MK_KP3 ]         = KEY_NP_3;
	_keytranslator[ MK_KP_ENTER ]    = KEY_NP_ENTER;
	_keytranslator[ MK_KP0 ]         = KEY_NP_0;
	_keytranslator[ MK_KP_PERIOD ]   = KEY_NP_DECIMAL;
}


void _input_service_initialize_native( void )
{
	memset( _keydown, 0, sizeof( bool ) * 256 );
	
	TISInputSourceRef current_keyboard = TISCopyCurrentKeyboardInputSource();
	CFDataRef uchr = (CFDataRef)TISGetInputSourceProperty( current_keyboard, kTISPropertyUnicodeKeyLayoutData );
	_uchr = ( uchr ? (UCKeyboardLayout*)CFDataGetBytePtr( uchr ) : 0 );
	_deadkeys = 0;
	_key_event_source = CGEventSourceCreate( kCGEventSourceStateCombinedSessionState );
	
	_build_translator_table();
}


void _input_service_poll_native( void )
{
	TISInputSourceRef current_keyboard = TISCopyCurrentKeyboardInputSource();
	CFDataRef uchr = (CFDataRef)TISGetInputSourceProperty( current_keyboard, kTISPropertyUnicodeKeyLayoutData );
	void* uchrptr = ( uchr ? (void*)CFDataGetBytePtr( uchr ) : 0 );
	
	if( uchrptr != _uchr )
	{
		_deadkeys = 0;
		_uchr = uchrptr;
		
		info_logf( "Switched UCHR resource" );
		_build_translator_table();
	}

	CGEventSourceStateID event_source = kCGEventSourceStateCombinedSessionState;
	
	//TODO: Look into using event taps instead, this is silly
	for( int i = 0; i < 256; ++i )
	{
		if( CGEventSourceKeyState( event_source, i ) )
		{
			if( !_keydown[i] )
			{
				unsigned int keycode = _keytranslator[ i ];
				if( !keycode )
					keycode = KEY_UNKNOWN;
				//printf( "DEBUG TRACE: Key %d down, translated to '%c'\n", i, (char)keycode );
				
				input_event_post_key( INPUTEVENT_KEYDOWN, keycode, i );
				
				_keydown[i] = true;
			}
		}
		else
		{
			if( _keydown[i] )
			{
				unsigned int keycode = _keytranslator[ i ];
				if( !keycode )
					keycode = KEY_UNKNOWN;
				//printf( "DEBUG TRACE: Key %d up, translated to '%c'\n", i, (char)keycode );
				
				input_event_post_key( INPUTEVENT_KEYUP, keycode, i );
				
				_keydown[i] = false;
			}
		}
	}
}


#endif
