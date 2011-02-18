#ifndef __TUI_H
#define __TUI_H

/* Text User Interface
 * Austin Heilman
 * Last Updated: 8/9/2007
 *
 */

#include "screen.h"
#include "apstring.h"
#include "keyboard.h"
#include "apvector.h"

enum TUIFlags
{
	TUI_SAVEBFR	= 0x1,  // Will save the screen buffer beneath the menu and restore it when removed.
	TUI_NOCLOSE 	= 0x2, // Does not automatically remove the object.
	TUI_NOESCAPE	= 0x4, // Does not allow the escape key to be used.

	// Menu Item Flags
	TUI_BARENTRY	= 0x8, // Draws a horizontal bar |-----| that matches the width.

	// Text Entry Flags
	TUI_READONLY	= 0x10, // Can't add/remove anything from field
	TUI_NUMONLY	= 0x20, // Only allow 0-9 to be entered
	TUI_ABCONLY	= 0x40, // Only allow A-Z
	TUI_FORCEUPPER	= 0x80, // Make all letters capital
	TUI_FORCELOWER = 0x100, // Make all letters lower-case
	TUI_HEXONLY	= 0x200, // Allow A-F 0-9
	TUI_WIPEONFILL = 0x400, // If a text area reaches its width and more is added, it will reset the value.
	TUI_WIPEONFUSE	= 0x800, // Clear a text areas's contents the *first* time data is sent to it.
	TUI_ALNUMONLY	= 0x1000, // Alphanumeric only
};

enum TUIType
{
	TUI_PARENT,
	TUI_MENUITEM,
	TUI_BUTTON,
	TUI_CONTAINER,
	TUI_SELECTAREA,
	TUI_DROPMENU,
	TUI_TEXT,
	TUI_TEXTINPUT,
};

enum TUIAlign
{
	ALIGN_CENTER,
	ALIGN_RIGHT,
};

enum TUIDirection
{
	TUI_BACKWARD		= -1,
	TUI_FORWARD		= +1,
};

static Screen* TUIScreen = new Screen();

short CalcPosition(TUIAlign align, short x_pos, short txt_width, short width=0);


class TUIObject;
struct TUIRetVal
{
	public:
		TUIRetVal(TUIObject* xobjref=NULL, int xposition=0)
		{
			objref = xobjref;
			position = xposition;
		}

		TUIObject* objref;
		int position;
};

class TUIObject
{
	public:
		TUIObject();
		~TUIObject();

		virtual bool Draw(bool force=false);
		virtual bool Remove();

		void SetXY(short x, short y);
		void SetX(short x);
		void SetY(short y);
		void Align(TUIAlign align, short x_pos, short width=0);
		short GetX();
		short GetY();

		virtual void SetWidth(short width);
		virtual void SetHeight(short height);
		virtual short GetWidth();
		virtual short GetHeight();

		void SetTextColor(short color);
		short GetTextColor();
		void SetBackgroundColor(short color);
		short GetBackgroundColor();

		void AddFlag(unsigned int flag);
		void RemoveFlag(unsigned int flag);
		unsigned int GetFlags();

		unsigned int GetSerial();

		virtual TUIRetVal SendInput(KeyInput input);

		virtual TUIObject* GetNext();
		virtual TUIObject* GetPrevious();
		virtual void SetNext(TUIObject* next);
		virtual void SetPrevious(TUIObject* previous);

		void SetHotKey(KeyInput input);
		KeyInput GetHotKey();

		bool CanSelect();
		void CanSelect(bool can_select);
		bool Selected();
		void Selected(bool selected);

		bool Drawn();
		void Drawn(bool drawn);

		bool Orphan();

		virtual bool IsA(TUIType type);

	protected:
		short _x, _y;
		short _width, _height;
		short _t_color, _bg_color;
		unsigned int _flags;
		unsigned int _serial;
		Screen* _screen;
		KeyInput _hotkey;
		bool _drawn;
		bool _can_select, _selected;

		TUIObject *_previous, *_next;
};

class TUIText : public TUIObject
{
	typedef TUIObject base;

	public:
		TUIText(apstring text);
		~TUIText();

		short AddTextLine(apstring text);
		short SetTextLine(apstring text, short line_num);
		apstring GetTextLine(short line_num);

		virtual bool Draw(bool force=false);
		virtual bool Remove();

