// This program is intended to convert a non-matlab formated datafile to one
// that matlab can read.  It does this by converting all columns to floating
// point numbers.  If a column is just text, it gets truncated down to a
// zero.
//
// Ideally: if rows have different numbers of columns, they get padded
// by zeros.

#include <iostream>
using namespace std;

#include "../tools/cInitFile.h"

int main(int argc, char * argv[])
{
  if (argc > 2) {
    cerr << "Format: " << argv[0] << " [filename]" << endl;
    return 1;
  }

  cInitFile in_file;

  if (argc == 1) {
    in_file.LoadStream(cin);
  }
  else {
    cString filename(argv[1]); 
    in_file.Open(filename);
    in_file.Load();
  }

  in_file.Compress();
  // in_file.Close();

  for (int i = 0; i < in_file.GetNumLines(); i++) {
    cString cur_line = in_file.GetLine(i);;
    while (cur_line.GetSize() > 0) {
      cout << cur_line.PopWord().AsDouble() << " ";
    }
    cout << endl;
  }

  return 0;
}

