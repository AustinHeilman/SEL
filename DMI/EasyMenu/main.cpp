#include "tui.h"
#include "cfgfile.h"

SelectArea* BuildSelectMenu(config);

int main()
{
	ConfigFile* config = new ConfigFile("menus.cfg");

	BuildSelectMenu(config);

	delete TUIScreen;
	return 0;
}

SelectArea* BuildSelectMenu(ConfigElem* cfgelem)