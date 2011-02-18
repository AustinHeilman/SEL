#include <fstream.h>
#include <stdlib.h>
#include <process.h>
#include "dmiinfo.h"
#include "screens.h"
#include "readfile.h"
#include "keyboard.h"

#if DMITOOL == 4

#pragma message("----------")
#pragma message("** COMPILING FOR SONY ROMINFO6.EXE HANDLING **")
#pragma message("----------")

apstring GetParserVersion()
{
	return apstring(__FILE__)+" 1.0";
}

DMIKeys ReadDMIDefaults()
{
	DMIKeys dmi_keys;
	ReadDMIDefaults(dmi_keys);

	return dmi_keys;
}

DMIKeys ReadDMIDefaults(DMIKeys &dmi_keys)
{
	if ( dmi_keys.size() > 0 )
		dmi_keys.resize(0);

	apvector<apstring> file_lines = ReadFile(getenv("DEFAULTS_FILE"));
	apstring retval;
	apstring key;

	key = "1:Product Name...[";
	dmi_keys.append(DMIKey("MNAME", GetStructureField(file_lines, key)));
	key = "2:Service Tag....[";
	dmi_keys.append(DMIKey("STAG", GetStructureField(file_lines, key)));

	key = "3:Serial Number..[";
	retval = GetStructureField(file_lines, key);
	int pos = retval.find("-");
	dmi_keys.append(DMIKey("PCODE", retval.substr(0, pos)));
	dmi_keys.append(DMIKey("SERIAL", retval.substr(pos+1, 7)));

	key = "4:UUID...........[";
	dmi_keys.append(DMIKey("UUID", GetStructureField(file_lines, key)));

	key = "5:BIOS Lock ID...[";
	dmi_keys.append(DMIKey("BLID", GetStructureField(file_lines, key)));

	key = "7:Model Code.....[";
	retval = GetStructureField(file_lines, key);
	dmi_keys.append(DMIKey("MCODE", retval));
	dmi_keys.append(DMIKey("PMCODE", retval.substr(0, 4)));
	dmi_keys.append(DMIKey("HWMCODE", retval.substr(5, 8)));
	dmi_keys.append(DMIKey("SWMCODE", retval.substr(14, 12)));

	key = "6:Sony String....[";
	retval = GetStructureField(file_lines, key);
	dmi_keys.append(DMIKey("EXFUNC", retval));
	return dmi_keys;
}

// Removes the "](###)" at the end of ROMINFO6 information
apstring CleanDMIKey(apstring line)
{
	int pos = line.find("]");
	if ( pos == npos )
		pos = line.length();

	return line.substr(0, pos);
}

apstring GetStructureField(apvector<apstring> &file_lines, apstring &key)
{
	for ( int i=0; i<file_lines.size(); i++ )
	{
		apstring line = file_lines[i];
		if ( line.length() < key.length() )
			continue;
		else if ( line.substr(0, key.length()) == key )
			return CleanDMIKey(line.substr(key.length(), line.length()));
	}

	cerr << "Fatal Error: GetStructureField() failed: " << key << endl;
	cout << "--- LINES ---\n";
	for ( int i=0; i<file_lines.size(); i++ )
	{
		cout << file_lines[i] << endl;
	}
	assert(0);
	return apstring("");
}

void CreateDMIDefaultsFile()
{
	system("%SONY_DIR%\\ROMINFO6.EXE >%DEFAULTS_FILE%");
}

void WriteSystemDMI(DMIKeys &dmi_keys, CSVRow* csv_row)
{
	CreateResultsScreen();

	// GEMCODE HANDLING FIRST TO ENCRYPT THE MCODE
	apstring mcode = GetEncryptedMCode(dmi_keys, csv_row);

	apstring args("-m");
	args += " -1 "+GetDMIKey("MNAME", dmi_keys)->GetValue(csv_row);
	args += " -2 "+GetDMIKey("STAG", dmi_keys)->GetValue(csv_row);
	args += " -3 "+GetDMIKey("PCODE", dmi_keys)->GetValue(csv_row)+"-"+GetDMIKey("SERIAL", dmi_keys)->GetValue();
	args += " -5 "+GetDMIKey("BLID", dmi_keys)->GetValue(csv_row);
	args += " -6 "+GetDMIKey("EXFUNC", dmi_keys)->GetValue(csv_row);
	args += " -7 "+mcode;

	//ROMINFO6.EXE -m -1 %MNAME% -2 %STAG% -3 %PCODE%-%SERIAL% -5 %BLID% -7 %ExMCODE%
	int retval = _spawnlp(_P_WAIT, "ROMINFO6.EXE", "ROMINFO6.EXE", args.c_str(), NULL);
	if ( retval != 0 )
	{
		cerr << "WriteSystemDMI() 'ROMINFO6.EXE' Error - Retval was '"<<retval<<"'\n";
		cerr << "Arguments:\n";
		cerr << args << "\n\n";
		assert(0);
	}

	apstring done_msg = "    ** PRESS ANY KEY TO REBOOT COMPUTER **    ";
	TUIScreen->GotoXY(CalcPosition(ALIGN_CENTER, 0, done_msg.length(), 80), 24);
	TUIScreen->SetBackgroundColor(BLACK);
	TUIScreen->SetTextColor(GREEN);
	disp_puts(done_msg.c_str());
	Getch();
}

#endif // DMITOOL==1

// ----- END RID.EXE HANDLING
