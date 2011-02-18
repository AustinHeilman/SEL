#include <ctype.h>
#include <stdlib.h>

#include "tui.h"
#include "keyboard.h"

unsigned int TUI_SERIAL_POOL = 0;

short CalcPosition(TUIAlign align, short x_pos, short txt_width, short width)
{
	if ( align == ALIGN_CENTER && txt_width <= width )
	{	// Lines object up between x_pos and x_pos+width
		return short(x_pos+short((width-txt_width)/2));
	}
	else if ( align == ALIGN_RIGHT && x_pos >= txt_width )
	{	// Lines object up to the left of x_pos
		return x_pos - txt_width;
	}
	else
		return x_pos;
}


// **********
// TUIObject:: Functions
// **********

TUIObject::TUIObject()
{
	_x = _y = 0;
	_width = _height = 0;
	_t_color = GREY;
	_bg_color = BLACK;
	_flags = 0;
	_serial = ++TUI_SERIAL_POOL;
	Drawn(false);
	Selected(false);
	CanSelect(false);

	_previous = _next = NULL;
}
TUIObject::~TUIObject()
{
	_serial = 0;
}

bool TUIObject::Draw(bool force)
{
	return true;
}

bool TUIObject::Remove()
{
	return true;
}

void TUIObject::SetXY(short x, short y)
{
	SetX(x);
	SetY(y);
}

void TUIObject::SetX(short x)
{
	_x = x;
}

void TUIObject::SetY(short y)
{
	_y = y;
}

short TUIObject::GetX()
{
	return _x;
}

short TUIObject::GetY()
{
	return _y;
}

void TUIObject::Align(TUIAlign align, short x_pos, short width)
{
	_x = CalcPosition(align, x_pos, GetWidth(), width);
}

void TUIObject::SetWidth(short width)
{
	_width = width;
}

void TUIObject::SetHeight(short height)
{
	_height = height;
}

short TUIObject::GetWidth()
{
	return _width;
}

short TUIObject::GetHeight()
{
	return _height;
}

void TUIObject::SetTextColor(short color)
{
	_t_color = color;
}

short TUIObject::GetTextColor()
{
	return _t_color;
}

void TUIObject::SetBackgroundColor(short color)
{
	_bg_color = color;
}

short TUIObject::GetBackgroundColor()
{
	return _bg_color;
}

void TUIObject::AddFlag(unsigned int flag)
{
	_flags += flag;
}

void TUIObject::RemoveFlag(unsigned int flag)
{
	_flags -= flag;
}

unsigned int TUIObject::GetFlags()
{
	return _flags;
}

unsigned int TUIObject::GetSerial()
{
	return _serial;
}

TUIRetVal TUIObject::SendInput(KeyInput input)
{
	// Abstract classes will need to have their own version of this function
	return TUIRetVal(NULL, 0);
}

TUIObject* TUIObject::GetNext()
{
	return _next;
}

TUIObject* TUIObject::GetPrevious()
{
	return _previous;
}

void TUIObject::SetNext(TUIObject* next)
{
	_next = next;
}

void TUIObject::SetPrevious(TUIObject* previous)
{
	_previous = previous;
}

void TUIObject::SetHotKey(KeyInput input)
{
	_hotkey = input;
}

KeyInput TUIObject::GetHotKey()
{
	return _hotkey;
}

bool TUIObject::CanSelect()
{
	return _can_select;
}

void TUIObject::CanSelect(bool can_select)
{
	_can_select = can_select;
}

bool TUIObject::Selected()
{
	return _selected;
}

void TUIObject::Selected(bool selected)
{
	_selected = selected;
}

bool TUIObject::Drawn()
{
	return _drawn;
}

void TUIObject::Drawn(bool drawn)
{
	_drawn = drawn;
}

bool TUIObject::Orphan()
{
	return ( _serial == 0 );
}

bool TUIObject::IsA(TUIType type)
{
	return ( type == TUI_PARENT );
}

// **********
// TUIContainer:: Functions
// **********

TUIContainer::TUIContainer()
{
	_head_object = _tail_object = _focus = NULL;
	_drawbox = false;
	SetBorderType(-1);
	_num_objects = 0;
}

