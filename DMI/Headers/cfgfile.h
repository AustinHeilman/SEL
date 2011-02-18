#ifndef CFGFILE_H
#define CFGFILE_H

#include <fstream.h>
#include <stdlib.h>
#include "apstring.h"
#include "apvector.h"

/* CfgFile.h
 * Austin Heilman
 * Read / Parse Config Files
 *
 */

class ConfigElem;
class ConfigLine;

typedef apvector<ConfigElem*> CfgElemList;
typedef apvector<ConfigLine*> CfgLineList;

struct CfgPair
{
	public:
		CfgPair() { first=""; second=""; }
		apstring first;
		apstring second;
};

class ConfigFile
{
	public:
		ConfigFile(const char* filename);
		~ConfigFile();

		bool ReadConfigFile(const char* filename);
		ConfigElem* GetConfigElem(apstring elem_name) const;
		apvector<apstring> GetElemNames();
		CfgElemList GetConfigElems();

	protected:
		CfgElemList _config_elems;
		const char* _file_name;
};

class ConfigElem
{
	public:
		ConfigElem(apstring type, apstring name);
		~ConfigElem();

		apstring GetElemType();
		apstring GetElemName();

		unsigned AddConfigLine(apstring key, apstring value);
		
		apstring GetConfigString(apstring key);
		long GetConfigInt(apstring key);
		double GetConfigDouble(apstring key);
		apvector<apstring> GetConfigStringArray(apstring key);
		apvector<apstring> ListConfigElemProps();
		
	protected:
		apstring _elem_name, _elem_type;
		CfgLineList _cfg_lines;
};

class ConfigLine
{
	public:
		ConfigLine();
		ConfigLine(apstring name, apstring value);
		apstring GetPropertyName();
		apstring GetPropertyValue(unsigned pos=0);
		apvector<apstring> GetPropertyValues();
		void AddValue(apstring value);
		
	protected:
		apstring _line_key;
		apvector<apstring> _values;
};

CfgPair ParseCfgLine(apstring line);

#endif // CFGFILE_H
