#include <stdlib.h>
#include "screens.h"

SelectArea* BuildModelsMenu(CSVFile* csv)
{
	SelectArea* models = new SelectArea();
	models->SetBackgroundColor(BLUE);
	models->SetXY(0, 1);
	models->SetWidth(80);
	models->AddFlag(TUI_NOCLOSE);

	int pcode_col = csv->GetColumnPos("PCODE");

	int y_pos = 1;
	int x_pos = 0;
	for ( int i=1; i < csv->GetNumRows(); i++ )
	{
		if ( i > 46 )
			break;

		CSVRow* row = csv->GetRow(i);
		CSVCell* cell = row->GetCell(0);
		char tmp[4];
		itoa(i, tmp, 10);
		apstring line(tmp);
		line.Pad(' ', 6-line.length());
		line += cell->GetValue();
		if ( row->GetCell(pcode_col)->GetValue() == "?" )
			line += " (CTO)";
		else if ( csv->NumMatchesInCSVCol(0, cell->GetValue()) > 1 )
			line += " ("+row->GetCell(pcode_col)->GetValue()+")";

		MenuItem* option = models->AddOption(line);
		option->SetXY(x_pos, y_pos);
		option->SetTextColor(GREY);
		option->SetBackgroundColor(BLUE);
		option->SetHighlightColor(CYAN);
		option->SetHighlightTextColor(WHITE);
		option->SetWidth(40);

		if ( y_pos % 23 == 0 ) // 23 lines used
		{
			models->SetHeight(23);
			x_pos += 40;
			y_pos = 1;
		}
		else
			y_pos++;
	}

	return models;
}

DropMenu* BuildFileMenu()
{
	DropMenu* menu = new DropMenu("File");
	menu->AddFlag(TUI_SAVEBFR);
	menu->SetBackgroundColor(GREY);
	menu->SetTextColor(BLACK);
	menu->SetHighlightColor(BLACK);
	menu->SetHighlightTextColor(GREY);
	menu->SetBorderType(SINGLE_LINE);
	menu->SetXY(0, 1);
	menu->SetWidth(11);

	MenuItem* option = menu->AddOption("View DMI");
	option->SetXY(1, 2);
	option->SetTextColor(BLACK);
	option->SetBackgroundColor(GREY);
	option->SetHighlightColor(BLACK);
	option->SetHighlightTextColor(GREY);
	option->SetWidth(-1);

	option = menu->AddOption("Update BIOS");
	option->SetXY(1, 3);
	option->SetTextColor(BLACK);
	option->SetBackgroundColor(GREY);
	option->SetHighlightColor(BLACK);
	option->SetHighlightTextColor(GREY);
	option->SetWidth(-1);

	option = menu->AddOption("     ");
	option->SetXY(1, 4);
	option->SetTextColor(BLACK);
	option->SetBackgroundColor(GREY);
	option->SetHighlightColor(BLACK);
	option->SetHighlightTextColor(GREY);
	option->CanSelect(false);
	option->AddFlag(TUI_BARENTRY);
	option->SetWidth(-1);

	option = menu->AddOption("Reboot");
	option->SetXY(1, 5);
	option->SetTextColor(BLACK);
	option->SetBackgroundColor(GREY);
	option->SetHighlightColor(BLACK);
	option->SetHighlightTextColor(GREY);
	option->SetWidth(-1);

	menu->DrawHeader();

	return menu;
}

DropMenu* BuildHelpMenu()
{
	DropMenu* menu = new DropMenu("Help");
	menu->AddFlag(TUI_SAVEBFR);
	menu->SetBackgroundColor(GREY);
	menu->SetTextColor(BLACK);
	menu->SetHighlightColor(BLACK);
	menu->SetHighlightTextColor(GREY);
	menu->SetBorderType(SINGLE_LINE);
	menu->SetXY(5, 1);
	//menu->SetWidth(11);

	MenuItem* option = menu->AddOption("About");
	option->SetXY(6, 2);
	option->SetTextColor(BLACK);
	option->SetBackgroundColor(GREY);
	option->SetHighlightColor(BLACK);
	option->SetHighlightTextColor(GREY);
	//option->SetWidth(-1);

	menu->DrawHeader();

	return menu;
}

