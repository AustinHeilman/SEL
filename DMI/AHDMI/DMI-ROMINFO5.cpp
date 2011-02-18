#include <fstream.h>
#include <stdlib.h>
#include <process.h>
#include "dmiinfo.h"
#include "screens.h"
#include "readfile.h"
#include "keyboard.h"

#if DMITOOL == 3

#pragma message("----------")
#pragma message("** COMPILING FOR SONY ROMINFO5.EXE HANDLING **")
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

	key = "Model Name............";
	dmi_keys.append(DMIKey("MNAME", GetStructureField(file_lines, key)));
	key = "Service Tag...........";
	dmi_keys.append(DMIKey("STAG", GetStructureField(file_lines, key)));

	key = "Serial Number.........";
	retval = GetStructureField(file_lines, key);
	int pos = retval.find("-");
	dmi_keys.append(DMIKey("PCODE", retval.substr(0, pos)));
	dmi_keys.append(DMIKey("SERIAL", retval.substr(pos+1, 7)));

	key = "UUID..................";
	dmi_keys.append(DMIKey("UUID", GetStructureField(file_lines, key)));

	key = "BIOS Lock No..........";
	dmi_keys.append(DMIKey("BLID", GetStructureField(file_lines, key)));

	key = "Model Code............";
	retval = GetStructureField(file_lines, key);
	dmi_keys.append(DMIKey("MCODE", retval));
	dmi_keys.append(DMIKey("PMCODE", retval.substr(0, 4)));
	dmi_keys.append(DMIKey("HWMCODE", retval.substr(5, 8)));
	dmi_keys.append(DMIKey("SWMCODE", retval.substr(14, 12)));

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
	system("%SONY_DIR%\\ROMINFO5.EXE >%DEFAULTS_FILE%");
}

void WriteSystemDMI(DMIKeys &dmi_keys, CSVRow* csv_row)
{
	CreateResultsScreen();

	// GEMCODE HANDLING FIRST TO ENCRYPT THE MCODE
	apstring mcode = GetEncryptedMCode(dmi_keys, csv_row);

	apstring args("-u");
	args += " "+GetDMIKey("MNAME", dmi_keys)->GetValue(csv_row);
	args += " "+GetDMIKey("STAG", dmi_keys)->GetValue(csv_row);
	args += " "+GetDMIKey("PCODE", dmi_keys)->GetValue(csv_row)+"-"+GetDMIKey("SERIAL", dmi_keys)->GetValue();
	args += " 0x00000000";
	args += " blno="+GetDMIKey("BLID", dmi_keys)->GetValue(csv_row);
	args += " mcode="+mcode;
	args += " esid=0x00000000";

	//ROMINFO5.EXE -u %MNAME% %STAG% %PCODE%-%SERIAL% 0x%UINFO% blno=%BLID% mcode=%ExMCODE% esid=%ESID%
	_spawnlp(_P_WAIT, "rominfo5.exe", "rominfo5.exe", "-u -e", NULL);
	int retval = _spawnlp(_P_WAIT, "rominfo5.exe", "rominfo5.exe", args.c_str(), NULL);
	if ( retval != 0 )
	{
		cerr << "WriteSystemDMI() 'ROMINFO5.EXE' Error - Retval was '"<<retval<<"'\n";
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
