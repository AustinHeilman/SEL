#ifndef __SCREEN_H
#define __SCREEN_H

/* Screen output controller
 * Last Update: 8/1/2007
 * Austin Heilman
 *
 * Notes:
 * Shorts used in this because no number exceeds 255 (0xFF)
 */

#include <disp.h>
#include "apvector.h"

typedef apvector<short> ScrnBfr;

enum DispColors
{
	BLACK, BLUE, GREEN, CYAN,
	RED, MAGENTA, BROWN, GREY,
	// Below colors will cause blinking in DOS if used as background colors
	DARKGREY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN,
	LIGHTRED, LIGHTMAGENTA, LIGHTYELLOW, WHITE
};

enum BoxTypes
{
	DOUBLE_LINE, // Double lines all the way around
	SINGLE_LINE, // Single line all the way around
	SOLID_LINE,  // Solid colored box all the way around
	DBL_H_SNG_V, // Double line for horizontal, single for vertical.
	SNG_H_DBL_V, // Single line for horizontal, double for vertical.
};

class Screen
{
	public:
		Screen();
		~Screen();

		void DisplayTest();

		void WordWrap(bool status);

		short GetX();	// Return current column
		short GetY();	// Return current row
		void GotoXY(short x, short y);	// Place cursor at X,Y
		void Cls(short color);	// Clears screen with current background color
		void Cll(short color);	// Clears current line
		void Cll(short line, short color);	// Clears specified line
		void SetTextColor(short color); // Sets text color
		void SetBackgroundColor(short color); // Sets background color
		short GetTextColor() { return _text_color; }
		short GetBackgroundColor() { return _bgnd_color; }
		void DrawBorder(short type, short fg_color, short bg_color, short col, short row, short rcol, short brow);
		void FillBox(unsigned attr, short col, short row, short rcol, short brow);
		void DrawBox(short type, short fg_color, short bg_color, short col, short row, short rcol, short brow);
		void SetCursorType(short type);
		void PeekBox(ScrnBfr &bfr, unsigned x1, unsigned y1, unsigned x2, unsigned y2);
		void PokeBox(ScrnBfr &bfr, unsigned x1, unsigned y1, unsigned x2, unsigned y2);

		//template <class SCROS> Screen & operator << ( SCROS t );

	protected:
		short _text_color, _bgnd_color;
};

/*
// Allows you to use the class as you would "cout <<"
template <class SCROS>
Screen& Screen::operator <<( SCROS t )
{
	disp_puts("    ");
	cout << t;
	return *this;
}
*/
#endif // __SCREEN_H
