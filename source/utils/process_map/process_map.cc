// This program take in a series of map dumps and reprocesses them into a
// simpler format where colors get pre-chosen.

#include <iostream>
#include <fstream>

#include "../../tools/tMatrix.h"
#include "../../tools/tDictionary.h"


#include "../../tools/cString.h"
#include "../../tools/cFile.h"
#include "../../tools/cInitFile.h"
#include "../../tools/cRandom.h"

using namespace std;

// A dictionary of input settings.
tDictionary<int> value_dict;
tDictionary<cString> desc_dict;

void AddSetting(const cString & key, int value, const cString & desc) {
  value_dict.Add(key, value);
  desc_dict.Add(key, desc);
}

int main(int argc, char * argv[])
{
  // Setup the default settings...
  AddSetting("start", 0, "Start of frame window (exclusive; start is skipped)");
  AddSetting("step", 100, "Number of updates to skip between frames.");
  AddSetting("stop", 10000, "Last update to frame (inclusive)");
  AddSetting("seed", 1, "Random number seed");
  AddSetting("type", 0, "Type of files to load (0=genotype, 1=phenotype)");
  AddSetting("num_demes", 0, "How many demes in the population (0=none)");
  AddSetting("num_colors", 65, "Number of colors in map (Use 43 for bright colors)");
  AddSetting("threshold1", 10, "Abundance threshold for color shift.");
  AddSetting("threshold2", 100, "Abundance threshold for assigning unique color");

  if (argc == 1) {
    cerr << endl;
    cerr << "This program will load in Avida-generated files with the naming scheme" << endl;
    cerr << "grid_genotype_id.####.dat (where #### = update output)." << endl;
    cerr  << endl;
    cerr << "Format: " << argv[0] << " {settings...}" << endl;
    cerr << "Example: " << argv[0] << " start=1000 step=100 stop=2000" << endl;
    cerr << endl;
    cerr << "Available settings: " << endl;

    tList<cString> setting_list;  
    tList<int> value_list;
    value_dict.AsLists(setting_list, value_list);
    
    while(setting_list.GetSize()) {
      cString setting = *(setting_list.Pop());
      int value = *(value_list.Pop());
      cString desc;
      desc_dict.Find(setting, desc);
      cerr << "  " << setting << " (default = " << value << ") : "
	   << desc << endl;
    }

    cerr << endl;
    cerr << "To run with all defaults, type: " << argv[0] << " go" << endl;
    cerr << endl;
    return 1;
  }

  cerr << "Initializing..." << endl;

  // Load in the command line arguments.
  for (int arg_num = 1; arg_num < argc; arg_num++) {
    cString cur_arg(argv[arg_num]);

    // Ignore a "go" argument...
    if (cur_arg == "go") continue;

    // Otherwise, process the argument.
    cString key(cur_arg.Pop('='));

    if (value_dict.HasEntry(key) == false) {
      cerr << "ERROR: Unknown keyword '" << key << "'." << endl;
      exit(1);
    }

    if (cur_arg.GetSize() == 0 || cur_arg.IsNumeric() == false) {
      cerr << "ERROR: Keyword '" << key << "' must be set to numeric value."
	   << endl;
      exit(1);
    }

    int value = cur_arg.AsInt();
    value_dict.SetValue(key, value);
    cerr << "Setting '" << key << "' to " << value << "." << endl;
  }
  
  // Load in the status of all arguments.
  int UD_start, UD_step, UD_stop, seed, num_demes;
  int num_colors, threshold1, threshold2, type=0;

  value_dict.Find("start", UD_start);
  value_dict.Find("step", UD_step);
  value_dict.Find("stop", UD_stop);
  value_dict.Find("seed", seed);
  value_dict.Find("type", type);
  value_dict.Find("num_demes", num_demes);
  value_dict.Find("num_colors", num_colors);
  value_dict.Find("threshold1", threshold1);
  value_dict.Find("threshold2", threshold2);

  // Setup variables based on command-line arguments.
  cRandom random(seed);
  const int num_files = (UD_stop - UD_start)/ UD_step;

  cerr << "A total of " << num_files << " files needed." << endl;

  // Load in the first file to get some basic information about the grids.
  cString filename;
  if (type == 0) filename.Set("grid_genotype_id.%d.dat", (UD_step + UD_start));
  else filename.Set("grid_phenotype_id.%d.dat", (UD_step + UD_start));
  cInitFile file1(filename);
  // file1.Load();
  // file1.Close();

  const int height = file1.GetNumLines();
  const int width = file1.GetLine(0).CountNumWords();
  const int num_cells = height * width;

  const int deme_height = (num_demes > 1) ? (height / num_demes) : height;
  const int deme_size = deme_height * width;

  // Do a brute-force square root.
  int num_deme_cols = 1;
  if (num_demes > 1) {
    while (num_deme_cols * num_deme_cols < num_demes) num_deme_cols++;
  }
  int num_deme_rows = num_demes / num_deme_cols;

  // Print out some basic info...
  cerr << "World size = " << width << " x " << height << endl;
  if (num_demes > 1) {
    cerr << "Planning on " << num_demes << " demes, organized in a "
	 << num_deme_cols << "x" << num_deme_rows << " grid."
	 << endl;
  }

  // Build a matrix that can hold all of the information from all of the files.
  tMatrix<int> grid_matrix(num_files, num_cells);
  
  // Loop through all files, loading them in.
  int update = UD_start;
  int max_id = -1;
  const int file_step = (num_files > 50) ? num_files / 50 : 1;
  cerr << "LOADING FILES....................................." << endl;
  for (int file_id = 0; file_id < num_files; file_id++) {
    // Keep a status bar printing on the screen.
    if (file_id % file_step == 0) {
      cerr << '#';
      cerr.flush();
    }

    // Generate the filename...
    update += UD_step;
    if (type == 0) filename.Set("grid_genotype_id.%d.dat", update);
    else filename.Set("grid_phenotype_id.%d.dat", update);
    
    cInitFile file(filename);
    // file.Load();

    // Load in this file...
    cString cur_line;
    int cur_line_num = 0;
    for (int pos = 0; pos < num_cells; pos++) {
      if (cur_line.GetSize() == 0) cur_line = file.GetLine(cur_line_num++);
      const int cur_id = cur_line.PopWord().AsInt();
      if (cur_id > max_id) max_id = cur_id;
      grid_matrix(file_id, pos) = cur_id;
    }

    // file.Close();
  }
  cerr << endl << "LOADING COMPLETE" << endl;
  cerr << "Max ID = " << max_id << endl;

  // Do a second pass and collect some extra statistics.
  tArray<int> org_count(max_id+1);
  org_count.SetAll(0);
  cerr << "PROCESSING COUNTS................................." << endl;
  for (int frame_id = 0; frame_id < num_files; frame_id++) {
    // Keep a status bar printing on the screen.
    if (frame_id % file_step == 0) {
      cerr << '#';
      cerr.flush();
    }
    for (int pos = 0; pos < num_cells; pos++) {
      const int gen_id = grid_matrix(frame_id, pos);
      if (gen_id >= 0) org_count[gen_id]++;
    }
  }
  cerr << endl;

  // Choose colors for each genotype.
  // Color 0 -> No orgs or dead cell. (Black)
  // Color 1 -> One organism ever; probably lethal mutation (Dark Gray)
  // Color 2 -> 2-T1 organisms ever; unsuccessful genotrype (Light Gray)
  // Color 3 -> T1+1-T2 organisms ever; margenally successful genotype (White)
  // Colors 4+ -> T2+1+ orgs in genotypes; ideally unique color (num_colors)
  //
  // Where T1 = threshold1 and T2 = threshold2

  cerr << "CHOOSING COLORS..................................." << endl;
  tArray<int> color_array(max_id+1);
  for (int i = 0; i < color_array.GetSize(); i++) {
    int cur_count = org_count[i];
    if (cur_count == 0) color_array[i] = 0;
    else if (cur_count == 1) color_array[i] = 1;
    else if (cur_count <= threshold1) color_array[i] = 2;
    else if (cur_count <= threshold2) color_array[i] = 3;
    else {
      int color_choice = random.GetUInt(num_colors - 4) + 4;
      color_array[i] = color_choice;
    }
  }

  cerr << "OUTPUTING RESULTS................................." << endl;
  ofstream fp("avida_movie.m");
  fp << "'Data Loaded from File...'" << endl;

  for (int frame_id = 0; frame_id < num_files; frame_id++) {
    // Keep a status bar printing on the screen.
    if (frame_id % file_step == 0) {
      cerr << '#';
      cerr.flush();
    }

    // NORMAL MAPS: Print the full matrix.
    if (num_demes <= 1) {
      fp << "frame" << frame_id << " = [ ";
      for (int pos = 0; pos < num_cells; pos++) {
	if (pos > 0 && pos % width == 0) fp << "; ";
	const int gen_id = grid_matrix(frame_id, pos);
	if (gen_id == -1) fp << "0 ";
	else fp << color_array[gen_id] << " ";
      }
      fp << "];" << endl;
    }

    // DEME MAPS: A bit more complex to keep the demes orderly.
    else {
      fp << "frame" << frame_id << " = [ ";

      // Loop through each row of demes.
      for (int deme_row = 0; deme_row < num_deme_rows; deme_row++) {
	// Now loop through each row of cells within this row of demes...
	for (int cell_row = 0; cell_row < deme_height; cell_row++) {
	  // Within this row, loop through all columns of demes...
	  for (int deme_col = 0; deme_col < num_deme_cols; deme_col++) {
	    const int deme_id = deme_row * num_deme_cols + deme_col;
	    // Finally, loop through all columns of demes...
	    for (int cell_col = 0; cell_col < width; cell_col++) {
	      // Determine which cell we're working with here.
	      const int deme_pos = cell_row * width + cell_col;
	      const int cell_id = deme_id * deme_size + deme_pos;

	      // If this is beyond the last cell, just output black.
	      if (cell_id >= num_cells) {
		fp << "3 ";
		continue;
	      }

	      // If this cell is in the grid, figure out its color.
	      const int gen_id = grid_matrix(frame_id, cell_id);
	      if (gen_id == -1) fp << "0 ";
	      else fp << color_array[gen_id] << " ";
	    } // Cell col

	    // At the end of each row of cells in a deme, skip a space w/black.
	    fp << "3 ";
	  } // Deme col

	  // At the end of all of the deme columns, mark the end of the row.
	  fp << "; ";
	} // Cell row

	// After finishing a full row of demes, skip a line.
	const int matrix_width = num_deme_cols * (width+1);
	for (int i = 0; i < matrix_width; i++)  fp << "3 ";
	fp << "; ";
      } // Deme row

      // When we're done with everything, close the bracket.
      fp << "];" << endl;
    }

  }

  //    fp << endl << "mymapvar = [0 0 0; 0.25 0.25 0.25; 0.5 0.5 0.5; 0.75 0.75 0.75; 0 0 0.5625; 0 0 0.6250; 0 0 0.6875; 0 0 0.7500; 0 0 0.8125; 0 0 0.8750; 0 0 0.9375; 0 0 1.0000; 0 0.0625 1.0000; 0 0.1250 1.0000; 0 0.1875 1.0000; 0 0.2500 1.0000; 0 0.3125 1.0000; 0 0.3750 1.0000; 0 0.4375 1.0000; 0 0.5000 1.0000; 0 0.5625 1.0000; 0 0.6250 1.0000; 0 0.6875 1.0000; 0 0.7500 1.0000; 0 0.8125 1.0000; 0 0.8750 1.0000; 0 0.9375 1.0000; 0 1.0000 1.0000; 0.0625 1.0000 1.0000; 0.1250 1.0000 0.9375; 0.1875 1.0000 0.8750; 0.2500 1.0000 0.8125; 0.3125 1.0000 0.7500; 0.3750 1.0000 0.6875; 0.4375 1.0000 0.6250; 0.5000 1.0000 0.5625; 0.5625 1.0000 0.5000; 0.6250 1.0000 0.4375; 0.6875 1.0000 0.3750; 0.7500 1.0000 0.3125; 0.8125 1.0000 0.2500; 0.8750 1.0000 0.1875; 0.9375 1.0000 0.1250; 1.0000 1.0000 0.0625; 1.0000 1.0000 0; 1.0000 0.9375 0; 1.0000 0.8750 0; 1.0000 0.8125 0; 1.0000 0.7500 0; 1.0000 0.6875 0; 1.0000 0.6250 0; 1.0000 0.5625 0; 1.0000 0.5000 0; 1.0000 0.4375 0; 1.0000 0.3750 0; 1.0000 0.3125 0; 1.0000 0.2500 0; 1.0000 0.1875 0; 1.0000 0.1250 0; 1.0000 0.0625 0; 1.0000 0 0; 0.9375 0 0; 0.8750 0 0; 0.8125 0 0; 0.7500 0 0; 0.6875 0 0; 0.6250 0 0; 0.5625 0 0];" << endl;

    fp << endl << "mymapvar = [ 0 0 0; 0.25 0.25 0.25; 0.5 0.5 0.5; 0.75 0.75 0.75; 0.25 0 0; 0.5 0 0; 0.75 0 0; 1 0 0; 1 0.25 0.25; 1 0.5 0.5; 0 0.25 0; 0 0.5 0; 0 0.75 0; 0 1 0; 0.25 1 0.25; 0.5 1 0.5; 0 0 0.25; 0 0 0.5; 0 0 0.75; 0 0 1; 0.25 0.25 1; 0.5 0.5 1; 1 1 0; 1 1 0.5; 1 0 1; 1 0.25 1; 1 0.5 1; 0 1 1; 0.5 1 1; 1 0.5 0; 1 0 0.5; 0.5 0 1; 0 0.5 1; 1 0.25 0; 1 0 0.25; 0.25 0 1; 0 0.25 1; 1 0.75 0; 1 0 0.75; 0.75 1 0; 0 1 0.75; 0.75 0 1; 0 0.75 1; 0.75 0.25 0; 0.5 0.25 0; 0.25 0.75 0; 0.25 0.5 0; 0.5 0.5 0; 0.75 0.75 0; 0.75 0 0.25; 0.5 0 0.25; 0.25 0 0.75; 0.25 0 0.5; 0.5 0 0.75; 0.75 0 0.5; 0.5 0 0.5; 0.75 0 0.75; 0 0.75 0.25; 0 0.5 0.25; 0 0.25 0.75; 0 0.25 0.5; 0 0.5 0.75; 0 0.75 0.5; 0 0.5 0.5; 0 0.75 0.75 ];" << endl;





  
  const int x_midpoint = width/2;
  const int y_midpoint = height/2;
  const int output_size = (num_cells > 3600) ? 6 : 3;

  fp << "colormap(mymapvar);" << endl;

  for (int frame_id = 0; frame_id < num_files; frame_id++) {

    // Clear the figure before re-drawing it...
    fp << "clf" << endl;

    // Start by placing the grid for the current frame on the screen.
    fp << "pcolor_all( frame" << frame_id;

    // If we *don't* have demes, rotate the map around so the starting oranism
    // at 0,0 is centered.
    if (num_demes == 0) {
      fp << "(["
	 << y_midpoint+1 << ":" << height << " 1:" << y_midpoint
	 << "],["
	 << x_midpoint+1 << ":" << width << " 1:" << x_midpoint
	 << "])";
    }

    fp << " ); "
       << endl;

    // Set it up to look good:
    // - "shading faceted" draws lines between cells.
    // - "axis off" removes the axes.
    // - "caxis([0 64]);" makes sure colors ar econsistant between frames.
    // - "title(...)" sets the title of the frame to be the update number.
    fp << "shading faceted" << endl
       << "axis off" << endl
       << "caxis([0 " << (64) << "]);" << endl;

    const int UD_num = (frame_id + 1) * UD_step + UD_start;
    fp << "title('" << UD_num << "');" << endl;

    // Setup output size, which was determined by the number of cells we have.
    fp << "set(gcf, 'PaperPosition', [0.5 0.5 "
       << output_size << " " << output_size
       << "]);" << endl;

    // Finally, print it as a PNG frame.
    fp << "print -dpng frame" << frame_id << ".png" << endl;
  }

  cerr << endl << "PROCESSING COMPLETE" << endl;
}
