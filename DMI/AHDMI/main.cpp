#include <stdlib.h>
#include <time.h>
#include <process.h>

#include "apstring.h"
#include "csv.h"
#include "screen.h"
#include "tui.h"
#include "keyboard.h"
#include "dmiinfo.h"
#include "screens.h"
#include "sysutil.h"

// Functions for building menus/windows are in screens.cpp

void CheckEnvVars();
bool CheckEnvVar(const char* name);
void UpdateClock();
TUIObject* FindSelectable(TUIContainer* dialogbox, int direction);
bool DMIEntry(TUIContainer* container, KeyInput &input, DMIKeys &dmi_keys);
int WriteConfirm(TUIContainer* confirm,  KeyInput &input);

int main(/*int argc, char *argv[]*/)
{
	// Clear the input buffer
	while ( KBHit() ) { Getch(); }
	CheckEnvVars();
	CreateDMIDefaultsFile();
	CSVFile* csv_data = new CSVFile(getenv("CSV_FILE"));
	DMIKeys dmi_keys = ReadDMIDefaults();

	TUIScreen->WordWrap(false);
	TUIScreen->Cls(BLUE);
	TUIScreen->Cll(0, GREY);
	TUIScreen->Cll(24, GREY);
	TUIScreen->SetTextColor(RED);
	TUIScreen->SetBackgroundColor(GREY);
	disp_puts("Press 'Esc' to bring up menu (BIOS)");
	TUIScreen->GotoXY(0, 2);

	// Begin menu building and running
	DropMenu* helpmenu = BuildHelpMenu();
	SelectArea* models = BuildModelsMenu(csv_data);
	DropMenu* filemenu = BuildFileMenu();
	TextInput* jumpinput = BuildLineJumpInput();
	TUIContainer* about = BuildAboutMenu();
	TUIContainer* unitinfo = BuildCurrentUnitInfo(dmi_keys);
	TUIContainer* editwindow = NULL;
	TUIContainer* confirm = NULL;

	int err_level = EXIT_SUCCESS; // 0 (stdlib.h)
	TUIRetVal retval; // Stores return values from menu inputs
	KeyInput input;   // Keyboard input

	CSVRow* csv_row = NULL; // Current CSV row selected

	TUIObject* curmenu = models;
	TUIObject* lastmenu = models;
	curmenu->Draw();

	while ( curmenu != NULL )
	{
		if ( input.RawInput() > 0 || !retval.position ) // No input or menu selection
		{
			// Allow the current menu class to handle keyboard input, first.
			retval = curmenu->SendInput(input);
			if ( retval.position == -1 )
			{ // Escape was pressed, close the menu.
				//Determine where to go based on the current menu open
				if ( curmenu == models )
					curmenu = filemenu;
				else if ( curmenu == filemenu )
					curmenu = models;
				else if ( curmenu == helpmenu )
					curmenu = models;
				else if ( curmenu == editwindow )
					curmenu = models;
				else if ( curmenu == unitinfo )
					curmenu = models;
				else if ( curmenu == about )
					curmenu = models;
				else if ( curmenu == confirm )
					curmenu = editwindow;
			}
			else if ( input.RawInput() == 8448 )  // Alt-F
			{
				curmenu = filemenu;
			}
			else if ( input.RawInput() == 8960 ) // Alt-H
			{
				curmenu = helpmenu;
			}
			else if ( input.RawInput() == 13631 ) // '?'
			{
				curmenu = about;
			}
			else
			{ // Deal with non-standard inputs for the current menu.
				if ( curmenu == filemenu )
				{
					switch ( input.RawInput() )
					{
						case RIGHT_KEY: curmenu = helpmenu; break;
						case LEFT_KEY: curmenu = helpmenu; break;
					}
					switch ( retval.position )
					{
						case 1: // View DMI
							curmenu = unitinfo;
							break;
						case 2: // BIOS
							err_level = 101;
							curmenu = NULL;
							break;
						case 4: // QUIT
							err_level = 100;
							curmenu = NULL;
							RebootSystem();
							break;
					}
				}
				else if ( curmenu == helpmenu )
				{
					switch ( input.RawInput() )
					{
						case LEFT_KEY: curmenu = filemenu; break;
						case RIGHT_KEY: curmenu = filemenu; break;
					}
					switch ( retval.position )
					{
						case 1: curmenu = about; break; // About
					}
				}
				else if ( curmenu == models || input.RawInput() == 8454 )
				{
					if ( retval.position > 0 || input.RawInput() == 8454 )
					{
						if ( retval.position > 0 )
						{
							csv_row = csv_data->GetRow(retval.position);
							curmenu = editwindow = BuildEditableUnitInfo(dmi_keys, editwindow, csv_row);
						}
						else
						{
							curmenu = editwindow = BuildEditableUnitInfo(dmi_keys, editwindow, NULL, true);
						}
						DMIEntry(editwindow, input, dmi_keys); // Ensure a selectable is.. selected on first run
					}
					else if ( input.RawInput() == RIGHT_KEY )
					{
						MenuItem* selected = static_cast<MenuItem*>(models->GetFocus());
						if ( selected != NULL )
						{
							int pos = selected->GetPosition()+23;
							if ( pos > models->GetNumObjects() )
								pos = models->GetNumObjects();
							models->SetFocus(pos, true);
						}
					}
					else if ( input.RawInput() == LEFT_KEY )
					{
						MenuItem* selected = static_cast<MenuItem*>(models->GetFocus());
						if ( selected != NULL )
						{
							int pos = selected->GetPosition()-23;
							if ( pos < 0 )
								pos = 0;
							models->SetFocus(pos, true);
						}
					}
					else if ( jumpinput->SendInput(input, true) )
						models->SetFocus(jumpinput->GetNumericValue(), true);
				}
				else if ( curmenu == editwindow )
				{
					bool retval = DMIEntry(editwindow, input, dmi_keys);
					if ( retval )
					{
						curmenu = confirm = BuildWriteConfirm(confirm);
						editwindow->Remove();
					}
				}
				else if ( curmenu == confirm )
				{
					int retval = WriteConfirm(confirm, input);
					if ( retval == 3 )
						curmenu = editwindow;
					else if ( retval == 2 )
					{
						err_level = 100;
						curmenu = NULL;
						WriteSystemDMI(dmi_keys, csv_row);
						RebootSystem();
					}
				}
			}
		}

		if ( curmenu )
		{
			if ( lastmenu != curmenu )
			{ // Check if focus changed - update screen if-so.
				if ( lastmenu )
					lastmenu->Remove();
				if ( curmenu )
					curmenu->Draw();
				lastmenu = curmenu;
			}
			while ( !KBHit() )
			{
				UpdateClock();
				usleep(20);
			}
			input = Getch();
		}
	}
	delete unitinfo;
	delete editwindow;
	delete about;
	delete models;
	delete filemenu;
	delete helpmenu;
	delete jumpinput;
	delete confirm;
	delete csv_data;
	delete TUIScreen;

	return err_level;
}