TUIContainer::~TUIContainer()
{
	TUIObject* current = GetHeadObject();
	while ( current )
	{
		current = current->GetNext();
		delete _head_object;
		_head_object = current;
	}
	_focus = NULL;
	_head_object = _tail_object = current = NULL;
}

short TUIContainer::GetHeight()
{
	if ( _height == 0 )
		return _num_objects;
	else
		return _height;
}

short TUIContainer::GetWidth()
{
	if ( _width == 0 )
	{
		short width = 0;
		TUIObject* current = GetHeadObject();
		while ( current != NULL )
		{
			if ( current->GetWidth() > width )
				width = current->GetWidth();
		}
		return width;
	}
	return _width;
}

void TUIContainer::SetBorderType(short border_type)
{
	_border_type = border_type;
}

short TUIContainer::GetBorderType()
{
	return _border_type;
}

bool TUIContainer::AddTUIObject(TUIObject* object)
{
	if ( object == NULL )
		return false;
	else if ( object == this )
		return false;
	else if ( !_head_object )
	{
		_head_object = _tail_object = object;
		SetFocus(object);
	}
	else
	{
		_tail_object->SetNext(object);
		object->SetPrevious(_tail_object);
		_tail_object = object;
	}
	_num_objects++;

	return true;
}

void TUIContainer::DrawBox(bool toggle)
{
	_drawbox = toggle;
}

bool TUIContainer::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;
	else
		Drawn(true);

	if ( _drawbox )
	{
		if ( _flags & TUI_SAVEBFR )
			TUIScreen->PeekBox(_save, _x, _y, _x+GetWidth(), _y+GetHeight());

		if ( _border_type >= 0 )
			TUIScreen->DrawBox(_border_type, _t_color, _bg_color, _x, _y, _x+GetWidth(), _y+GetHeight());
		else
			TUIScreen->FillBox(_bg_color, _x, _y, _x+GetWidth(), _y+GetHeight());
	}

	TUIObject* current = GetHeadObject();
	while ( current )
	{
		current->Draw(true);
		current = current->GetNext();
	}

	return true;
}

bool TUIContainer::Remove()
{
	if ( !Drawn() )
		return false;
	else
		Drawn(false);

	TUIObject* current = GetTailObject();
	while ( current )
	{
		current->Remove();
		current = current->GetPrevious();
	}

	if ( _flags & TUI_SAVEBFR )
		TUIScreen->PokeBox(_save, _x, _y, _x+GetWidth(), _y+GetHeight());

	return true;
}

TUIRetVal TUIContainer::SendInput(KeyInput input)
{
	switch( input.RawInput() )
	{
		case ESC_KEY:
		{
			if ( !(_flags & TUI_NOESCAPE) ) // Escape key is allowed
				return TUIRetVal(NULL, -1); // Escape key hit.
			break;
		}
	}
	return TUIRetVal(NULL, 0);	// No official result, do nothing.
}

TUIObject* TUIContainer::GetHeadObject()
{
	return _head_object;
}

TUIObject* TUIContainer::GetTailObject()
{
	return _tail_object;
}

TUIObject* TUIContainer::GetFocus()
{
	return _focus;
}

void TUIContainer::SetFocus(int position, bool draw)
{
	if ( position < 1 )
		position = 1;

	int count = 1;
	TUIObject* current = GetHeadObject();
	while ( current )
	{
		if ( count == position )
		{
			SetFocus(current, draw);
			break;
		}
		else
		{
			current = current->GetNext();
			count++;
		}
	}
}

void TUIContainer::SetFocus(TUIObject* tuiobject, bool draw)
{
	TUIObject* curfocus = GetFocus();
	if ( curfocus )
	{
		curfocus->Selected(false);
		if ( draw )
			curfocus->Draw(true);
	}
	_focus = tuiobject;
	tuiobject->Selected(true);
	if ( draw )
		tuiobject->Draw(draw);
}