TextInput* BuildLineJumpInput()
{
	TUIScreen->SetTextColor(BLACK);
	TUIScreen->SetBackgroundColor(GREY);
	TUIScreen->GotoXY(65, 24);
	disp_puts("Quick Select:");

	TextInput* input = new TextInput();
	input->SetBackgroundColor(BLACK);
	input->SetTextColor(WHITE);
	input->SetWidth(2);
	input->SetXY(78, 24);
	input->AddFlag(TUI_NUMONLY|TUI_WIPEONFILL);

	input->Draw();

	return input;
}

TUIContainer* BuildAboutMenu()
{
	TUIContainer* about = new TUIContainer();
	about->SetXY(15, 6);
	about->SetWidth(43);
	about->SetHeight(12);
	about->SetBackgroundColor(GREY);
	about->SetTextColor(BLACK);
	about->DrawBox(true);
	about->AddFlag(TUI_SAVEBFR);
	about->SetBorderType(SINGLE_LINE);

	TUIText* title = new TUIText("  SONY VAIO  ");
	title->SetWidth(13);
	title->SetTextColor(WHITE);
	title->SetY(6);
	title->Align(ALIGN_CENTER, about->GetX(), about->GetWidth());
	about->AddTUIObject(title);

	TUIText* version_l = new TUIText("DMI Front-End");
	version_l->SetXY(16, 7);
	version_l->SetBackgroundColor(GREY);
	version_l->SetTextColor(BLACK);
	about->AddTUIObject(version_l);
	TUIText* version_r = new TUIText("Version 1.02");
	version_r->SetY(7);
	version_r->Align(ALIGN_RIGHT, about->GetX()+about->GetWidth());
	version_r->SetBackgroundColor(GREY);
	version_r->SetTextColor(BLACK);
	about->AddTUIObject(version_r);

	TUIText* parser_l = new TUIText("DMI Parser");
	parser_l->SetXY(16, 8);
	parser_l->SetBackgroundColor(GREY);
	parser_l->SetTextColor(BLACK);
	about->AddTUIObject(parser_l);
	TUIText* parser_r = new TUIText(GetParserVersion());
	parser_r->SetY(8);
	parser_r->Align(ALIGN_RIGHT, about->GetX()+about->GetWidth());
	parser_r->SetBackgroundColor(GREY);
	parser_r->SetTextColor(BLACK);
	about->AddTUIObject(parser_r);

	TUIText* ctime_l = new TUIText("Compile Time");
	ctime_l->SetXY(16, 9);
	ctime_l->SetBackgroundColor(GREY);
	ctime_l->SetTextColor(BLACK);
	about->AddTUIObject(ctime_l);
	TUIText* ctime_r = new TUIText(__TIMESTAMP__);
	ctime_r->SetY(9);
	ctime_r->Align(ALIGN_RIGHT, about->GetX()+about->GetWidth());
	ctime_r->SetBackgroundColor(GREY);
	ctime_r->SetTextColor(BLACK);
	about->AddTUIObject(ctime_r);

	TUIText* tdate_l = new TUIText("Tool Date");
	tdate_l->SetXY(16, 11);
	tdate_l->SetBackgroundColor(GREY);
	tdate_l->SetTextColor(BLACK);
	about->AddTUIObject(tdate_l);
	TUIText* tdate_r = new TUIText(apstring(getenv("TOOL_DATE") != NULL ? getenv("TOOL_DATE") : "ERROR"));
	tdate_r->SetY(11);
	tdate_r->Align(ALIGN_RIGHT, about->GetX()+about->GetWidth());
	tdate_r->SetBackgroundColor(GREY);
	tdate_r->SetTextColor(BLACK);
	about->AddTUIObject(tdate_r);

	TUIText* bios_l = new TUIText("BIOS Image");
	bios_l->SetXY(16, 13);
	bios_l->SetBackgroundColor(GREY);
	bios_l->SetTextColor(BLACK);
	about->AddTUIObject(bios_l);
	TUIText* bios_r = new TUIText(apstring(getenv("BIOS_IMG") != NULL ? getenv("BIOS_IMG") : "ERROR"));
	bios_r->SetY(13);
	bios_r->Align(ALIGN_RIGHT, about->GetX()+about->GetWidth());
	bios_r->SetBackgroundColor(GREY);
	bios_r->SetTextColor(BLACK);
	about->AddTUIObject(bios_r);

	TUIText* author = new TUIText("Created by Austin Heilman");
	author->SetXY(16, 17);
	author->SetBackgroundColor(GREY);
	author->SetTextColor(BLACK);
	about->AddTUIObject(author);

	return about;
}