void CheckEnvVars()
{
	CheckEnvVar("SONY_DIR");
	CheckEnvVar("CSV_FILE");
	CheckEnvVar("TOOL_DATE");
	CheckEnvVar("BIOS_IMG");
	CheckEnvVar("FLASH_CMD");
	CheckEnvVar("DEFAULTS_FILE");
	CheckEnvVar("FLASH_CMD");
	CheckEnvVar("MCODE_UTIL");
	CheckEnvVar("MCODE_TMP_FILE");
}

bool CheckEnvVar(const char* name)
{
	if ( !getenv(name) )
	{
		TUIScreen->Cls(BLACK);
		cerr << "CheckEnvVar() Error - Could not find environment var '"<< name <<"'\n\n";
		assert(0);
		return false;
	}
	return true;
}

void UpdateClock()
{
	disp_hidecursor();

	time_t curtime;
	time(&curtime);
	TUIScreen->GotoXY(55, 0);
	TUIScreen->SetBackgroundColor(GREY);
	TUIScreen->SetTextColor(BLACK);
	disp_puts(ctime(&curtime));
}

bool DMIEntry(TUIContainer* dialogbox, KeyInput &input, DMIKeys &dmi_keys)
{
	TUIObject* focus = dialogbox->GetFocus();

	if ( input.RawInput() == ENTER_KEY && focus->IsA(TUI_BUTTON) )
	{
		TUIButton* button = static_cast<TUIButton*>(focus);
		if ( button->GetRetVal() == 1 )
		{
			for ( int i=0; i<dmi_keys.size(); i++ )
			{
				if ( !ValidateDMIEntry(&dmi_keys[i]) )
					return false;
			}

			return true;
		}
	}
	else if ( !( focus->CanSelect() ) ||
		input.RawInput() == UP_KEY || input.RawInput() == DOWN_KEY ||
		input.RawInput() == ENTER_KEY || input.RawInput() == TAB_KEY )
	{
		TUIObject* newfocus;
		if ( input.RawInput() == UP_KEY )
			newfocus = FindSelectable(dialogbox, TUI_BACKWARD);
		else
			newfocus = FindSelectable(dialogbox, TUI_FORWARD);

		if ( focus->IsA(TUI_TEXTINPUT) )
		{
			DMIKey* dmikey = GetDMIKey(static_cast<TextInput*>(focus), dmi_keys);
			assert(dmikey != NULL);
			if ( ValidateDMIEntry(dmikey) )
				focus->SetBackgroundColor(GREEN);
			else
				focus->SetBackgroundColor(RED);
		}

		if ( focus->CanSelect() )
		{
			focus->Selected(false);
			if ( dialogbox->Drawn() )
				focus->Draw(true);
		}

		if ( newfocus->IsA(TUI_TEXTINPUT) )
			newfocus->SetBackgroundColor(CYAN);
		else if ( newfocus->IsA(TUI_BUTTON) )
			newfocus->Selected(true);

		if ( newfocus->CanSelect() && dialogbox->Drawn() )
			newfocus->Draw(true);
		dialogbox->SetFocus(newfocus);
	}
	else
	{
		if ( focus->IsA(TUI_TEXTINPUT) )
		{
			TextInput* entry = static_cast<TextInput*>(focus);
			entry->SendInput(input, true);
		}
	}

	return false;
}

