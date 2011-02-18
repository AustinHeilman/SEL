#include <bios.h>
#include <time.h>
#include "keyboard.h"

KeyInput::KeyInput(int input, int xshift)
{
	_input = input;
	_shiftstatus = xshift;
}

bool KeyInput::IsASCII()
{
	return ( (_input & 0xFF) > 0 );
}

char KeyInput::ASCIIValue()
{
	int ascii = _input & 0xFF;
	if ( ascii > 0 )
		return ascii;
	else
		return _input-0x1E00;
}

int KeyInput::ShiftStatus()
{
	return _shiftstatus;
}

int KeyInput::RawInput()
{
	return _input;
}

bool KeyInput::operator==(KeyInput &rhs)
{
	return ( RawInput() == rhs.RawInput() );
}

bool KeyInput::operator!=(KeyInput &rhs)
{
	return !(*this==rhs);
}


KeyInput Getch()
{
	// Wait until a key is available
	while ( !_bios_keybrd(_KEYBRD_READY) ) {usleep(1);}

	int xshift = _bios_keybrd(_KEYBRD_SHIFTSTATUS);
	int input = _bios_keybrd(_KEYBRD_READ);

	return KeyInput(input, xshift);
}

bool IsKeyDown(KeyDown key_flag)
{
	return (_bios_keybrd(_KEYBRD_SHIFTSTATUS) & key_flag);
}

bool KBHit()
{
	return (_bios_keybrd(_KEYBRD_READY) != 0);
}