TUIContainer* BuildCurrentUnitInfo(DMIKeys &dmi_keys)
{
	TUIContainer* unitinfo = new TUIContainer();
	unitinfo->SetXY(6, 2);
	unitinfo->SetWidth(65);
	unitinfo->SetHeight(16);
	unitinfo->SetBackgroundColor(GREY);
	unitinfo->SetTextColor(BLACK);
	unitinfo->SetBorderType(DOUBLE_LINE);
	unitinfo->AddFlag(TUI_SAVEBFR);
	unitinfo->DrawBox(true);

	TUIText* title = new TUIText("  CURRENT DMI INFORMATION  ");
	title->SetY(2);
	title->Align(ALIGN_CENTER, unitinfo->GetX(), unitinfo->GetWidth());
	title->SetTextColor(WHITE);
	title->SetBackgroundColor(BLACK);
	unitinfo->AddTUIObject(title);

	TUIText* mname = new TUIText("Model Name컴컴컴컴");
	mname->SetXY(8, 4);
	mname->SetTextColor(BLACK);
	mname->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(mname);

	TUIText* mname_val = new TUIText(GetDMIKey("MNAME", dmi_keys)->GetDefaultValue());
	mname_val->SetXY(26, 4);
	mname_val->SetTextColor(BLACK);
	mname_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(mname_val);

	TUIText* pcode = new TUIText("Product Code컴컴컴");
	pcode->SetXY(8, 6);
	pcode->SetTextColor(BLACK);
	pcode->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(pcode);

	TUIText* pcode_val = new TUIText(GetDMIKey("PCODE", dmi_keys)->GetDefaultValue());
	pcode_val->SetXY(26, 6);
	pcode_val->SetTextColor(BLACK);
	pcode_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(pcode_val);

	TUIText* serial = new TUIText("Serial Number컴컴�");
	serial->SetXY(8, 8);
	serial->SetTextColor(BLACK);
	serial->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(serial);

	TUIText* serial_val = new TUIText(GetDMIKey("SERIAL", dmi_keys)->GetDefaultValue());
	serial_val->SetXY(26, 8);
	serial_val->SetTextColor(BLACK);
	serial_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(serial_val);

	TUIText* stag = new TUIText("Service Tag컴컴컴�");
	stag->SetXY(8, 10);
	stag->SetTextColor(BLACK);
	stag->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(stag);

	TUIText* stag_val = new TUIText(GetDMIKey("STAG", dmi_keys)->GetDefaultValue());
	stag_val->SetXY(26, 10);
	stag_val->SetTextColor(BLACK);
	stag_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(stag_val);

	TUIText* mcode = new TUIText("M Code컴컴컴컴컴컴");
	mcode->SetXY(8, 12);
	mcode->SetTextColor(BLACK);
	mcode->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(mcode);

	apstring mcode_tmp = GetDMIKey("MCODE", dmi_keys)->GetDefaultValue();
	TUIText* mcode_val = new TUIText(mcode_tmp);
	mcode_val->SetXY(26, 12);
	mcode_val->SetTextColor(BLACK);
	mcode_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(mcode_val);

	TUIText* blid = new TUIText("BIOS Lock컴컴컴컴�");
	blid->SetXY(8, 14);
	blid->SetTextColor(BLACK);
	blid->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(blid);

	TUIText* blid_val = new TUIText(GetDMIKey("BLID", dmi_keys)->GetDefaultValue());
	blid_val->SetXY(26, 14);
	blid_val->SetTextColor(BLACK);
	blid_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(blid_val);

	TUIText* uuid = new TUIText("UUID컴컴컴컴컴컴컴");
	uuid->SetXY(8, 16);
	uuid->SetTextColor(BLACK);
	uuid->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(uuid);

	TUIText* uuid_val = new TUIText("NONE");
	DMIKey* uuid_tmp = GetDMIKey("UUID", dmi_keys);
	if ( uuid_tmp )
		uuid_val->SetTextLine(uuid_tmp->GetDefaultValue(), 0);
	uuid_val->SetXY(26, 16);
	uuid_val->SetTextColor(BLACK);
	uuid_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(uuid_val);
	/*
	TUIText* esid = new TUIText("ESID컴컴컴컴컴컴컴");
	esid->SetXY(8, 18);
	esid->SetTextColor(BLACK);
	esid->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(esid);

	TUIText* esid_val = new TUIText("NONE");
	DMIKey* esid_tmp = GetDMIKey("ESID", dmi_keys);
	if ( esid_tmp )
		esid_val->SetTextLine(esid_tmp->GetDefaultValue(), 0);
	esid_val->SetXY(26, 18);
	esid_val->SetTextColor(BLACK);
	esid_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(esid_val);
	*/

	return unitinfo;
}

