#include "stringutil.h"

StringArray SplitWords(apstring str, apstring delim)
{
	StringArray list;
	do
	{
		int pos = str.find(delim);
		if ( pos == npos )
			pos = str.length();

		apstring newstr = str.substr(0, pos);
		cout << "NewStr = (0,"<<pos << ")   " << newstr << endl;
		str = str.substr(pos+delim.length(), str.length());

		list.append(newstr);
	}
	while ( str.length() > 0 );

	return list;
}

apstring RemoveSpaces(apstring str, unsigned flags)
{
	if ( flags & CLR_ALL_SPACES )
	{
		int pos = str.find(" ");
		while ( pos != npos )
		{
			apstring begin = str.substr(0, pos);
			apstring end = str.substr(pos+1, str.length());
			str = begin+end;

			pos = str.find(" ");
		}
	}

	if ( flags & CLR_LEADING_SPACES )
	{
		while ( str[0] == ' ' )
			str = str.substr(1, str.length());
	}

	if ( flags & CLR_TRAILING_SPACES )
	{
		while ( str[str.length()] == ' ' )
			str = str.substr(0, str.length()-1);
	}

	if ( flags & CLR_DOUBLE_SPACES )
	{
		int pos = str.find("  ");
		while ( pos != npos )
		{
			apstring begin = str.substr(0, pos);
			apstring end = str.substr(pos+1, str.length());
			str = begin+end;

			pos = str.find("  ");
		}
	}

	return str;
}

