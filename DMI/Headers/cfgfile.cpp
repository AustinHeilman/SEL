#include "cfgfile.h"

CfgPair ParseCfgLine(apstring line)
{
	CfgPair pair;
	int pos;
	
	// First remove leading white space.
	pos = line.find_first_not_of(" \t");
	line = line.substr(pos, line.length());
	
	// Remove anything after a comment.
	if ( (pos = line.find("#")) != npos )
		line = line.substr(0, pos-1);
	else if ( (pos = line.find("//")) != npos )
		line = line.substr(0, pos-1);
		
	// Remove trailing white space.
	pos = line.find_last_not_of(" \t");
	if ( pos != npos )
		line = line.substr(0, pos+1);
	
	// Find the size of the first word and extract it
	pos = line.find_first_of(" \t");
	if ( pos == npos )
		pos = line.length();
	pair.first = line.substr(0, pos);
	line = line.substr(pos, line.length());
	
	// Find where the data begins
	pos = line.find_first_not_of(" \t");
	pair.second = line.substr(pos, line.length());
	
	return pair;
}


ConfigFile::ConfigFile(const char* filename)
{
	ReadConfigFile(filename);
}

ConfigFile::~ConfigFile()
{
	for ( unsigned i=0; i<_config_elems.size(); i++ )
	{
		if ( _config_elems[i] )
		{
			delete _config_elems[i];
			_config_elems[i] = NULL;
		}
	}
}

bool ConfigFile::ReadConfigFile(const char* filename)
{
	ifstream filestr(filename);
	if ( !filestr.is_open() )
	{
		cerr << "ConfigFile::ReadConfigFile() - Failed to open file '" << filename << "'\n";
		assert(0);
		return false;
	}
	_file_name = filename;

	unsigned line_num = 0;
	ConfigElem* cur_elem = NULL;
	bool in_elem = false;
	while ( !filestr.eof() )
	{
		line_num++;
		apstring line;
		getline(filestr, line);
		if( line.length() == 0 ) // Skip blank lines.
			continue;
		
		// Remove leading white space
		int pos = line.find_first_not_of(" \t");
		line = line.substr(pos, line.length());
		
		if ( line[0] == '#' ) // Skip comment lines
			continue;
		else if ( line.substr(0, 2) == "//" ) // Skip comment lines
			continue;
		else if ( !cur_elem )
		{
			CfgPair pair = ParseCfgLine(line);
			if ( pair.second.length() == 0 )
			{
				cerr << "ConfigFile::ReadConfigFile() - Fail on line " << line_num << endl;
				cerr << "Elem type '"<<pair.first<<"' with no name.\n";
				assert(0);
			}
			cur_elem = new ConfigElem(pair.first, pair.second);
			_config_elems.append(cur_elem);
		}
		else if ( line[0] == '{' )
		{
			if ( in_elem )
			{
				cerr << "ConfigFile::ReadConfigFile() - Fail on line " << line_num << endl;
				cerr << "Found an opening '{' brace, before closing previous elem.\n";
				assert(0);
			}
			if ( !cur_elem )
			{
				cerr << "ConfigFile::ReadConfigFile() - Fail on line " << line_num << endl;
				cerr << "Opening '{' found with no config elem name set.\n";
				assert(0);
			}
			in_elem = true;
		}
		else if ( line[0] == '}' )
		{
			cur_elem = NULL;
			in_elem = false;
		}
		else if ( cur_elem && in_elem )
		{
			CfgPair pair = ParseCfgLine(line);
			cur_elem->AddConfigLine(pair.first, pair.second);
		}
		else
		{
			cerr << "ConfigFile::ReadConfigFile() - Fail on line " << line_num << endl;
			cerr << "Found data outside of a config elem.\n";
			assert(0);
		}
	}
	filestr.close();

	return true;
}

ConfigElem* ConfigFile::GetConfigElem(apstring name) const
{
	for ( unsigned i=0; i < _config_elems.size(); i++ )
	{
		ConfigElem* elem_ptr = _config_elems[i];
		if ( elem_ptr->GetElemName() == name )
			return elem_ptr;
	}
	return NULL;
}

apvector<apstring> ConfigFile::GetElemNames()
{
	apvector<apstring> list;
	for ( unsigned i=0; i < _config_elems.size(); i++ )
	{
		list.append(_config_elems[i]->GetElemName());
	}
	return list;
}

CfgElemList ConfigFile::GetConfigElems()
{
	return _config_elems;
}


ConfigElem::ConfigElem(apstring type, apstring name)
{
	_elem_type = type;
	_elem_name = name;
}

ConfigElem::~ConfigElem()
{
	for ( unsigned i=0; i<_cfg_lines.size(); i++ )
	{
		if ( _cfg_lines[i] )
		{
			delete _cfg_lines[i];
			_cfg_lines = NULL;
		}
	}
}

apstring ConfigElem::GetElemType()
{
	return _elem_type;
}

apstring ConfigElem::GetElemName()
{
	return _elem_name;
}

unsigned ConfigElem::AddConfigLine(apstring key, apstring value)
{
	for ( unsigned i=0; i<_cfg_lines.size(); i++ )
	{
		ConfigLine* line_ptr = _cfg_lines[i];
		if ( line_ptr->GetPropertyName() == key )
		{
			// Existing key, add another value to it.
			line_ptr->AddValue(value);
			return i;
		}
	}
	// New key, append it to the list.
	ConfigLine* newline = new ConfigLine(key, value);
	_cfg_lines.append(newline);
	return _cfg_lines.size();
}

apstring ConfigElem::GetConfigString(apstring key)
{
	apvector<apstring> list = GetConfigStringArray(key);
	if ( list.size() > 0 )
		return list[0];
	else
		return "";
}

long ConfigElem::GetConfigInt(apstring key)
{
	apvector<apstring> list = GetConfigStringArray(key);
	if ( list.size() > 0 )
		return atol(list[0].c_str());
	else
		return 0;
}

double ConfigElem::GetConfigDouble(apstring key)
{
	apvector<apstring> list = GetConfigStringArray(key);
	if ( list.size() > 0 )
		return atof(list[0].c_str());
	else
		return 0.0;
}

apvector<apstring> ConfigElem::GetConfigStringArray(apstring key)
{
	for ( unsigned i=0; i<_cfg_lines.size(); i++ )
	{
		ConfigLine* line_ptr = _cfg_lines[i];
		if ( line_ptr->GetPropertyName() == key )
		{
			return line_ptr->GetPropertyValues();
		}
	}
	return apvector<apstring>(0);
}

apvector<apstring> ConfigElem::ListConfigElemProps()
{
	apvector<apstring> list;
	for ( unsigned i=0; i<_cfg_lines.size(); i++ )
		list.append(_cfg_lines[i]->GetPropertyName());
	
	return list;
}

ConfigLine::ConfigLine()
{
	ConfigLine("", "");
}

ConfigLine::ConfigLine(apstring key, apstring value)
{
	_line_key = key;
	AddValue(value);
}

apstring ConfigLine::GetPropertyName()
{
	return _line_key;
}

apstring ConfigLine::GetPropertyValue(unsigned pos)
{
	if ( pos > _values.size() )
		pos = _values.size();
	
	return _values[pos];
}

apvector<apstring> ConfigLine::GetPropertyValues()
{
	return _values;
}

void ConfigLine::AddValue(apstring value)
{
	_values.append(value);
}