		virtual bool IsA(TUIType type);

	protected:
		ScrnBfr _save;
		apvector<apstring> _textlines;
		bool _drawn;
};

class MenuItem : public TUIObject
{
	typedef TUIObject base;

	public:
		MenuItem();
		MenuItem(apstring name);
		~MenuItem();

		virtual bool Draw(bool force=false);
		virtual bool Remove();

		virtual short GetWidth();

		short GetHighlightColor();
		void SetHighlightColor(short color);
		short GetHighlightTextColor();
		void SetHighlightTextColor(short color);

		void SetName(apstring name);
		apstring GetName();

		void SetTUIParent(TUIObject* tui_ref);
		TUIObject* GetTUIParent();

		int GetRetVal();
		void SetRetVal(int retval);
		int GetPosition();

		virtual bool IsA(TUIType type);

	protected:
		short _hlt_color, _hlb_color;
		bool _drawn;
		apstring _name;
		TUIObject* _tui_ref;
		ScrnBfr _save;
		int _retval;
};

//Container
class TUIContainer : public TUIObject
{
	typedef TUIObject base;

	public:
		TUIContainer();
		~TUIContainer();

		virtual short GetHeight();
		virtual short GetWidth();

		void SetBorderType(short border_type);
		short GetBorderType();

		bool AddTUIObject(TUIObject* object);

		void DrawBox(bool toggle);
		virtual bool Draw(bool force=false);
		virtual bool Remove();

		virtual TUIRetVal SendInput(KeyInput input);

		TUIObject* GetHeadObject();
		TUIObject* GetTailObject();
		TUIObject* GetFocus();
		void SetFocus(int position, bool draw=false);
		void SetFocus(TUIObject* tuiobject, bool draw=false);

		TUIObject* FindObjectByHotKey(KeyInput &input, TUIObject* start=NULL, TUIDirection dir=TUI_FORWARD);

		short GetNumObjects();

		virtual bool IsA(TUIType type);

	protected:
		ScrnBfr _save;
		TUIObject *_head_object, *_tail_object, *_focus;
		bool _drawbox;
		short _border_type;
		short _num_objects;
		bool _drawn;
};

class SelectArea : public TUIContainer
{
	typedef TUIContainer base;

	public:
		SelectArea();
		~SelectArea();

		virtual short GetWidth();
		virtual short GetHeight();

		virtual MenuItem* AddOption(apstring name);

		virtual bool Draw(bool force=false);
		virtual bool Remove();

		void SetBorderType(short border_type);
		short GetBorderType();

		void SetPadding(short padding);
		short GetPadding();

		virtual TUIRetVal SendInput(KeyInput input);

		virtual bool IsA(TUIType type);

	protected:
		short _border_type;
		short _padding;
};

class DropMenu : public SelectArea
{
	typedef SelectArea base;

	public:
		DropMenu(apstring name);
		~DropMenu();
		void SetName(apstring name);
		apstring GetName();

		void SetHighlightColor(short color);
		short GetHighlightColor();
		void SetHighlightTextColor(short color);
		short GetHighlightTextColor();

		void DrawHeader(bool selected=false);
		virtual bool Remove();
		virtual bool Draw(bool force=false);

		virtual bool IsA(TUIType type);

	protected:
		short _hlb_color, _hlt_color;
		apstring _name;
};

class TextInput : public TUIObject
{
	typedef TUIObject base;

	public:
		TextInput(apstring init="");
		~TextInput();

		virtual bool SendInput(KeyInput input, bool update=false); // Standard access for adding data
		virtual bool SendInput(char input, bool update=false); // Adds character to _value
		virtual bool SendInput(int ascii, bool update=false); // Processes ascii input

		bool ValidInput(int ascii);

		bool Draw(bool force=false);

		apstring GetStringValue();
		long GetNumericValue();

		virtual bool IsA(TUIType type);

	protected:
		bool _drawn, _touched;
		apstring _value;
};

class TUIButton : public MenuItem
{
	typedef MenuItem base;

	public:
		TUIButton(apstring text, int retval);
		~TUIButton();

		virtual short GetWidth();

		virtual bool Draw(bool force=false);
		virtual bool Remove();

		virtual bool IsA(TUIType type);

	protected:
		short _retval;
};

#endif // __TUI_H