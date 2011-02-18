#include "dmiinfo.h"
//
// This file contains functions for the DMIKey structure and utility functions
// DMI write/read functions are contained in dmi parser files.
//

DMIKey* GetDMIKey(apstring key, DMIKeys &dmikeys)
{
	for ( int i=0; i<dmikeys.size(); i++ )
	{
		if ( dmikeys[i].GetName() == key )
			return &dmikeys[i];
	}
	return NULL;
}

DMIKey* GetDMIKey(TextInput* entryptr, DMIKeys &dmikeys)
{
	for ( int i=0; i<dmikeys.size(); i++ )
	{
		if ( dmikeys[i].GetEntryRef() == entryptr )
			return &dmikeys[i];
	}
	return NULL;
}

//-------
// DMIKey:: Functions
//-------

DMIKey::DMIKey()
{
	DMIKey("", "");
}

DMIKey::DMIKey(apstring name, apstring value)
{
	SetName(name);
	SetDefaultValue(value);
	_entryptr = NULL;
}

void DMIKey::SetName(apstring name)
{
	_name = name;
}

void DMIKey::SetDefaultValue(apstring value)
{
	_def_value = value;
}

apstring DMIKey::GetName()
{
	return _name;
}

apstring DMIKey::GetDefaultValue()
{
	return _def_value;
}

apstring DMIKey::GetValue(CSVRow* csvrow)
{
	if ( _entryptr && !_entryptr->Orphan() )
		return _entryptr->GetStringValue();
	else if ( csvrow )
	{
		int pos = csvrow->GetColumnPos(GetName());
		if ( pos != -1 )
		{
			apstring val = csvrow->GetCell(pos)->GetValue();
			if ( val != "?" )
				return val;
		}
	}
	return GetDefaultValue();
}

void DMIKey::SetEntryRef(TextInput* entryptr)
{
	_entryptr = entryptr;
}

TextInput* DMIKey::GetEntryRef()
{
	return _entryptr;
}
