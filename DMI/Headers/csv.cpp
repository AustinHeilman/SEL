#include <stdlib.h>
#include <assert.h>
#include <fstream.h>
#include "csv.h"

// **********
// CSVFile:: Functions
// **********

CSVFile::CSVFile(const char* filename)
{
	_head_row = NULL;
	_tail_row = NULL;
	_filename = filename;
	_num_rows = 0;
	_num_columns = 0;
	Open(filename);
}

CSVFile::~CSVFile()
{
	CSVRow* current = _head_row;
	while ( current != NULL )
	{
		current = current->GetNextRow();
		delete _head_row;
		_head_row = current;
	}
	_head_row = current = _tail_row = NULL;
}

void CSVFile::Display()
{
	cout << "Displaying CSV file contents (" << _filename << ")\n";

	CSVRow* cur_row = _head_row;
	while ( cur_row != NULL )
	{
		cout << "[" << cur_row->GetLineNum() << "] '";
		CSVCell* cur_cell = cur_row->GetHeadCell();
		while ( cur_cell != NULL )
		{
			cout << cur_cell->GetValue() << ",";
			cur_cell = cur_cell->GetNextCell();
		}
		cout << endl;
		cur_row = cur_row->GetNextRow();
	}
}

bool CSVFile::Open(const char* filename)
{
	ifstream filestr(filename);
	if ( !filestr.is_open() )
	{
		cerr << endl << "CSVFile::Open() - Failed to open file " << filename << endl;
		assert(0);
		return false;
	}

	unsigned line_count = 0;
	apstring line;
	while ( !filestr.eof() )
	{
		getline(filestr, line);
		if( line.length() == 0 ) // Skip blank lines.
			continue;
		AddNewRow(line);
	}
	filestr.close();

	return true;
}

CSVRow* CSVFile::GetRow(unsigned num)
{
	CSVRow* current = _head_row;
	while ( current != NULL )
	{
		if ( current->GetLineNum() == num )
			return current;

		current = current->GetNextRow();
	}

	return NULL;
}

CSVRow* CSVFile::AddNewRow(apstring unparsed)
{
	CSVRow* tmp = AddNewRow();
	tmp->ParseLine(unparsed);
	return tmp;
}

CSVRow* CSVFile::AddNewRow()
{
	CSVRow* tmp_new = new CSVRow(this, _num_rows++);

	if ( _head_row == NULL )
	{
		_head_row = _tail_row = tmp_new;
	}
	else
	{
		_tail_row->_next_row = tmp_new;
		tmp_new->_previous_row = _tail_row;
		_tail_row = tmp_new;
	}
	return _tail_row;
}

unsigned CSVFile::GetNumRows()
{
	return _num_rows;
}

unsigned CSVFile::GetNumColumns()
{
	return _num_columns;
}

apstring CSVFile::GetFileName()
{
	return _filename;
}

CSVRow* CSVFile::GetHeadRow()
{
	return _head_row;
}

CSVRow* CSVFile::GetTailRow()
{
	return _tail_row;
}

int CSVFile::GetColumnPos(apstring colname)
{
	unsigned pos = 0;
	CSVRow* headrow = GetHeadRow();

	CSVCell* current = headrow->GetHeadCell();
	while ( current != NULL )
	{
		if ( current->GetValue() == colname )
			return pos;
		current = current->GetNextCell();
		pos++;
	}
	return -1;
}

apstring CSVFile::GetColumnName(unsigned pos)
{
	CSVCell* cell = GetHeaderCell(pos);
	if ( cell != NULL )
		return cell->GetValue();
	else
	{
		char buffer[10];
		itoa(pos, buffer, 10);
		return apstring("Error-")+apstring(buffer);
	}
}

CSVCell* CSVFile::GetHeaderCell(unsigned pos)
{
	CSVRow* headrow = GetHeadRow();
	assert(headrow != 0);

	CSVCell* current = headrow->GetHeadCell();
	while ( current != NULL )
	{
		if ( pos == 0 )
			return current;
		current = current->GetNextCell();
		pos--;
	}
	return NULL;
}

int CSVFile::NumMatchesInCSVCol(int col_num, apstring match)
{
	int matches = 0;
	for ( int i=1; i < GetNumRows(); i++ )
	{
		CSVRow* row = GetRow(i);
		CSVCell* cell = row->GetCell(col_num);

		if ( cell->GetValue() == match )
			matches++;
	}
	return matches;
}


// **********
// CSVRow:: Functions
// **********

CSVRow::CSVRow(CSVFile* fileptr, unsigned line_num)
{
	_fileptr = fileptr;
	_head_cell = NULL;
	_tail_cell = NULL;
	_previous_row = NULL;
	_next_row = NULL;
	_line_num = line_num;
	SetHeader(line_num==0);
}