TUIObject* TUIContainer::FindObjectByHotKey(KeyInput &input, TUIObject* start, TUIDirection dir)
{
	if ( !start )
		start = GetHeadObject();

	TUIObject* current = start;
	while ( 1 )
	{
		if ( current->GetHotKey() == input )
			return current;
		else
		{
			if ( dir == TUI_BACKWARD )
				current = current->GetPrevious();
			else
				current = current->GetNext();

			if ( !current )
			{
				if ( dir == TUI_BACKWARD )
					current = GetTailObject();
				else
					current = GetHeadObject();
			}
		}
		if ( current == start )
			break;
	}

	return NULL;
}

short TUIContainer::GetNumObjects()
{
	return _num_objects;
}

bool TUIContainer::IsA(TUIType type)
{
	return ( type == TUI_CONTAINER || base::IsA(type) );
}



// **********
// TUIText:: Functions
// **********
TUIText::TUIText(apstring text)
{
	AddTextLine(text);
}
TUIText::~TUIText()
{
}

short TUIText::AddTextLine(apstring text)
{
	_textlines.append(text);
	if ( text.length() > _width )
		_width = text.length();
	_height++;

	return _textlines.size();
}

short TUIText::SetTextLine(apstring text, short line_num)
{
	if ( line_num > _textlines.size() )
		return AddTextLine(text);
	else
	{
		_textlines[line_num] = text;
		if ( text > _width )
			_width = text.length();

		return line_num;
	}
}

apstring TUIText::GetTextLine(short line_num)
{
	if ( line_num > _textlines.size() )
		return apstring();
	return _textlines[line_num];
}

bool TUIText::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;
	else
		Drawn(true);

	if ( _flags & TUI_SAVEBFR )
	{
		TUIScreen->PeekBox(_save, _x, _y, _x+GetWidth(), _y+GetHeight());
	}

	TUIScreen->SetTextColor(_t_color);
	TUIScreen->SetBackgroundColor(_bg_color);

	//TUIScreen->FillBox(_bg_color, _x, _y, _x+GetWidth(), _y+GetHeight());

	short y_pos = _y;
	for ( int i=0; i<_textlines.size(); i++ )
	{
		TUIScreen->GotoXY(_x, y_pos++);
		disp_puts(_textlines[i].c_str());
	}
	return true;
}

bool TUIText::Remove()
{
	if ( !Drawn() )
		return false;

	if ( _flags & TUI_SAVEBFR )
	{
		short width = (GetWidth()-1);
		short height = (GetHeight()-1);
		TUIScreen->PokeBox(_save, _x, _y, _x+width, _y+height);
	}

	return true;
}

bool TUIText::IsA(TUIType type)
{
	return ( type == TUI_TEXT || base::IsA(type) );
}


// **********
// MenuItem:: Functions
// **********

MenuItem::MenuItem()
{
	MenuItem("NULL");
}
MenuItem::~MenuItem()
{
}

MenuItem::MenuItem(apstring name)
{
	CanSelect(true);
	SetName(name);
	SetTUIParent(NULL);
	SetHighlightColor(BLACK);
	SetHighlightTextColor(-1);
	SetRetVal(GetSerial());
}

bool MenuItem::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;
	else
		Drawn(true);

	if ( _flags & TUI_SAVEBFR )
	{
		TUIScreen->PeekBox(_save, _x, _y, _x+GetWidth(), _y+GetHeight());
	}

	TUIScreen->GotoXY(_x, _y);

	if ( _flags & TUI_BARENTRY )
	{
		TUIScreen->SetTextColor(GetTextColor());
		TUIScreen->SetBackgroundColor(_bg_color);
		TUIScreen->FillBox(_bg_color, _x, _y, (_x+GetWidth())-1, _y);
		for ( short i=0; i<GetWidth(); i++ )
		{
			disp_putc('Ä');
		}
	}
	else if ( Selected() )
	{
		TUIScreen->SetTextColor(GetHighlightTextColor());
		TUIScreen->SetBackgroundColor(_hlb_color);
		TUIScreen->FillBox(_hlb_color, _x, _y, (_x+GetWidth())-1, _y);
		disp_puts(_name.c_str());
	}
	else
	{
		TUIScreen->SetTextColor(GetTextColor());
		TUIScreen->SetBackgroundColor(_bg_color);
		TUIScreen->FillBox(_bg_color, _x, _y, (_x+GetWidth())-1, _y);
		disp_puts(_name.c_str());
	}

	return true;
}

