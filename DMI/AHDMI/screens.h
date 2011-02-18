#ifndef __SCREENS_H
#define __SCREENS_H

#include <stdlib.h>

#include "apstring.h"
#include "csv.h"
#include "screen.h"
#include "tui.h"
#include "keyboard.h"
#include "dmiinfo.h"

SelectArea* BuildModelsMenu(CSVFile* csv);
DropMenu* BuildFileMenu();
DropMenu* BuildHelpMenu();
TextInput* BuildLineJumpInput();
TUIContainer* BuildAboutMenu();
TUIContainer* BuildCurrentUnitInfo(DMIKeys &dmi_keys);
TUIContainer* BuildEditableUnitInfo(DMIKeys &dmi_keys, TUIContainer* unitinfo, CSVRow* csv_row, bool admin=false);
TUIContainer* BuildWriteConfirm(TUIContainer* confirm);
TUIContainer* CreateResultsScreen();

#endif // __SCREENS_H
