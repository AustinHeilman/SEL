#include <fstream.h>
#include <stdlib.h>
#include <process.h>
#include "dmiinfo.h"
#include "screens.h"
#include "readfile.h"
#include "keyboard.h"

#if DMITOOL == 2

#pragma message("----------")
#pragma message("** COMPILING FOR QUANTA DMI.EXE HANDLING **")
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

	key = "System product name       (/DPS)= ";
	dmi_keys.append(DMIKey("MNAME", GetStructureField(file_lines, key)));
	key = "System version            (/DVS)= ";
	dmi_keys.append(DMIKey("STAG", GetStructureField(file_lines, key)));

	key = "System serial number      (/DSS)= ";
	retval = GetStructureField(file_lines, key);
	int pos = retval.find("-");
	dmi_keys.append(DMIKey("PCODE", retval.substr(0, pos)));
	dmi_keys.append(DMIKey("SERIAL", retval.substr(pos+1, 7)));

	key = "UUID                      (/DUS)= ";
	dmi_keys.append(DMIKey("UUID", GetStructureField(file_lines, key)));

	key = "OEM string 1              (/DO0)= ";
	dmi_keys.append(DMIKey("BLID", GetStructureField(file_lines, key)));

	key = "OEM string 3              (/DO2)= ";
	retval = GetStructureField(file_lines, key);
	dmi_keys.append(DMIKey("MCODE", retval));
	dmi_keys.append(DMIKey("PMCODE", retval.substr(0, 4))); //0-4
	dmi_keys.append(DMIKey("HWMCODE", retval.substr(4, 8))); //4-12
	dmi_keys.append(DMIKey("SWMCODE", retval.substr(12, 12))); //12-length

	key = "OEM string 2              (/DO1)= ";
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
		else if ( line.substr(1, key.length()) == key )
			return line.substr(key.length()+1, line.length());
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
	system("%SONY_DIR%\\DMI.EXE /B >%DEFAULTS_FILE%");
}

void WriteSystemDMI(DMIKeys &dmi_keys, CSVRow* csv_row)
{
	CreateResultsScreen();

	// GEMCODE HANDLING FIRST TO ENCRYPT THE MCODE
	apstring mcode = GetEncryptedMCode(dmi_keys, csv_row);

	apstring args("");
	args += " /dvs:"+GetDMIKey("STAG", dmi_keys)->GetValue(csv_row);
	args += " /dps:"+GetDMIKey("MNAME", dmi_keys)->GetValue(csv_row);
	args += " /dss:"+GetDMIKey("PCODE", dmi_keys)->GetValue(csv_row)+"-"+GetDMIKey("SERIAL", dmi_keys)->GetValue();
	args += " /do0:"+GetDMIKey("BLID", dmi_keys)->GetValue(csv_row);
	args += " /do1:"+GetDMIKey("BLID", dmi_keys)->GetValue(csv_row);
	args += " /do2:"+mcode;
	args += " /CPR";

	//DMI.EXE /dvs:%STAG% /dps:%MNAME% /dss:%PCODE%-%SERIAL% /do0:%BLID% /do1:%ExFUNC% /do2:%ExMCODE% /CPR
	int retval = _spawnlp(_P_WAIT, "dmi.exe", "dmi.exe", args.c_str(), NULL);
	if ( retval != 0 )
	{
		cerr << "WriteSystemDMI() 'DMI.EXE' Error - Retval was '"<<retval<<"'\n";
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
