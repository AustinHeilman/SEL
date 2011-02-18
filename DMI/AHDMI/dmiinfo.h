#ifndef __DMIINFO_H
#define __DMIINFO_H

/* DMI Defaults Storage
 * Last Update: 10/21/2007
 * Austin Heilman
 *
 */

/*
 *  -- DMI Parsers --
 *  Each DMI tools program outputs the DMI information in a slightly
 *  different way.
 *  Use the preprocessor DMITOOL to control which DMI tool to run and
 *  how to parse the lines it produces.
 *  ALL dmi tools should be directed to output to C:\TMP\DMIINFO.TXT
 *
 *  Note: Preprocessor directives used to keep exe size to a minimum
 *        and support different DMI read/write systems.
 *
 *  # - Program         Description
 *  1 - RID.EXE 		(DMI-RID.cpp)
 *  2 - DMI.EXE			(DMI-QUANTADMI.CPP)
 *  3 - ROMINFO5.EXE	(DMI-ROMINFO5.CPP)
 *  4 - ROMINFO6.EXE	(DMI-ROMINFO6.CPP)
 *
 */
#define DMITOOL 3


#include "apstring.h"
#include "apvector.h"
#include "tui.h"
#include "csv.h"

struct DMIKey
{
	public:
		DMIKey();
		DMIKey(apstring name, apstring value);

		void SetName(apstring name);
		void SetDefaultValue(apstring value);

		apstring GetName();
		apstring GetDefaultValue();
		apstring GetValue(CSVRow* csvrow=NULL);

		void SetEntryRef(TextInput* entryptr);
		TextInput* GetEntryRef();

	protected:
		apstring _name;
		apstring _def_value;
		TextInput* _entryptr;
};
typedef apvector<DMIKey> DMIKeys;

DMIKey* GetDMIKey(apstring key, DMIKeys &dmikeys);
DMIKey* GetDMIKey(TextInput* entryptr, DMIKeys &dmikeys);

// Functions in DMI-XXX.cpp files for external DMI program handling
apstring GetParserVersion();
DMIKeys ReadDMIDefaults();
DMIKeys ReadDMIDefaults(DMIKeys &dmi_keys);
apstring GetStructureField(apvector<apstring> &file_lines, apstring &key);
bool ValidateDMIEntry(DMIKey* dmikey);
apstring GetEncryptedMCode(DMIKeys &dmi_keys, CSVRow* csv_row);
void CreateDMIDefaultsFile();
void WriteSystemDMI(DMIKeys &dmi_keys, CSVRow* csv_row);

#endif // __DMIINFO_H
