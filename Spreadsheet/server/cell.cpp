#include "cell.h"
#include <iterator>

using namespace std;

void Cell::Edit(const string &contents) {
	revert_indx = findLastEdit(history.size() - 1);
	history.push_back(CellEdit(contents, EType::EDIT));
}

/*NOTE: Revert and Undo messages don't have contents so they use the empty
contents-slot of the message like an out parameter to return the change. The
out param MUST be changed*/

void Cell::Revert(string &out_changed) {
	out_changed = history[revert_indx].contents;
	revert_indx = findLastEdit(revert_indx - 1);
	history.push_back(CellEdit(out_changed, EType::REVERT));
}

void Cell::Undo(string &out_changed) {
	if(!history.empty())
		history.pop_back();
	out_changed = GetCurrent();
	revert_indx = findLastEdit(history.size()-1);
}

int Cell::findLastEdit(int start) {
	for(int i = start; i > 0; i--)
	{
		if(history[i].type == EType::EDIT)
			return i;
	}
	return 0;
}