bool MenuItem::Remove()
{
	if ( !Drawn() )
		return false;
	else
		Drawn(false);

	if ( _flags & TUI_SAVEBFR)
	{
		TUIScreen->PokeBox(_save, _x, _y, _x+GetWidth(), _y+GetHeight());
	}

	return true;
}

short MenuItem::GetWidth()
{
	if ( _width <= 0 && GetTUIParent() )
		return GetTUIParent()->GetWidth();
	else if ( _name.length() > _width )
		return _name.length();
	else
		return _width;
}

short MenuItem::GetHighlightColor()
{
	return _hlb_color;
}

void MenuItem::SetHighlightColor(short color)
{
	_hlb_color = color;
}

short MenuItem::GetHighlightTextColor()
{
	if ( _hlt_color < 0 )
		return GetTextColor();
	return _hlt_color;
}

void MenuItem::SetHighlightTextColor(short color)
{
	_hlt_color = color;
}

void MenuItem::SetName(apstring name)
{
	_name = name;
	if ( _width < _name.length() )
		SetWidth(_name.length());
}

apstring MenuItem::GetName()
{
	return _name;
}

void MenuItem::SetTUIParent(TUIObject* tui_ref)
{
	_tui_ref = tui_ref;
}

TUIObject* MenuItem::GetTUIParent()
{
	return _tui_ref;
}

int MenuItem::GetRetVal()
{
	return _retval;
}

void MenuItem::SetRetVal(int retval)
{
	_retval = retval;
}

int MenuItem::GetPosition()
{
	int count = 0;
	TUIObject* current = this;
	while ( current )
	{
		count++;
		current = current->GetPrevious();
	}
	return count;
}

bool MenuItem::IsA(TUIType type)
{
	return ( type == TUI_MENUITEM || base::IsA(type) );
}



// **********
// SelectArea:: Functions
// **********

SelectArea::SelectArea()
{
	_border_type = -1;
	_padding = 0;
}
SelectArea::~SelectArea()
{
}

short SelectArea::GetWidth()
{
	if ( _width <= 0 )
	{
		short width = 0;
		TUIObject* current = GetHeadObject();
		while ( current )
		{
			if ( current->GetWidth() > width )
				width = current->GetWidth();
			current = current->GetNext();
		}
		if ( width <= 0 )
			return 1;
		else
			return width;
	}
	else
		return _width;
}

short SelectArea::GetHeight()
{
	if ( _height <= 0 )
		return _num_objects;
	else
		return _height;
}

void SelectArea::SetBorderType(short border_type)
{
	if ( border_type >= 0 && _border_type < 0 ) // No border to having a border
		_padding++;
	else if ( border_type < 0 && _border_type >= 0 ) // Removing the border
		_padding--;

	_border_type = border_type;
}

short SelectArea::GetBorderType()
{
	return _border_type;
}

void SelectArea::SetPadding(short padding)
{
	if ( padding < 1 && _border_type >= 0 )
	{
		// If there is a border type, the padding must be atleast 1
		padding = 1;
	}
	else if ( padding < 0 )
		padding = 0;

	_padding = padding;
}

short SelectArea::GetPadding()
{
	return _padding;
}

MenuItem* SelectArea::AddOption(apstring name)
{
	MenuItem* menuitm = new MenuItem(name);
	menuitm->SetTUIParent(this);
	AddTUIObject(static_cast<TUIObject*>(menuitm));
	menuitm->SetXY(_x+_padding, _y+_num_objects);

	return menuitm;
}

bool SelectArea::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;
	else
		Drawn(true);

	short width = (GetWidth()-1)+(_padding*2);
	short height = (GetHeight()-1)+(_padding*2);

	TUIScreen->GotoXY(_x, _y);

	if ( _flags & TUI_SAVEBFR )
	{
		TUIScreen->PeekBox(_save, _x, _y, _x+width, _y+height);
	}

	if ( _border_type >= 0 )
	{
		TUIScreen->DrawBox(_border_type, _t_color, _bg_color, _x, _y, _x+width, _y+height);
	}
	else
	{
		TUIScreen->FillBox(_bg_color, _x, _y, _x+width, _y+height);
	}

	TUIObject* current = GetHeadObject();
	while ( current )
	{
		current->Draw(true);
		current = current->GetNext();
	}
	return true;
}

