#include <fstream.h>
#include <stdlib.h>
#include <process.h>
#include "dmiinfo.h"
#include "screens.h"
#include "readfile.h"
#include "keyboard.h"

#if DMITOOL == 1

#pragma message("----------")
#pragma message("** COMPILING FOR RID.EXE HANDLING **")
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

	key = "Product Name = ";
	dmi_keys.append(DMIKey("MNAME", GetStructureField(file_lines, key)));
	key = "Version = ";
	dmi_keys.append(DMIKey("STAG", GetStructureField(file_lines, key)));

	key = "Serial Number = ";
	retval = GetStructureField(file_lines, key);
	int pos = retval.find("-");
	dmi_keys.append(DMIKey("PCODE", retval.substr(0, pos)));
	dmi_keys.append(DMIKey("SERIAL", retval.substr(pos+1, 7)));

	key = "UUID = ";
	dmi_keys.append(DMIKey("UUID", GetStructureField(file_lines, key)));

	key = "Specific String 1 = ";
	dmi_keys.append(DMIKey("BLID", GetStructureField(file_lines, key)));

	key = "Specific String 3 = ";
	retval = GetStructureField(file_lines, key);
	dmi_keys.append(DMIKey("MCODE", retval));
	dmi_keys.append(DMIKey("PMCODE", retval.substr(0, 4))); //0-4
	dmi_keys.append(DMIKey("HWMCODE", retval.substr(4, 8))); //4-12
	dmi_keys.append(DMIKey("SWMCODE", retval.substr(12, 12))); //12-length

	key = "Specific String 2 = ";
	retval = GetStructureField(file_lines, key);
	dmi_keys.append(DMIKey("EXFUNC", retval));

	return dmi_keys;
}

apstring GetStructureField(apvector<apstring> &file_lines, apstring &key)
{
	for ( int i=0; i<file_lines.size(); i++ )
	{
		apstring line = file_lines[i];
		if ( line.length() < key.length() )
			continue;
		else if ( line.substr(0, key.length()) == key )
			return line.substr(key.length(), line.length());
	}

	cerr << "Fatal Error: GetStructureField() failed: " << key;
	assert(0);
	return apstring("");
}

void CreateDMIDefaultsFile()
{
	system("%SONY_DIR%\\RID.EXE >%DEFAULTS_FILE%");
}

void WriteSystemDMI(DMIKeys &dmi_keys, CSVRow* csv_row)
{
	CreateResultsScreen();

	// GEMCODE HANDLING FIRST TO ENCRYPT THE MCODE
	apstring mcode = GetEncryptedMCode(dmi_keys, csv_row);

	apstring args("");
	args += "-m";
	args += " -1 "+GetDMIKey("MNAME", dmi_keys)->GetValue(csv_row);
	args += " -2 "+GetDMIKey("STAG", dmi_keys)->GetValue(csv_row);
	args += " -3 "+GetDMIKey("PCODE", dmi_keys)->GetValue(csv_row)+"-"+GetDMIKey("SERIAL", dmi_keys)->GetValue();
	args += " -5 "+GetDMIKey("BLID", dmi_keys)->GetValue(csv_row);
	args += " -6 "+GetDMIKey("EXFUNC", dmi_keys)->GetValue(csv_row);
	args += " -7 "+mcode;

	//RID.EXE -m -1 %MNAME% -2 %STAG% -3 %PCODE%-%SERIAL% -5 %BLID% -6 FNC-EXTB -7 %ExMCODE%
	int retval = _spawnlp(_P_WAIT, "RID.EXE", "RID.EXE", args.c_str(), NULL);
	if ( retval != 0 )
	{
		cerr << "WriteSystemDMI() 'RID.EXE' Error - Retval was '"<<retval<<"'\n";
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
