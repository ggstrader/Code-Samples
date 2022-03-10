#ifndef SPREADSHEET_CELL
#define SPREADSHEET_CELL

#include <string>
#include <vector>
#include "Utilities.h"
enum class EType { START, EDIT, REVERT };
struct CellEdit {
	EType type;
	std::string contents;
	CellEdit(std::string c = "", EType t = EType::START) { contents = c; type = t; }
};

class Cell {
	std::vector<CellEdit> history;
public:
	Cell(std::string n) {
		name = n;
		revert_indx = 0;
		history.push_back(CellEdit());
	}
	Cell(std::string n, std::string contents) {
		name = n;
		revert_indx = 0;
		history.push_back(CellEdit(contents));
	 }
	std::string GetCurrent() {return history.back().contents;}
	void Edit(const std::string &);
	void Revert(std::string &);
	void Undo(std::string &);
	std::string name;

private:
	int findLastEdit(int);
	int revert_indx;
};

#endif // SPREADSHEET_CELL
