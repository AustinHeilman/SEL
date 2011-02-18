#include <fstream.h>
#include <process.h>
#include "dmiinfo.h"
#include "csv.h"
#include "tui.h"

/*
 * Generic Utility Functions
 * DMI Related
 *
 */

bool ValidateDMIEntry(DMIKey* dmikey)
{
	if ( dmikey == NULL )
		return true;

	TextInput* input_box = dmikey->GetEntryRef();
	if ( !input_box )
		return true; // No user enterable data

	apstring value = input_box->GetStringValue();
	if ( dmikey->GetName() == "MNAME" )
	{
		return true; // Only used in admin-mode.
	}
	if ( dmikey->GetName() == "PCODE" )
	{
		if ( value.length() >= 7 )
			return true;
	}
	if ( dmikey->GetName() == "SERIAL" )
	{
		if ( strcmp(getenv("ENFORCE_SERIAL"), "OFF") != 0 )
		{
			// If serial number checking is enabled:
			// Serial must start with '3' or be '1234567'
			if ( value == "1234567" )
				return true;
			if ( value.substr(0,1) != "3" )
				return false;
		}
	}
	else if ( dmikey->GetName() == "STAG" )
	{
		if ( value == "01" )
			return true;
	}

	if ( input_box->GetStringValue().length() < input_box->GetWidth() )
		return false;

	return true;
}

apstring GetEncryptedMCode(DMIKeys &dmi_keys, CSVRow* csv_row)
{
	apstring mcode("");
	{
		if ( csv_row->GetColumnPos("MCODE") != -1 ) // Single MCode style (new)
		{
			mcode += GetDMIKey("MCODE", dmi_keys)->GetValue(csv_row);
		}
		else
		{
			mcode += GetDMIKey("PMCODE", dmi_keys)->GetValue(csv_row);
			mcode += GetDMIKey("HWMCODE", dmi_keys)->GetValue(csv_row);
			mcode += GetDMIKey("SWMCODE", dmi_keys)->GetValue(csv_row);
		}

		int retval = spawnlp(_P_WAIT, getenv("MCODE_UTIL"), getenv("MCODE_UTIL"), mcode.c_str(), NULL);
		if ( retval != 0 )
		{
			cerr << "GetEncryptedMCode() 'MCODE_UTIL' Error - Retval was " << retval << "\n";
			cerr << "MCode passed = '" << mcode << "'\n\n";
			assert(0);
		}
	}

	const char* filename = getenv("MCODE_TMP_FILE");
	ifstream filestr(filename);
	if ( !filestr.is_open() )
	{
		cerr << endl << "GetEncryptedMCode() - Failed to open file " << filename << endl;
		assert(0);
	}

	apstring line;
	while ( !filestr.eof() )
	{
		getline(filestr, line);
		if( line.length() == 0 ) // Skip blank lines.
			continue;
		else
		{
			line = line.substr(line.find("=")+1, line.length());
			break;
		}
	}
	filestr.close();
	return line;
}
