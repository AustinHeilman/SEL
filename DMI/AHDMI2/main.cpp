#include <iostream.h>
#include <time.h>

#include "cfgfile.h"
#include "apstring.h"
#include "keyboard.h"
#include "screen.h"
#include "tui.h"
#include "csv.h"
#include "sysutil.h"

void UpdateClock();

int main(/*int argc, char *argv[]*/)
{
	while ( KBHit() ) { Getch(); } // Clear the input buffer

	ConfigFile* cfgfile = new ConfigFile("AHDMI.CFG");
	CSVFile* csvfile = new CSVFile("MODELS.CSV");

	TUIScreen->WordWrap(false);
	TUIScreen->Cls(BLUE);
	TUIScreen->Cll(0, GREY);
	TUIScreen->Cll(24, GREY);
	TUIScreen->SetTextColor(RED);
	TUIScreen->SetBackgroundColor(GREY);
	disp_puts("Press 'Esc' to bring up menu (BIOS)");
	TUIScreen->GotoXY(0, 2);

	int err_level = EXIT_SUCCESS; // 0 (stdlib.h)
	TUIRetVal retval; // Stores return values from menu inputs
	KeyInput input;   // Keyboard input

	// Main control loop
	while ( 1 )
	{
		// Get keyboard input
		//if ( lastmenu != curmenu )
		//{ // Check if focus changed - update screen if-so.
		//	if ( lastmenu )
		//		lastmenu->Remove();
		//	if ( curmenu )
		//		curmenu->Draw();
		//	lastmenu = curmenu;
		//}
		while ( !KBHit() )
		{
			UpdateClock();
			usleep(20);
		}
		input = Getch();
	}

	delete TUIScreen;
	return 0;
}

void UpdateClock()
{
	time_t curtime;
	time(&curtime);
	TUIScreen->GotoXY(55, 0);
	TUIScreen->SetBackgroundColor(GREY);
	TUIScreen->SetTextColor(BLACK);
	disp_puts(ctime(&curtime));
}