int WriteConfirm(TUIContainer* confirm,  KeyInput &input)
{
	TUIObject* focus = confirm->GetFocus();
	if ( input.RawInput() == ENTER_KEY && focus->IsA(TUI_BUTTON) )
	{
		TUIButton* button = static_cast<TUIButton*>(focus);
		return button->GetRetVal();
	}
	else if ( input.RawInput() == UP_KEY || input.RawInput() == LEFT_KEY
		|| input.RawInput() == DOWN_KEY || input.RawInput() == RIGHT_KEY
		|| confirm->FindObjectByHotKey(input) )
	{
		if ( !focus->CanSelect() )
			return 0;

		TUIObject* newfocus = confirm->FindObjectByHotKey(input);
		if ( !newfocus )
		{
			if ( input.RawInput() == UP_KEY || input.RawInput() == LEFT_KEY )
				newfocus = FindSelectable(confirm, -1);
			else if ( input.RawInput() == DOWN_KEY || input.RawInput() == RIGHT_KEY )
				newfocus = FindSelectable(confirm, +1);
		}

		static_cast<TUIButton*>(focus)->Selected(false);
		focus->Draw(true);

		static_cast<TUIButton*>(newfocus)->Selected(true);
		newfocus->Draw(true);

		confirm->SetFocus(newfocus);
	}

	return 0;
}

TUIObject* FindSelectable(TUIContainer* container, int direction)
{
	TUIObject* start = container->GetFocus();
	TUIObject* current = start;
	while ( 1 )
	{
		if ( direction == TUI_BACKWARD )
			current = current->GetPrevious();
		else
			current = current->GetNext();

		if ( !current )
		{
			if ( direction == TUI_BACKWARD )
				current = container->GetTailObject();
			else
				current = container->GetHeadObject();
		}
		if ( current == start )
			break;
		else if ( current->CanSelect() )
			return current;
	}
	assert(0);
	return NULL;
}
