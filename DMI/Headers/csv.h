#ifndef __PARSECSV_H
#define __PARSECSV_H

/* CSV File Parser
 * Last Update: 8/1/2007
 * Austin Heilman
 *
 * Notes:
 *	 For now this only provides access as file->row->cell
 *	 In the future when there is more time it will support file->column->cell
 *	 which would make it easier to find duplicate cells.
 *
 * To-do:
 * overload the [ ] operator for the classes to get a row / cell using it.
 * example CSVFile[4] gets the 4th row, CSVFile[1][2] would get the first row and second cell.
 *
 */

#include "apstring.h"

class CSVFile;
class CSVRow;
class CSVCell;

class CSVFile
{
	friend class CSVRow;

	public:
		CSVFile(const char* filename);
		~CSVFile();
		void Display();

		bool Open(const char *filename);

		unsigned GetColNum(apstring col_name);
		apstring GetColName(unsigned col_num);

		CSVRow* GetRow(unsigned num);
		CSVRow* AddNewRow(apstring unparsed);
		CSVRow* AddNewRow();

		unsigned GetNumRows();
		unsigned GetNumColumns();

		apstring GetFileName();

		CSVRow* GetHeadRow();
		CSVRow* GetTailRow();

		int GetColumnPos(apstring name);
		apstring GetColumnName(unsigned pos);
		CSVCell* GetHeaderCell(unsigned pos);

		int NumMatchesInCSVCol(int col_num, apstring match);

	protected:
		apstring _filename;
		unsigned _num_rows;
		unsigned _num_columns;

		// Used to track the rows in the file
		CSVRow *_head_row, *_tail_row;
};



class CSVRow
{
	friend class CSVFile;

	public:
		CSVRow(CSVFile* fileptr, unsigned line_num = 0);
		~CSVRow();

		bool IsHeader();
		void SetHeader(bool value);
		unsigned GetLineNum();
		void SetLineNum(unsigned line_num = 0);

		CSVFile* GetFileRef();
		void SetFileRef(CSVFile* fileptr);

		CSVCell* AddNewCell(apstring value);
		CSVCell* AddNewCell();
		CSVCell* GetCell(unsigned pos);
		CSVCell* GetHeadCell();
		CSVCell* GetTailCell();

		unsigned GetColumnPos(apstring name);
		apstring GetColumnName(unsigned pos);

		CSVRow* GetPreviousRow();
		CSVRow* GetNextRow();

		void ParseLine(apstring unparsed);

	protected:
		CSVFile* _fileptr;
		bool _header;
		unsigned _line_num;

		// Used to navigate cells in the row.
		CSVCell *_head_cell, *_tail_cell;
		// Used to navigate rows in a file.
		CSVRow *_next_row, *_previous_row;
};




class CSVCell
{
	friend class CSVRow;

	public:
		CSVCell(CSVRow* row_ptr);
		~CSVCell();

		CSVCell* GetHeaderCell();
		unsigned GetColumnPosition();

		apstring GetValue();
		void SetValue(apstring value);

		CSVCell* GetPreviousCell();
		CSVCell* GetNextCell();

	protected:
		void SetHeaderCell(CSVCell* headercell);

		CSVRow* _row_ptr;		// Points to the row the cell is on.
		CSVCell* _headercell;	// Points to the header cell (to get the column name)
		apstring _value;		// Value of the cell

		// Used to navigate cells in a row.
		CSVCell *_previous_cell, *_next_cell;
};

#endif // __PARSECSV_H