bool SelectArea::Remove()
{
	if ( !Drawn() )
		return false;
	else
		Drawn(false);

	if ( _flags & TUI_SAVEBFR )
	{
		short width = (GetWidth()-1)+(_padding*2);
		short height = (GetHeight()-1)+(_padding*2);

		TUIScreen->PokeBox(_save, _x, _y, _x+width, _y+height);
	}
	return true;
}

TUIRetVal SelectArea::SendInput(KeyInput input)
{
	TUIObject* current = GetFocus();
	switch( input.RawInput() )
	{
		case ESC_KEY:
		{
			if ( !(_flags & TUI_NOESCAPE) ) // Escape key is allowed
				return TUIRetVal(NULL, -1); // Escape key hit.
			break;
		}
		case ENTER_KEY:
		{
			MenuItem* tmp = static_cast<MenuItem*>(current);
			return TUIRetVal(current, tmp->GetPosition());
			break;
		}
		case UP_KEY:
		{
			current->Selected(false);
			current->Draw(true);

			while ( 1 )
			{
				current = current->GetPrevious();
				if ( !current )
					current = GetTailObject();

				if ( current->CanSelect() )
					break;
			}

			SetFocus(current);
			current->Selected(true);
			current->Draw(true);
			break;
		}
		case DOWN_KEY:
		{
			current->Selected(false);
			current->Draw(true);
			while ( 1 )
			{
				current = current->GetNext();
				if ( !current )
					current = GetHeadObject();

				if ( current->CanSelect() )
					break;
			}

			SetFocus(current);
			current->Selected(true);
			current->Draw(true);
			break;
		}
	}

	return TUIRetVal(NULL, 0);	// No official result, do nothing.
}

bool SelectArea::IsA(TUIType type)
{
	return ( type == TUI_SELECTAREA || base::IsA(type) );
}



// **********
// DropMenu:: Functions
// **********

DropMenu::DropMenu(apstring name)
{
	_name = name;
	_hlb_color = BLACK;
	_hlt_color = GREY;
}
DropMenu::~DropMenu()
{
}

void DropMenu::DrawHeader(bool selected)
{
	TUIScreen->GotoXY(_x, _y-1);
	if ( selected )
	{
		TUIScreen->SetTextColor(GetHighlightTextColor());
		TUIScreen->SetBackgroundColor(_hlb_color);
	}
	else
	{
		TUIScreen->SetTextColor(_t_color);
		TUIScreen->SetBackgroundColor(_bg_color);
	}
	disp_puts(_name.c_str());
}

void DropMenu::SetName(apstring name)
{
	_name = name;
}

apstring DropMenu::GetName()
{
	return _name;
}

void DropMenu::SetHighlightColor(short color)
{
	_hlb_color = color;
}

short DropMenu::GetHighlightColor()
{
	return _hlb_color;
}

short DropMenu::GetHighlightTextColor()
{
	if ( _hlt_color < 0 )
		return GetTextColor();
	return _hlt_color;
}

void DropMenu::SetHighlightTextColor(short color)
{
	_hlt_color = color;
}

bool DropMenu::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;

	DrawHeader(true);

	return base::Draw(true);
}

bool DropMenu::Remove()
{
	if ( !Drawn() )
		return false;

	DrawHeader(false);

	return base::Remove();
}

bool DropMenu::IsA(TUIType type)
{
	return ( type == TUI_DROPMENU || base::IsA(type) );
}



// **********
// TextInput:: Functions
// **********

TextInput::TextInput(apstring init)
{
	_can_select = true;
	_touched = false;
	_value = init;
}
TextInput::~TextInput()
{
}

bool TextInput::SendInput(KeyInput input, bool update)
{
	if ( input.IsASCII() )
		return SendInput(input.ASCIIValue(), update);
	else
		return false;
}

bool TextInput::SendInput(char input, bool update)
{
	return SendInput(int(input), update);
}

