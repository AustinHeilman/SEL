#include "screen.h"

Screen::Screen()
{
	_text_color = GREY;
	_bgnd_color = BLACK;
	disp_open();

	SetTextColor(_text_color);
	SetBackgroundColor(_bgnd_color);

	disp_setcursortype(DISP_CURSORUL);
	disp_hidecursor();

	WordWrap(false);
	GotoXY(0, 0);
}

Screen::~Screen()
{
	disp_flush();
	GotoXY(0, 0);
	SetTextColor(GREY);
	SetBackgroundColor(BLACK);
	Cls(BLACK);
	disp_close();
}

void Screen::WordWrap(bool enable)
{
	// 0 = No wrap
	// 1 = Wrap
	disp_nowrap = !enable;
}

void Screen::DisplayTest()
{
	int attchar, segment, i, row, col;

	Cls(BLACK);
	disp_printf("\r\n0123456789ABCDEF  0123456789ABCDEF\n");
	i = 0;
	for (row = 0; row < 16; row++)
	{
		for (col = 0; col < 16; col++)
		{
			attchar = 0x0700 + i;
			disp_pokew(row + 2, col, attchar);
			attchar = 0x0041 + i * 256;
			disp_pokew(row + 2, col + 18, attchar);
			i++;
		}
	}
	for (i = 0; i < 16; i++)
	disp_printf("\n");
}

void Screen::GotoXY(short x, short y)
{
	disp_move(y, x);
	disp_flush();
}

short Screen::GetX()
{
	// Global from disp_open()
	return disp_cursorcol;
}

short Screen::GetY()
{
	// Global from disp_open()
	return disp_cursorrow;
}

void Screen::SetTextColor(short color)
{
	_text_color = color;
	disp_setattr((_bgnd_color<<4) | _text_color);
	//disp_setattr(_text_color+(_bgnd_color*16));
}

void Screen::SetBackgroundColor(short color)
{
	_bgnd_color = color;
	disp_setattr((_bgnd_color<<4) | _text_color);
	//disp_setattr(_text_color+(_bgnd_color*16));
}

void Screen::Cls(short color)
{
	disp_setattr((color<<4 | _text_color));
	GotoXY(0, 0);
	disp_eeop();
}

void Screen::Cll(short color)
{
	Cll(GetY(), color);
}

void Screen::Cll(short line, short color)
{
	disp_setattr((color<<4 | _text_color));
	GotoXY(0, line);
	disp_eeol();
	disp_setattr((_bgnd_color<<4) | _text_color);
}

void Screen::DrawBorder(short type, short fg_color, short bg_color, short col, short row, short rcol, short brow)
{
	disp_box(type, (bg_color<<4 | fg_color), row, col, brow, rcol);
}

void Screen::FillBox(unsigned color, short col, short row, short rcol, short brow)
{
	disp_fillbox((color<<4 | color)*256+32, row, col, brow, rcol);
}

void Screen::DrawBox(short type, short fg_color, short bg_color, short col, short row, short rcol, short brow)
{
	FillBox(bg_color, col, row, rcol, brow);
	DrawBorder(type, fg_color, bg_color, col, row, rcol, brow);
}

void Screen::SetCursorType(short type)
{
	disp_setcursortype(type);
}

void Screen::PeekBox(ScrnBfr &bfr, unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
	bfr.resize(0);
	for ( unsigned row=x1; row <= x2; row++ )
	{
		for ( unsigned col=y1; col <= y2; col++ )
		{
			bfr.append(disp_peekw(col, row));
		}
	}
}

void Screen::PokeBox(ScrnBfr &bfr, unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
	unsigned long count = 0;
	for ( unsigned row=x1; row <= x2; row++ )
	{
		for ( unsigned col=y1; col <= y2; col++ )
		{
			if ( count > bfr.size() )
				continue; // Dont walk off the array
			disp_pokew(col, row, bfr[count]);
			count++;
		}
	}
}
