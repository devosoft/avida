// This program takes a series of files, and a column id for each.  It then
// sends to standard out a matrix where each row is a histogram of the
// relevant column.

#include <iostream>

#include "../../tools/tMatrix.h"

#include "../../tools/cString.h"
#include "../../tools/cFile.h"
#include "../../tools/cInitFile.h"


using namespace std;


int main(int argc, char * argv[])
{
  if (argc < 4) {
    cerr << "Format: " << argv[0] << " key_col count_col file1 [file2 ...]" << endl;
    return 1;
  }

  const cString col_str(argv[1]);
  const cString count_str(argv[2]);
  const int col_id = col_str.AsInt();
  const int count_id = count_str.AsInt();
  const int max_id = col_id > count_id ? col_id : count_id;
  const int num_files = argc - 3;

  tArray< tArray<int> > count_arrays(num_files+1);

  for (int i = 0; i < num_files; i++) {
    cerr << "Processing: '" << argv[i+3] << "'" << endl;

    cInitFile file(argv[i+3]);
//     file.Load();
//     file.Compress();
//     file.Close();

    tArray<int> & site_count = count_arrays[i];
    site_count.SetAll(0);
    
    int total_count = 0;

    for (int line_num = 0; line_num < file.GetNumLines(); line_num++) {
      cString cur_line = file.GetLine(line_num);
      int value = -1;
      int count = -1;
      
      int cur_col = 1;
      while (cur_col <= max_id) {
	cString cur_entry(cur_line.PopWord());
	if (cur_col == col_id)  value = cur_entry.AsInt();
	else if (cur_col == count_id) count = cur_entry.AsInt();
	cur_col++;
      }

      if (value < 0) {
	cout << "Error in file '" << argv[i+3]
	     << "': Only posive values allowed." << endl
	     << "   (line = " << line_num+1
	     << ",  count = " << count
	     << ",  value = '" << value << "')" << endl;
	return 1;
      }
      if (count < 0) {
	cout << "Error in file '" << argv[i+3]
	     << "': Only posive abundance allowed" << endl;
	return 1;
      }

      if (site_count.GetSize() <= value) site_count.Resize(value+1, 0);
      site_count[value] += count;
      total_count += count;
    }
    cerr << "  Total count = " << total_count << endl;
  }

  // Now that we have all of the arrays, figure out which one is the largest
  // and normalize all sizes.
  int max_size = 0;
  for (int i = 0; i < num_files; i++) {
    if (count_arrays[i].GetSize() > max_size) {
      max_size = count_arrays[i].GetSize();
    }
  }

  for (int i = 0; i < num_files; i++) {
    count_arrays[i].Resize(max_size, 0);
  }

  // And now print it all out.
  cerr << num_files << " rows, " << max_size << " columns." << endl;
  for (int i = 0; i < num_files; i++) {
    for (int j = 0; j < max_size; j++) {
      tArray<int> & site_array = count_arrays[i];
      cout << site_array[j] << " ";
    }
    cout << endl;
  }
}
