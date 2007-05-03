#include <iostream>

#include "tArray.h"
#include "cFile.h"
#include "cRandom.h"
#include "cString.h"
#include "cInitFile.h"
#include "cStringList.h"

using namespace std;


int main(int argc, char * argv[])
{
  if (argc != 6) {
    cerr << "Usage: " << argv[0]
	 << " [env_file] [task_count_pos] [start] [stop] [step]"
	 << endl
	 << "  [env_file] is the the environment file you are using." << endl
	 << "  [task_count] is the number of tasks active at once." << endl
	 << "  [start] is the first update to make a change at" << endl
	 << "  [stop] is the last update to make a change at" << endl
	 << "  [step] is the time between task changes." << endl
	 << endl;
    exit(1);
  }


  // Collect the information from the command line arguments.
  const cString env_filename(argv[1]);
  const cString task_count_str(argv[2]);
  const cString start_str(argv[3]);
  const cString stop_str(argv[4]);
  const cString step_str(argv[5]);

  const int task_count = task_count_str.AsInt();
  const int update_start = start_str.AsInt();
  const int update_stop  = stop_str.AsInt();
  const int update_step  = step_str.AsInt();

  // Load in the environment file that we're using...
  cInitFile env_file(env_filename);
  env_file.Load();
  env_file.Compress();

  // Create a container for the reaction names...
  cStringList reaction_list;

  // Collect the reaction names...
  for (int i = 0; i < env_file.GetNumLines(); i++) {
    cString cur_line = env_file.GetLine(i);
    cString line_type = cur_line.PopWord();
    line_type.ToUpper();                      // Make keyword into upper case.
    
    // Only pay attention to the REACTION commands...
    if (line_type != "REACTION") continue;

    // Grab the name of the reaction defined on this line.
    reaction_list.PushRear(cur_line.PopWord());
  }

  // Make sure everything is okay so far...
  const int num_tasks = reaction_list.GetSize();
  if (task_count >= num_tasks) {
    cerr << "Error: Only " << num_tasks << " tasks are available; cannot have "
	 << task_count << "rotate!" << endl;
    exit(1);
  }

  if (update_start >= update_stop) {
    cerr << "Error: Stop update (" << update_stop
	 << ") must be *after* start update (" << update_start
	 << ")!" << endl;
    exit(1);
  }

  // Let the user know what's going on
  cerr << "Generating events with " << task_count
       << " simultaneously active tasks out of " << num_tasks
       << " available." << endl
       << "...changes occur every " << update_step
       << " updates, from " << update_start
       << " until " << update_stop << "." << endl;

  // Now, generate the output file!
  cRandom random;

  // Start with turning on the initial tasks.
  tArray<bool> active_tasks(num_tasks);
  active_tasks.SetAll(false);

  for (int i = 0; i < task_count; i++) {
    int next_task = random.GetUInt(num_tasks);
    if (active_tasks[next_task] == true) {
      i--;
      continue;
    }
    active_tasks[next_task] = true;
  }

  // Now write out the initial state...
  for (int i = 0; i < num_tasks; i++) {
    cout << "u 0 set_reaction_value " << reaction_list.GetLine(i) << " ";
    if (active_tasks[i] == true) cout << "1";  // Mark active tasks as 1
    else cout << "-1"; // ...and non-active as -1.
    cout << endl;
  }

  // Loop through the updates making random changes at each step.
  for (int cur_update = update_start;
       cur_update <= update_stop;
       cur_update += update_step) {
    int next_task = random.GetUInt(num_tasks);
    while (active_tasks[next_task] == true) {
      next_task = random.GetUInt(num_tasks);
    }

    // Figure out which task is being replaced and do so.
    int old_task = random.GetUInt(num_tasks);
    while (active_tasks[old_task] == false) {
      old_task = random.GetUInt(num_tasks);
    }

    active_tasks[next_task] = true;
    active_tasks[old_task] = false;

    // Write out the replacement...
    cout << "u " << cur_update << " set_reaction_value "
	 << reaction_list.GetLine(old_task) << " -1" << endl;
    cout << "u " << cur_update << " set_reaction_value "
	 << reaction_list.GetLine(next_task) << " 1" << endl;
  }
}
