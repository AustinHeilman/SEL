#include <direct.h>
#include <time.h>
#include <process.h>

#include "apstring.h"
#include "tui.h"
#include "screen.h"
#include "keyboard.h"
#include "fileutil.h"

void SortModules(FilesFound &modules);
SelectArea* BuildModulesMenu(FilesFound &modules);
TextInput* BuildLineJumpInput();
bool LoadModule(TUIRetVal &retval, FilesFound &modules);
void UpdateClock();

int main(int argc, char *argv[])
{
	// Clear the input buffer
	while ( KBHit() ) { Getch(); }

	disp_hidecursor();
	TUIScreen->WordWrap(false);
	TUIScreen->Cls(BLUE);
	TUIScreen->Cll(0, GREY);
	TUIScreen->Cll(24, GREY);
	TUIScreen->SetTextColor(BLACK);
	TUIScreen->SetBackgroundColor(GREY);

	char path[_MAX_DIR];
	_getcwd(path, _MAX_DIR);
	apstring root(path);
	FilesFound modules = GetFilesInDir(root, "*.*", _A_SUBDIR);
	SortModules(modules);

	TUIScreen->GotoXY(1, 0);
	disp_printf("DMI Modules found in %s", root.c_str());
	TUIScreen->GotoXY(1, 24);
	disp_printf("Ver. 0.4  (6AH)");
	TUIScreen->GotoXY(30, 24);
	disp_printf("Modules Found: %i", modules.size());

	SelectArea* menu = BuildModulesMenu(modules);
	TextInput* jumpinput = BuildLineJumpInput();
	TUIRetVal retval;
	KeyInput input;

	menu->Draw();
	bool exit = false;
	while ( !exit )
	{
		if ( input.RawInput() > 0 || !retval.position )
		{
			retval = menu->SendInput(input);
			if ( retval.position > 0 )
			{
				exit = LoadModule(retval, modules);
			}
			else
			{
				if ( input.RawInput() == RIGHT_KEY )
				{
					MenuItem* selected = static_cast<MenuItem*>(menu->GetFocus());
					if ( selected != NULL )
					{
						int pos = selected->GetPosition()+23;
						if ( pos > menu->GetNumObjects() )
							pos = menu->GetNumObjects();
						menu->SetFocus(pos, true);
					}
				}
				else if ( input.RawInput() == LEFT_KEY )
				{
					MenuItem* selected = static_cast<MenuItem*>(menu->GetFocus());
					if ( selected != NULL )
					{
						int pos = selected->GetPosition()-23;
						if ( pos < 0 )
							pos = 0;
						menu->SetFocus(pos, true);
					}
				}
				else if ( jumpinput->SendInput(input, true) )
					menu->SetFocus(jumpinput->GetNumericValue(), true);
			}
		}

		if ( !exit )
		{
			menu->Draw();
			while ( !KBHit() )
			{
				UpdateClock();
				usleep(20);
			}
			input = Getch();
		}
	}

	delete menu;
	delete jumpinput;
	delete TUIScreen;

	cout << "DMI Module Lister\n";
   	cout << "Austin Heilman 9/28/2007\n";

	return EXIT_SUCCESS;
}

void SortModules(FilesFound &modules)
{
	// SelectionSort for lower memory usage
	int min;
	for ( int i=0; i < (modules.size()-1); i++ )
	{
		min = i;
		for ( int j=(i+1); j < modules.size(); j++ )
		{
			if ( modules[j].name < modules[min].name )
			{
				min = j;
			}
		}
		FileFound tmp = modules[i];
		modules[i] = modules[min];
		modules[min] = tmp;
	}
}


SelectArea* BuildModulesMenu(FilesFound &modules)
{
	SelectArea* menu = new SelectArea();
	menu->SetBackgroundColor(BLUE);
	menu->SetXY(0, 1);
	menu->SetWidth(26);
	menu->AddFlag(TUI_NOCLOSE);

	int y_pos = 1;
	int x_pos = 0;
	for ( int i=0; i<modules.size(); i++ )
	{
		if ( i >= 70 )
			break;

		char tmp[4];
		itoa((i+1), tmp, 10);
		apstring line(tmp);
		line.Pad(' ', 5-line.length());
		line += modules[i].name;

		MenuItem* option = menu->AddOption(line);
		option->SetTextColor(GREY);
		option->SetBackgroundColor(BLUE);
		option->SetHighlightColor(CYAN);
		option->SetHighlightTextColor(BLACK);
		option->SetXY(x_pos, y_pos);
		option->SetWidth(-1);
		option->SetRetVal(i);

		if ( y_pos % 23 == 0 ) // 23 lines used
		{
			menu->SetHeight(23);
			x_pos += 27;
			y_pos = 1;
		}
		else
			y_pos++;
	}

	return menu;
}

TextInput* BuildLineJumpInput()
{
	TUIScreen->SetTextColor(BLACK);
	TUIScreen->GotoXY(65, 24);
	disp_puts("Quick Select:");

	TextInput* input = new TextInput();
	input->SetBackgroundColor(BLACK);
	input->SetTextColor(WHITE);
	input->SetWidth(2);
	input->SetXY(78, 24);
	input->AddFlag(TUI_NUMONLY+TUI_WIPEONFILL);

	input->Draw();

	return input;
}

bool LoadModule(TUIRetVal &retval, FilesFound &modules)
{
	FileFound module = modules[retval.position-1];

	apstring path = module.path + module.name;
	int result = _chdir(path.c_str());

	if ( result != 0 )
	{
		disp_printf("Error changing directory! %i", result);
		assert(0);
	}
	/*
	// Code below will break PERL4S but works fine with AHDMI
	else
	{
		int retval = _spawnlp(_P_WAIT, "autoexec.bat", "autoexec.bat", module.name.c_str(), NULL);
		if ( retval != 0 )
		{
			cerr << "LoadModule() 'autoexec.bat' Error - Retval was '"<<retval<<"'\n";
			cerr << "Arguments:\n";
			cerr << module.name << "\n\n";
			assert(0);
		}
	}
	*/
	return ( result == 0 );
}

void UpdateClock()
{
	disp_hidecursor();
	TUIScreen->GotoXY(55, 0);
	TUIScreen->SetBackgroundColor(GREY);
	TUIScreen->SetTextColor(BLACK);

	time_t curtime;
	time(&curtime);
	disp_puts(ctime(&curtime));
}