TUIContainer* BuildEditableUnitInfo(DMIKeys &dmi_keys, TUIContainer* unitinfo, CSVRow* csv_row, bool admin)
{
	if ( unitinfo )
	{
		delete unitinfo;
		unitinfo = NULL;
		for ( int i=0; i<dmi_keys.size(); i++ )
			dmi_keys[i].SetEntryRef(NULL);
	}
	apstring tmp_val; // Temporarily copy a value

	unitinfo = new TUIContainer();
	unitinfo->SetXY(6, 2);
	unitinfo->SetWidth(65);
	unitinfo->SetHeight(18);
	unitinfo->SetBackgroundColor(GREY);
	unitinfo->SetTextColor(BLACK);
	unitinfo->SetBorderType(DOUBLE_LINE);
	unitinfo->AddFlag(TUI_SAVEBFR);
	unitinfo->DrawBox(true);

	TUIText* title = new TUIText("  SET DMI INFORMATION  ");
	title->SetY(2);
	title->Align(ALIGN_CENTER, unitinfo->GetX(), unitinfo->GetWidth());
	title->SetTextColor(WHITE);
	title->SetBackgroundColor(BLACK);
	unitinfo->AddTUIObject(title);

	TUIText* mname = new TUIText("Model Name컴컴컴컴");
	mname->SetXY(8, 4);
	mname->SetTextColor(BLACK);
	mname->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(mname);
	if ( admin )
	{
		TextInput* mname_val = new TextInput();
		mname_val->SetXY(26, 4);
		mname_val->SetTextColor(WHITE);
		mname_val->SetBackgroundColor(BLACK);
		mname_val->SetWidth(12);
		mname_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE);
		unitinfo->AddTUIObject(mname_val);
		GetDMIKey("MNAME", dmi_keys)->SetEntryRef(mname_val);
	}
	else
	{
		TUIText* mname_val = new TUIText(GetDMIKey("MNAME", dmi_keys)->GetValue(csv_row));
		mname_val->SetXY(26, 4);
		mname_val->SetTextColor(BLACK);
		mname_val->SetBackgroundColor(GREY);
		unitinfo->AddTUIObject(mname_val);
	}

	TUIText* pcode = new TUIText("Product Code컴컴컴");
	pcode->SetXY(8, 6);
	pcode->SetTextColor(BLACK);
	pcode->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(pcode);

	if ( csv_row )
		tmp_val = csv_row->GetCell(csv_row->GetColumnPos("PCODE"))->GetValue();
	else
		tmp_val = "";
	if ( tmp_val == "?" || admin )
	{
		TextInput* pcode_val = new TextInput(GetDMIKey("PCODE", dmi_keys)->GetValue(csv_row));
		pcode_val->SetXY(26, 6);
		pcode_val->SetTextColor(WHITE);
		pcode_val->SetBackgroundColor(BLACK);
		pcode_val->SetWidth(8);
		pcode_val->AddFlag(TUI_NUMONLY|TUI_WIPEONFUSE);
		unitinfo->AddTUIObject(pcode_val);
		GetDMIKey("PCODE", dmi_keys)->SetEntryRef(pcode_val);
	}
	else
	{
		TUIText* pcode_val = new TUIText(tmp_val);
		pcode_val->SetXY(26, 6);
		pcode_val->SetTextColor(BLACK);
		pcode_val->SetBackgroundColor(GREY);
		unitinfo->AddTUIObject(pcode_val);
	}

	TUIText* serial = new TUIText("Serial Number컴컴�");
	serial->SetXY(8, 8);
	serial->SetTextColor(BLACK);
	serial->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(serial);

	TextInput* serial_val = new TextInput(GetDMIKey("SERIAL", dmi_keys)->GetDefaultValue());
	serial_val->SetXY(26, 8);
	serial_val->SetTextColor(WHITE);
	serial_val->SetBackgroundColor(BLACK);
	serial_val->SetWidth(7);
	serial_val->AddFlag(TUI_NUMONLY|TUI_WIPEONFUSE);
	unitinfo->AddTUIObject(serial_val);
	GetDMIKey("SERIAL", dmi_keys)->SetEntryRef(serial_val);

	TUIText* stag = new TUIText("Service Tag컴컴컴�");
	stag->SetXY(8, 10);
	stag->SetTextColor(BLACK);
	stag->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(stag);

	TextInput* stag_val = new TextInput(GetDMIKey("STAG", dmi_keys)->GetDefaultValue());
	stag_val->SetXY(26, 10);
	stag_val->SetTextColor(WHITE);
	stag_val->SetBackgroundColor(BLACK);
	stag_val->SetWidth(8);
	stag_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE|TUI_ALNUMONLY);
	unitinfo->AddTUIObject(stag_val);
	GetDMIKey("STAG", dmi_keys)->SetEntryRef(stag_val);

	TUIText* mcode = new TUIText("M Code컴컴컴컴컴컴");
	mcode->SetXY(8, 12);
	mcode->SetTextColor(BLACK);
	mcode->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(mcode);

	if ( admin || csv_row->GetColumnPos("MCODE") != -1 )
	{
		if ( csv_row )
			tmp_val = csv_row->GetCell(csv_row->GetColumnPos("MCODE"))->GetValue();
		else
			tmp_val = "";

		if ( tmp_val == "?" || admin )
		{
			apstring tmpcode;
			tmpcode += GetDMIKey("PMCODE", dmi_keys)->GetValue(csv_row);
			tmpcode += GetDMIKey("HWMCODE", dmi_keys)->GetValue(csv_row);
			tmpcode += GetDMIKey("SWMCODE", dmi_keys)->GetValue(csv_row);

			TextInput* mcode_val = new TextInput(tmpcode);
			mcode_val->SetXY(26, 12);
			mcode_val->SetTextColor(WHITE);
			mcode_val->SetBackgroundColor(BLACK);
			mcode_val->SetWidth(24);
			mcode_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE);
			unitinfo->AddTUIObject(mcode_val);
			GetDMIKey("MCODE", dmi_keys)->SetEntryRef(mcode_val);
		}
		else
		{
			TUIText* mcode_val = new TUIText(tmp_val);
			mcode_val->SetXY(26, 12);
			mcode_val->SetTextColor(BLACK);
			mcode_val->SetBackgroundColor(GREY);
			unitinfo->AddTUIObject(mcode_val);
		}
	}
	else // OLDER MCODE COMPATABILITY
	{
		tmp_val = csv_row->GetCell(csv_row->GetColumnPos("PMCODE"))->GetValue();
		if ( tmp_val == "?" || admin )
		{
			TextInput* mcode_val = new TextInput(GetDMIKey("PMCODE", dmi_keys)->GetValue(csv_row));
			mcode_val->SetXY(26, 12);
			mcode_val->SetTextColor(WHITE);
			mcode_val->SetBackgroundColor(BLACK);
			mcode_val->SetWidth(4);
			mcode_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE);
			unitinfo->AddTUIObject(mcode_val);
			GetDMIKey("PMCODE", dmi_keys)->SetEntryRef(mcode_val);
		}
		else
		{
			TUIText* mcode_val = new TUIText(tmp_val);
			mcode_val->SetXY(26, 12);
			mcode_val->SetTextColor(BLACK);
			mcode_val->SetBackgroundColor(GREY);
			unitinfo->AddTUIObject(mcode_val);
		}

		tmp_val = csv_row->GetCell(csv_row->GetColumnPos("HWMCODE"))->GetValue();
		if ( tmp_val == "?" || admin )
		{
			TextInput* mcode_val = new TextInput(GetDMIKey("HWMCODE", dmi_keys)->GetValue(csv_row));
			mcode_val->SetXY(31, 12);
			mcode_val->SetTextColor(WHITE);
			mcode_val->SetBackgroundColor(BLACK);
			mcode_val->SetWidth(8);
			mcode_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE);
			unitinfo->AddTUIObject(mcode_val);
			GetDMIKey("HWMCODE", dmi_keys)->SetEntryRef(mcode_val);
		}
		else
		{
			TUIText* mcode_val = new TUIText(tmp_val);
			mcode_val->SetXY(31, 12);
			mcode_val->SetTextColor(BLACK);
			mcode_val->SetBackgroundColor(GREY);
			unitinfo->AddTUIObject(mcode_val);
		}

		tmp_val = csv_row->GetCell(csv_row->GetColumnPos("SWMCODE"))->GetValue();
		if ( tmp_val == "?" || admin )
		{
			TextInput* mcode_val = new TextInput(GetDMIKey("SWMCODE", dmi_keys)->GetValue(csv_row));
			mcode_val->SetXY(40, 12);
			mcode_val->SetTextColor(WHITE);
			mcode_val->SetBackgroundColor(BLACK);
			mcode_val->SetWidth(12);
			mcode_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE);
			unitinfo->AddTUIObject(mcode_val);
			GetDMIKey("SWMCODE", dmi_keys)->SetEntryRef(mcode_val);
		}
		else
		{
			TUIText* mcode_val = new TUIText(tmp_val);
			mcode_val->SetXY(40, 12);
			mcode_val->SetTextColor(BLACK);
			mcode_val->SetBackgroundColor(GREY);
			unitinfo->AddTUIObject(mcode_val);
		}
	}

	TUIText* blid = new TUIText("BIOS Lock컴컴컴컴�");
	blid->SetXY(8, 14);
	blid->SetTextColor(BLACK);
	blid->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(blid);

	if ( csv_row )
		tmp_val = csv_row->GetCell(csv_row->GetColumnPos("BLID"))->GetValue();
	else
		tmp_val = "";
	if ( tmp_val == "?" || admin )
	{
		TextInput* blid_val = new TextInput(GetDMIKey("BLID", dmi_keys)->GetValue(csv_row));
		blid_val->SetXY(26, 14);
		blid_val->SetTextColor(WHITE);
		blid_val->SetBackgroundColor(BLACK);
		blid_val->SetWidth(10);
		blid_val->AddFlag(TUI_FORCEUPPER|TUI_WIPEONFUSE);
		unitinfo->AddTUIObject(blid_val);
		GetDMIKey("BLID", dmi_keys)->SetEntryRef(blid_val);
	}
	else
	{
		TUIText* blid_val = new TUIText(tmp_val);
		blid_val->SetXY(26, 14);
		blid_val->SetTextColor(BLACK);
		blid_val->SetBackgroundColor(GREY);
		unitinfo->AddTUIObject(blid_val);
	}

	TUIText* uuid = new TUIText("UUID컴컴컴컴컴컴컴");
	uuid->SetXY(8, 16);
	uuid->SetTextColor(BLACK);
	uuid->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(uuid);

	TUIText* uuid_val = new TUIText("NONE");
	DMIKey* uuid_tmp = GetDMIKey("UUID", dmi_keys);
	if ( uuid_tmp )
		uuid_val->SetTextLine(uuid_tmp->GetDefaultValue(), 0);
	uuid_val->SetXY(26, 16);
	uuid_val->SetTextColor(BLACK);
	uuid_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(uuid_val);

	/*
	TUIText* esid = new TUIText("ESID컴컴컴컴컴컴컴");
	esid->SetXY(8, 18);
	esid->SetTextColor(BLACK);
	esid->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(esid);

	TUIText* esid_val = new TUIText("NONE");
	DMIKey* esid_tmp = GetDMIKey("ESID", dmi_keys);
	if ( esid_tmp )
		esid_val->SetTextLine(esid_tmp->GetDefaultValue(), 0);
	esid_val->SetXY(26, 18);
	esid_val->SetTextColor(BLACK);
	esid_val->SetBackgroundColor(GREY);
	unitinfo->AddTUIObject(esid_val);
	*/

	TUIButton* write_btn = new TUIButton(" WRITE DMI ", 1);
	write_btn->SetXY(40, 18);
	write_btn->Align(ALIGN_CENTER, unitinfo->GetX(), unitinfo->GetWidth());
	write_btn->SetHighlightColor(BLACK);
	write_btn->SetHighlightTextColor(WHITE);
	unitinfo->AddTUIObject(write_btn);

	return unitinfo;
}

