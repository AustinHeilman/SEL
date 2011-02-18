#include "fileutil.h"

FilesFound GetFilesInDir(apstring path, apstring filename, int attribute)
{
	FilesFound files;
	
	char end = path[path.length()-1];
	if ( end != '\\' && end != '/' )
		path += '\\';
	apstring match = path + filename;

	struct FIND *file = findfirst(match.c_str(), attribute);
	while ( file != NULL )
	{
		if ( !(int(file->name[0]) == 46) ) //Skip "." and ".."
		{
			if ( (file->attribute & attribute) )
			{
				FileFound tmp;
				tmp.attribute = file->attribute;
				tmp.path = path;
				tmp.name = file->name;
				tmp.time = file->time;
				tmp.date = file->date;
				tmp.size = file->size;

				files.append(tmp);
			}
		}
		file = findnext();
	}

	return files;
}
