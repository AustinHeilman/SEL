#include "conio.h"
#include "screen.h"
#include "keyboard.h"

int main()
{
	// Clear the input buffer
	while ( kbhit() ) { getch(); }

	Screen scr;
	scr.WordWrap(false);
	scr.Cls(BLUE);
	scr.Cll(0, GREY);
	scr.Cll(24, GREY);
	scr.SetTextColor(RED);
	scr.SetBackgroundColor(GREY);
	disp_puts("Press 'Ctrl-C' to quit");

	scr.SetTextColor(WHITE);
	scr.SetBackgroundColor(BLUE);
	KeyInput input;
	while ( 1 )
	{
		input = Getch();

		scr.Cll(3, BLUE);
		scr.GotoXY(0, 3);
		cout << "ShiftStatus()   " << input.ShiftStatus() << endl;

		scr.Cll(4, BLUE);
		scr.GotoXY(0, 4);
		cout << "IsAscii()   " << input.IsASCII() << endl;

		scr.Cll(5, BLUE);
		scr.GotoXY(0, 5);
		cout << "ASCII Val   " << int(input.ASCIIValue()) << "   -   " << input.ASCIIValue() << endl;

		scr.Cll(6, BLUE);
		scr.GotoXY(0, 6);
		cout << "Raw Input   " << input.RawInput() << endl;

		scr.Cll(9, BLUE);
		scr.GotoXY(0, 9);
		cout << "Testing      " << (input.RawInput()) << endl;

		if ( input.RawInput() == 11779 )
			break;
	}
}