TUIContainer* BuildWriteConfirm(TUIContainer* confirm)
{
	if ( confirm )
	{
		delete confirm;
		confirm = NULL;
	}

	confirm = new TUIContainer();
	confirm->SetXY(15, 6);
	confirm->SetWidth(37);
	confirm->SetHeight(7);
	confirm->SetBackgroundColor(GREY);
	confirm->SetTextColor(BLACK);
	confirm->DrawBox(true);
	confirm->AddFlag(TUI_SAVEBFR);
	confirm->SetBorderType(SINGLE_LINE);

	TUIText* title = new TUIText("  WRITE CONFIRM  ");
	title->SetTextColor(WHITE);
	title->SetY(6);
	title->Align(ALIGN_CENTER, confirm->GetX(), confirm->GetWidth());
	confirm->AddTUIObject(title);

	TUIText* question = new TUIText("Is it O.K. to write this DMI data?");
	question->SetXY(17, 9);
	question->SetBackgroundColor(GREY);
	question->SetTextColor(BLACK);
	confirm->AddTUIObject(question);

	TUIButton* yes_btn = new TUIButton(" YES ", 2);
	yes_btn->SetXY(confirm->GetX()+6, 12);
	yes_btn->SetHighlightColor(BLACK);
	yes_btn->SetHighlightTextColor(WHITE);
	yes_btn->SetHotKey(KeyInput(5497));
	confirm->AddTUIObject(yes_btn);

	TUIButton* no_btn = new TUIButton(" NO ", 3);
	no_btn->SetY(12);
	no_btn->Align(ALIGN_RIGHT, (confirm->GetX()+confirm->GetWidth())-6);
	no_btn->SetHighlightColor(BLACK);
	no_btn->SetHighlightTextColor(WHITE);
	confirm->AddTUIObject(no_btn);
	no_btn->SetHotKey(KeyInput(12654));
	no_btn->Selected(true);
	confirm->SetFocus(no_btn);

	return confirm;
}

TUIContainer* CreateResultsScreen()
{
	TUIScreen->SetTextColor(GREY);
	TUIScreen->SetBackgroundColor(BLACK);
	for ( short x=0; x<=80; x++ )
	{
		for ( short y=0; y<=24; y++ )
		{
			TUIScreen->GotoXY(x, y);
			disp_puts(" ");
		}
	}

	TUIText* title = new TUIText("[ WRITING DMI - PLEASE WAIT ]");
	title->SetBackgroundColor(BLACK);
	title->SetTextColor(WHITE);
	title->SetY(0);
	title->Align(ALIGN_CENTER, 0, 80);
	title->Draw();

	TUIScreen->GotoXY(0, 1);

	return NULL;
}