CSVRow::~CSVRow()
{
	CSVCell* current = _head_cell;
	while ( current != NULL )
	{
		current = current->GetNextCell();
		delete _head_cell;
		_head_cell = current;
	}
	_head_cell = current = _tail_cell = NULL;
}

void CSVRow::ParseLine(apstring unparsed)
{
	apstring str = "";
	bool inquotes = false;
	CSVRow* headrow = _fileptr->GetRow(0);
	unsigned col_num = 0;

	for( size_t i=0; i < unparsed.length(); i++)
	{
		//To-Do:
		// Remove leading and trailing spaces
		//     Examples: xxx, xxx      xxx ,xxx      xxx , xxx
		// Allow quotes in quotes, and support single quotes

		char c = unparsed[i];
		if ( c=='\"' )
			inquotes = !inquotes; // Flip the value
		else
		{
			if ( c == ',' && !inquotes )
			{
				CSVCell* newcell = AddNewCell(str);
				newcell->SetHeaderCell(_fileptr->GetHeaderCell(col_num++));
				str = "";
			}
			else
				str += c;
		}
	}
	if ( str.length() > 0 )
	{
		CSVCell* newcell = AddNewCell(str);
		newcell->SetHeaderCell(_fileptr->GetHeaderCell(col_num++));
	}

	if ( col_num != _fileptr->GetNumColumns() )
	{
		cerr << "Error: " << _fileptr->GetFileName() << "[" << GetLineNum() << "] ";
		if ( col_num < _fileptr->GetNumColumns() )
			cerr << "num cells(" << col_num << ") < ("<<_fileptr->GetNumColumns()<<")" << endl;
		else
			cerr << "num cells(" << col_num << ") > ("<<_fileptr->GetNumColumns()<<")" << endl;
		assert(col_num == _fileptr->GetNumColumns());
	}

}

bool CSVRow::IsHeader()
{
	return _header;
}

void CSVRow::SetHeader(bool value)
{
	_header = value;
}

unsigned CSVRow::GetLineNum()
{
	return _line_num;
}

void CSVRow::SetLineNum(unsigned line_num)
{
	_line_num = line_num;
}

CSVFile* CSVRow::GetFileRef()
{
	return _fileptr;
}

void CSVRow::SetFileRef(CSVFile* fileptr)
{
	_fileptr = fileptr;
}

CSVCell* CSVRow::AddNewCell(apstring value)
{
	CSVCell* tmp = AddNewCell();
	tmp->SetValue(value);

	return tmp;
}

CSVCell* CSVRow::AddNewCell()
{
	CSVCell* tmp_new = new CSVCell(this);
	if ( IsHeader() )
		_fileptr->_num_columns++;

	if ( _head_cell == NULL )
	{
		_head_cell = _tail_cell = tmp_new;
	}
	else
	{
		_tail_cell->_next_cell = tmp_new;
		tmp_new->_previous_cell = _tail_cell;
		_tail_cell = tmp_new;
	}
	return _tail_cell;
}

CSVCell* CSVRow::GetCell(unsigned pos)
{
	CSVCell* current = _head_cell;
	unsigned xpos = 0;
	while ( current != NULL )
	{
		if ( xpos++ == pos )
			return current;

		current = current->_next_cell;
	}

	return NULL;
}

CSVCell* CSVRow::GetHeadCell()
{
	return _head_cell;
}

CSVCell* CSVRow::GetTailCell()
{
	return _tail_cell;
}

unsigned CSVRow::GetColumnPos(apstring name)
{
	return GetFileRef()->GetColumnPos(name);
}

apstring CSVRow::GetColumnName(unsigned pos)
{
	return GetFileRef()->GetColumnName(pos);
}

CSVRow* CSVRow::GetPreviousRow()
{
	return _previous_row;
}

CSVRow* CSVRow::GetNextRow()
{
	return _next_row;
}

// **********
// CSVCell:: Functions
// **********

CSVCell::CSVCell(CSVRow* row_ptr)
{
	_previous_cell = NULL;
	_next_cell = NULL;
	_row_ptr = row_ptr;
	_headercell = NULL;
	_value = "";
}

CSVCell::~CSVCell()
{
}

CSVCell* CSVCell::GetHeaderCell()
{
	return _headercell;
}

unsigned CSVCell::GetColumnPosition()
{
	unsigned pos = 0;
	CSVCell* current = GetHeaderCell();
	while ( current != NULL )
	{
		if ( current == this )
			return pos;
		current = current->GetNextCell();
		pos++;
	}
	return -1;
}

void CSVCell::SetHeaderCell(CSVCell* headercell)
{
	_headercell = headercell;
}

apstring CSVCell::GetValue()
{
	return _value;
}

void CSVCell::SetValue(apstring value)
{
	_value = value;
}

CSVCell* CSVCell::GetPreviousCell()
{
	return _previous_cell;
}

CSVCell* CSVCell::GetNextCell()
{
	return _next_cell;
}
