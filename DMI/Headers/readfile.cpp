#include "readfile.h"

apvector<apstring> ReadFile(const char* filename)
{
	apvector<apstring> file_lines;
	ifstream filestr(filename);
	if ( !filestr.is_open() )
	{
		cerr << endl << "ReadFile() - Failed to open file " << filename << endl;
		assert(0);
	}

	apstring line;
	while ( !filestr.eof() )
	{
		getline(filestr, line);
		if( line.length() == 0 ) // Skip blank lines.
			continue;
		else
			file_lines.append(line);
	}
	filestr.close();

	return file_lines;
}