bool TextInput::SendInput(int ascii, bool update)
{
	if ( ascii == (BACKSPACE_KEY&0xFF) )
	{
		_value = _value.substr(0, _value.length()-1);

		if ( update )
			Draw(true);

		_touched = true;

		return true;
	}
	else if ( !ValidInput(ascii))
	{
		return false;
	}
	else if ( !_touched && (_flags & TUI_WIPEONFUSE) )
	{
		_value = "";
	}

	else if ( _value.length() >= _width )
	{
		if ( _flags & TUI_WIPEONFILL )
			_value = "";
		else
			return false;
	}

	_touched = true;


	if ( _flags & TUI_FORCELOWER )
		ascii = tolower(ascii);
	else if ( _flags & TUI_FORCEUPPER )
		ascii = toupper(ascii);

	_value += char(ascii);

	if ( update )
		Draw(true);

	return true;
}

bool TextInput::ValidInput(int ascii)
{
	if ( _flags & TUI_READONLY ) // Don't allow writing
		return false;
	else if ( (_flags & TUI_NUMONLY) && !isdigit(ascii) ) // Only allow 0-9
		return false;
	else if ( (_flags & TUI_HEXONLY) && !isxdigit(ascii) ) // Only allow 0-9 A-F
		return false;
	else if ( (_flags & TUI_ABCONLY) && !isalpha(ascii) ) // Only allow A-Z
		return false;
	else if ( (_flags & TUI_ALNUMONLY) && !isalnum(ascii) ) // Alphanumerics only
		return false;
	else
		return true;
}

bool TextInput::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;
	else
		Drawn(false);

	TUIScreen->GotoXY(_x, _y);
	TUIScreen->FillBox(_bg_color, _x, _y, (_x+_width-1), _y);
	TUIScreen->SetBackgroundColor(_bg_color);
	TUIScreen->SetTextColor(_t_color);
	disp_puts(_value.c_str());

	return true;
}

apstring TextInput::GetStringValue()
{
	return _value;
}

long TextInput::GetNumericValue()
{
	if ( !(_flags & TUI_NUMONLY) )
		return 0;
	if ( _value.length() == 0 )
		return 0;

	long int i;
	char* endptr;
	char* tmp = const_cast<char*>(_value.c_str());

	i = strtol(tmp, &endptr, 10);

	return i;
}

bool TextInput::IsA(TUIType type)
{
	return ( type == TUI_TEXTINPUT || base::IsA(type) );
}



// **********
// TUIButton:: Functions
// **********

TUIButton::TUIButton(apstring name, int retval)
{
	CanSelect(true);
	SetName(name);
	SetRetVal(retval);
	_selected = false;
	_hlb_color = BLACK;
	_hlt_color = GREY;
}
TUIButton::~TUIButton()
{
}

short TUIButton::GetWidth()
{
	return GetName().length()+2; // Room for the > < arrows
}

bool TUIButton::Draw(bool force)
{
	if ( Drawn() && !force )
		return false;
	else
		Drawn(true);

	TUIScreen->GotoXY(_x, _y);
	if ( _selected )
	{
		TUIScreen->FillBox(_hlb_color, _x, _y, (_x+GetWidth()-1), _y);
		TUIScreen->SetBackgroundColor(_hlb_color);
		TUIScreen->SetTextColor(_hlt_color);
	}
	else
	{
		TUIScreen->FillBox(_bg_color, _x, _y, (_x+GetWidth()-1), _y);
		TUIScreen->SetBackgroundColor(_bg_color);
		TUIScreen->SetTextColor(_t_color);
	}

	short txt_ctr = CalcPosition(ALIGN_CENTER, _x, _name.length(), GetWidth());
	if ( _selected )
		disp_printf("%c", char(16));
	TUIScreen->GotoXY(txt_ctr, _y);

	disp_puts(_name.c_str());
	if ( _selected )
		disp_printf("%c", char(17));

	return true;
}

bool TUIButton::Remove()
{
	if ( !Drawn() )
		return false;
	else
		Drawn(false);

	if ( _flags & TUI_SAVEBFR )
	{
		TUIScreen->PokeBox(_save, _x, _y, GetWidth(), _height);
	}
	return true;
}

bool TUIButton::IsA(TUIType type)
{
	return ( type == TUI_BUTTON || base::IsA(type) );
}
