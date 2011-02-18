#ifndef __KEYBOARD_H
#define __KEYBOARD_H


/* Keyboard Input Controller
 * Last Update: 8/23/2007
 * Austin Heilman
 *
 * Notes:
 */

enum ScanCode
{
	ESC_KEY		= 283,
	UP_KEY		= 18432,
	DOWN_KEY		= 20480,
	LEFT_KEY		= 19200,
	RIGHT_KEY		= 19712,
	ENTER_KEY		= 7181,
	BACKSPACE_KEY	= 3592,
	TAB_KEY		= 3849,

	F10_KEY		= 17408,
};

enum KeyDown
{
	KEYDWN_RSHIFT	= 0x1,
	KEYDWN_LSHIFT	= 0x2,
	KEYDWN_CTRL	= 0x4,
	KEYDWN_ALT	= 0x8,
	KEYDWN_SRLLCK	= 0x10,
	KEYDWN_NUMLCK	= 0x20,
	KEYDWN_CAPLCK	= 0x40,
	KEYDWN_INSERT	= 0x80,
};

struct KeyInput
{
	public:
		KeyInput(int input=0, int xshift=0);
		bool IsASCII();
		char ASCIIValue();
		int ShiftStatus();
		int RawInput();

		bool operator==(KeyInput &rhs);
		bool operator!=(KeyInput &rhs);

	private:
		int _input;
		int _shiftstatus;
};

KeyInput Getch();
bool IsKeyDown(KeyDown key_flag);
bool KBHit();

#endif // __KEYBOARD_H
